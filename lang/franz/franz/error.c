#ifndef lint
static char *rcsid =
   "$Header: /a/cvs/386BSD/ports/lang/franz/franz/error.c,v 1.1 1994/03/18 16:26:15 jkh Exp $";
#endif

/*
 * 	error.c				$Locker:  $
 * error handler
 *
 * (c) copyright 1982, Regents of the University of California
 */


#include "global.h"
#include "frame.h"
#include "catchfram.h"

static lispval	IEargs[5];
static int	IElimit;

/* error
 * this routine is always called on a non-fatal error.  The first argu-
 * ment is printed out.  The second a boolean flag indicating if the
 * error routine is permitted to return a pointer to a lisp value if
 * the "cont" command is executed.
 */
 
/* error from lisp C code, this temporarily replaces the old error
 * allowing us to interface with the new errset scheme with minimum
 * difficulty.  We assume that an error which comes to this routine
 * is of an "undefined error type" ER%misc .  Soon all calls to this
 * routine will be removed.
 *
 */

lispval
error(mesg,contvl)
char *mesg;
int contvl;
{
    lispval errorh();

    return(errorh(Vermisc,mesg,nil,contvl,0));
}


/* new error handler, works with errset 
 * 
 * call is errorh(type,message,valret,contuab) where
 * type is an atom which classifys the error, and whose clb, if not nil
 * is the name of a function to call to handle the error.
 * message is a character string to print to describe the error
 * valret is the value to return to an errset if one is found,
 * and contuab is non nil if this error is continuable.
 */
 

/* VARARGS5 */
static lispval
Ierrorh(type,message,valret,contuab,uniqid)
lispval type,valret;
int uniqid,contuab;
char *message;
{
 	register struct frame *curp, *uwpframe = (struct frame *)0; 
	register lispval handy;
	lispval *work = IEargs; 
	int limit = IElimit;
	int pass, curdepth;
	lispval Lread(), calhan();
	lispval contatm;
	lispval handy2;
	extern struct frame *errp;
	pbuf pb;
	Savestack(2);

	contatm = (contuab == TRUE ? tatom : nil);

	/* if there is a catch every error handler */
	if((handy = Verall->a.clb) != nil)	
	{
	    handy = Verall->a.clb;
	    Verall->a.clb = nil;		/* turn off before calling */
	    handy = calhan(limit,work,type,uniqid,contatm,message,handy);
	    if(contuab && (TYPE(handy) == DTPR))
		return(handy->d.car);
	}

	if((handy = type->a.clb) != nil)	/* if there is an error handler */
	{
	    handy = calhan(limit,work,type,uniqid,contatm,message,handy);
	    if(contuab && (TYPE(handy) == DTPR))
		return(handy->d.car);
	}

	pass = 1;
	/* search stack for error catcher */
  ps2:

	for (curp = errp ; curp != (struct frame *) 0 ; curp = curp->olderrp)
	{
	   if(curp->class == F_CATCH) 
	   {
		/* 
		 * interesting catch tags are ER%unwind-protect, generated
		 * by unwind-protect and ER%all, generated by errset
		 */
		if((pass == 1) && (curp->larg1 == Veruwpt))
		{
		    uwpframe = curp;
		    pass = 2;
		    goto ps2;
		}
		else if(curp->larg1 == Verall)
		{
		    /* 
		     * have found an errset to jump to. If there is an
		     * errset handler, first call that.
		     */
		    if((handy=Verrset->a.clb) != nil)
		    {
			calhan(limit,work,type,uniqid,contatm,message,handy);
		    }

		    /*
		     * if there is an unwind-protect then go to that first.
		     * The unwind protect will return to errorh after
		     * it has processed its cleanup forms.
		     * assert: if pass == 2 
		     *		then there is a pending unwind-protect
		     */
		     if(uwpframe != (struct frame *)0)
		     {
			/*
			 * generate form to return to unwind-protect 
			 */
			protect(handy2 = newdot());
			handy2->d.car = Veruwpt;
			handy = handy2->d.cdr = newdot();
			handy->d.car = nil;		/* indicates error */
			handy = handy->d.cdr = newdot();
			handy->d.car = type;
			handy = handy->d.cdr = newdot();
			handy->d.car = matom(message);
			handy = handy->d.cdr = newdot();
			handy->d.car = valret;
			handy = handy->d.cdr = newdot();
			handy->d.car = inewint(uniqid);
			handy = handy->d.cdr = newdot();
			handy->d.car = inewint(contuab);
			while (limit-- > 0)	/* put in optional args */
			{  handy = handy->d.cdr = newdot();
			   handy->d.car = *work++;
			}
			lispretval = handy2;		/* return this as value */
			retval = C_THROW;
			Iretfromfr(uwpframe);
			/* NOTREACHED */
		    }
		    /*
		     * Will return to errset
		     * print message if flag on this frame is non nil
		     */
		    if(curp->larg2 != nil)
		    {
			printf("%s  ",message);
			while(limit-->0) {
			    printr(*work++,stdout);
			    fflush(stdout);
			}
			fputc('\n',stdout);
			fflush(stdout);
		    }

		    lispretval = valret;
		    retval = C_THROW;		/* looks like a throw */
		    Iretfromfr(curp);
		}
	    }
	}
	    
	/* no one will catch this error, we must see if there is an
	   error-goes-to-top-level catcher */
	
	if (Vertpl->a.clb != nil)
	{
	    
	    handy = calhan(limit,work,type,uniqid,contatm,message,Vertpl->a.clb);
	    if( contuab  && (TYPE(handy) == DTPR))
		   return(handy->d.car);
	}

	/* at this point, print error message and break, just like
	   the current error scheme */
	printf("%s ",message);
	while(limit-->0) {
		printr(*work++,stdout);
		fflush(stdout);
	}


	/* If automatic-reset is set
	 * we will now jump to top level, calling the reset function
	 * if it exists, or using the c rest function if it does not 
	 */

	if(Sautor)
	{
		if ((handy = reseta->a.fnbnd) != nil)
		{
			lispval Lapply();
			lbot = np;
			protect(reseta);
			protect(nil);
			Lapply();
		}
		Inonlocalgo(C_RESET,inewint(0),nil);
		/* NOTREACHED */
	}
	
	/*
	 * no one wants the error.  We set up another read-eval-print
	 * loop. The user can get out of this error by typing (return 'val)
	 * if the error is continuable.  Normally this code be replaced
	 * by more clever lisp code, when the full lisp is built
	 */

	errp = Pushframe(F_PROG,nil,nil);

	if(TYPE(Verdepth->a.clb) != INT)
	{
		curdepth = 1;
	}
	else curdepth = 1 + Verdepth->a.clb->i;
	PUSHDOWN(Verdepth,inewint(curdepth));

	switch(retval) {
	case C_RET:	/* 
			 * attempt to return from error
			 */
			if(!contuab) error("Can't continue from this error",
						  FALSE);
			popnames(errp->svbnp);
			errp = Popframe();
			Restorestack();
			return(lispretval);

	case C_GO:	/*
			 * this may look like a valid prog, but it really
			 * isn't, since go's are not allowed.  Let the
			 * user know.
			 */
			error("Can't 'go' through an error break",FALSE);
			/* NOT REACHED */

	case C_INITIAL: /*
			  * normal case, just fall through into read-eval-print
			  * loop
			  */
			break;
	}
	lbot = np;
	protect(P(stdin));
	protect(eofa);

	while(TRUE) {
		
		fprintf(stdout,"\n%d:>",curdepth);
		dmpport(stdout);
		vtemp = Lread();
		if(vtemp == eofa) franzexit(0);
		printr(eval(vtemp),stdout);
	}
	/* NOTREACHED */
}

lispval
errorh(type,message,valret,contuab,uniqid)
lispval type,valret;
int uniqid,contuab;
char *message;
{
	IElimit = 0;
	Ierrorh(type,message,valret,contuab,uniqid);
	/* NOTREACHED */
}

lispval
errorh1(type,message,valret,contuab,uniqid,arg1)
lispval type,valret,arg1;
int uniqid,contuab;
char *message;
{
	IElimit = 1;
	IEargs[0] = arg1;
	Ierrorh(type,message,valret,contuab,uniqid);
	/* NOTREACHED */
}

lispval
errorh2(type,message,valret,contuab,uniqid,arg1,arg2)
lispval type,valret,arg1,arg2;
int uniqid,contuab;
char *message;
{
	IElimit = 2;
	IEargs[0] = arg1;
	IEargs[1] = arg2;
	Ierrorh(type,message,valret,contuab,uniqid);
	/* NOTREACHED */
}

lispval
calhan(limit,work,type,uniqid,contuab,message,handler)
register lispval *work;
lispval handler,type,contuab;
register limit;
register char *message;
int uniqid;
{
	    register lispval handy;
	    Savestack(4);
	    lbot = np;
	    protect(handler);		/* funcall the handler */
	    protect(handy = newdot());		/* with a list consisting of */
	    handy->d.car = type;			/* type, */
	    handy = (handy->d.cdr = newdot());
	    handy->d.car = inewint(uniqid);	/* identifying number, */
	    handy = (handy->d.cdr = newdot());
	    handy->d.car = contuab;
	    handy = (handy->d.cdr = newdot());
	    handy->d.car = matom(message);	/* message to be typed out, */
	    while(limit-- > 0)
	    {					/* any other args. */
		    handy = handy->d.cdr = newdot();
		    handy->d.car = *work++;
	    }
	    handy->d.cdr = nil;

	    handy = Lfuncal();
	    Restorestack();
	    return(handy);
}

/* lispend **************************************************************/
/* Fatal errors come here, with their epitaph.				*/
lispend(mesg)
	char	mesg[];
	{
	dmpport(poport);
	fprintf(errport,"%s\n",mesg);
	dmpport(errport);
	franzexit(0);
	/* NOT REACHED */
	}

/* namerr ***************************************************************/
/* handles namestack overflow, at present by simply giving a message	*/

namerr()
{
	if((nplim = np + NAMINC) > orgnp + NAMESIZE) 
	{  
	  printf("Unrecoverable Namestack Overflow, (reset) is forced\n");
	  fflush(stdout);
	  nplim = orgnp + NAMESIZE - 4*NAMINC;
	  lbot = np = nplim - NAMINC;
	  protect(matom("reset"));
	  Lfuncal();
	}
	error("NAMESTACK OVERFLOW",FALSE);
	/* NOT REACHED */
}

binderr()
{
	bnp -= 10;
	error("Bindstack overflow.",FALSE);
	/* NOT REACHED */
}

rtaberr()
{
	bindfix(Vreadtable,strtab,nil);
	error("Illegal read table.",FALSE);
	/* NOT REACHED */
}
xserr()
{
	error("Ran out of alternate stack",FALSE);
}
badmem(n)
{
	char errbuf[256];

	sprintf(errbuf,"Attempt to allocate beyond static structures (%d).",n);
	error(errbuf,FALSE);
	/* NOT REACHED */
}
argerr(msg)
char *msg;
{
	errorh1(Vermisc,"incorrect number of args to",
				  nil,FALSE,0,matom(msg));
	/* NOT REACHED */
}

lispval Vinterrfcn = nil;

/*
 * wnaerr - wrong number of arguments to a compiled function hander
 * called with the function name (symbol) and a descriptor of the
 * number of arguments that were expected.  The form of the descriptor
 * is (considered as a decimal number) xxyy where xx is the minumum
 * and yy-1 is the maximum.  A maximum of -1 means that there is no
 * maximum.
 *
 */
wnaerr(fcn,wantargs)
lispval fcn;
{
    if (Vinterrfcn == nil)
    {
	Vinterrfcn = matom("int:wrong-number-of-args-error");
    }
    if (Vinterrfcn->a.fnbnd != nil)
    {
	protect(fcn);
	protect(inewint(wantargs / 1000));	  /* min */
	protect(inewint((wantargs % 1000) - 1));  /* max */
	Ifuncal(Vinterrfcn);
	error("wrong number of args function should never return ", FALSE);
    }

    errorh1(Vermisc,"wrong number of arguments to ",nil,FALSE,0,fcn);
}

	
    

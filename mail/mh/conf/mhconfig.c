/* mhconfig.c - configure MH */
/* cc [-DSYS5] mhconfig.c -o mhconfig */
#ifndef	lint
static char ident[] = "@(#)mhconfig.c,v 1.1.1.1 1993/01/30 04:39:59 jtc Exp";
#endif	/* lint */

#include <ctype.h>
#include <stdio.h>
#ifdef	hpux
#define	SYS5
#endif
#ifdef	SYS5
#define	index	strchr
#define	rindex	strrchr
#endif /* SYS5 */

static void insopt(), mkopts();

extern	char *strcpy(), *malloc(), *index(), *rindex();
#ifdef	notdef
extern	char *sprintf();	/* rely on stdio.h to declare this */
#endif

#define	BANNER	"This is %s, for generic UNIX (MH.6 configuration program)\n\n"


#define	SED		"config.sed"
#define	SEDOC		"doc/config.sed"
#define	MAKE		"../%s/Makefile"

#define	WARNING	"This file is automatically generated.  Do not edit!"
#define	MHRELEASE	"6.8"		/* for version: "Maj.min.pat" */
#define	MHCENTERFOOT	"MH.6.8"	/* for nroff page footers */
#define	MHLEFTFOOT	"[mh.6]"	/* [mh.6]    MH.6.6   page# */
#define	MHSLIBVER	"3.2"		/* SunOS4 shared library version */

#define	NOTOK	(-1)

#define	NULLCP	((char *) 0)

#define	SVAL(s)	(s ? s : "")

#define	QUOTE	'\\'

/*  */

static int   smtp = 0;
static int   mpop = 0;

static int   sedP = 1;
static int   filesP = 1;
static int   docP = 1;
static int   makeP = 1;

static char *myname = "mhconfig";
static char *config = NULL;
static char *myopts = NULL;

static char *binpath = "/usr/local";
static char *bbhome = "/usr/spool/bboards";
static char *bboards = "off";
static char *bbdelivery = "off";
static char *ccoptions = NULL;
static char *chownpath = "/etc/chown";
static char *compiler = "/bin/cc";
static char *copy = "cp";
static char *curses = "-lcurses -ltermlib";
static char *debug = "off";
static char *editor = "prompter";
static char *etcpath = "/usr/local/lib/mh";
static char *ldoptions = NULL;
static char *ldoptlibs = NULL;
static char *lex = "lex -nt";
static char *ln = "ln";
static char *mailgroup = "off";
static char *mailpath = "/usr/spool/mail";
static char *maildir = NULL;
static char *mailfile = NULL;
static char *mandir = "/usr/man";
static char *manuals = "standard";
static char *mf = "off";
static char *mts = "sendmail";
static char *poppath = "/usr/etc";
static char *removeit = "mv -f";
static char *regtest = "off";
static char *oldload = "off";
static char *options = NULL;
static char *pop = "off";
static char *ranlib = "on";
static char *sharedlib = "off";
static char *signl = "int";
static char *slibdir = "/usr/local/lib";
static char *slflags = "-pic";
static char *sprntf = NULL;

/*  */

#define	unknown		0
#define	mmdf		1
#define	mmdf2		2
#define	mh		3
#define	sendmail	4
static int mtsnum = unknown;

static struct mtsopt {
    char  *mtskey;
    char  *optval;
    int	   code;
    int    mtsflags;
#define	NOFLAGS	0x00
#define	DOSMTP	0x01
#define	NOMF	0x02
} mtsopts[] = {
    "mmdf", "MMDFMTS MMDFI", mmdf, NOFLAGS,
    "mmdf/smtp", "SENDMTS SMTP", mmdf, DOSMTP,
    "mmdf2", "MMDFMTS MMDFII", mmdf2, NOFLAGS,
    "mmdf2/smtp", "SENDMTS MMDFII SMTP", mmdf2, DOSMTP,
    "mh", "MHMTS", mh, NOFLAGS,
    "sendmail", "SENDMTS", sendmail, NOMF,
    "sendmail/smtp", "SENDMTS SMTP", sendmail, NOMF,
    "zmailer", "ZMAILER SENDMTS", sendmail, NOMF,
    "zmailer/smtp", "ZMAILER SENDMTS SMTP", sendmail, NOMF,
    NULL, NULL, unknown, NOFLAGS
};

/* other options which cause sed BEGIN/END configuration changes */

static struct copt {
    char *c_name;
    int   c_value;
}	copts[] = {
    "APOP", 0,	/* authenticated pop */
    "BSD43", 0,	/* sgid ttys */
    "KPOP", 0, 	/* KERBEROS pop */
    "HESIOD", 0,
    "MIME", 0,	/* multi-media extensions */
    "MPOP", 0,	/* mobile pop */
    "MSGID", 0, /* slocal dup surpression */
    "TMA", 0,	/* trusted mail */
    "TTYD", 0,
    "UCI", 0,
    NULL, 0
};



/*  */

static struct bind {
    char   *keyword;

    char  **value;

    int     flags;
#define	BD_NIL	0x00
#define	BD_ADD	0x01
}                   binds[] = {
                        "bin", &binpath, BD_NIL,
                        "bbhome", &bbhome, BD_NIL,
                        "bboards", &bboards, BD_NIL,
                        "bbdelivery", &bbdelivery, BD_NIL,
			"ccoptions", &ccoptions, BD_ADD,
			"chown", &chownpath, BD_NIL,
			"cc", &compiler, BD_NIL,
			"cp", &copy, BD_NIL,
			"curses", &curses, BD_NIL,
			"debug", &debug, BD_NIL,
			"editor", &editor, BD_NIL,
                        "etc", &etcpath, BD_NIL,
			"ldoptions", &ldoptions, BD_ADD,
			"ldoptlibs", &ldoptlibs, BD_ADD,
			"lex", &lex, BD_NIL,
			"ln", &ln, BD_NIL,
                        "mail", &mailpath, BD_NIL,
			"mailgroup", &mailgroup, BD_NIL,
                        "mandir", &mandir, BD_NIL,
                        "manuals", &manuals, BD_NIL,
			"mf", &mf, BD_NIL,
                        "mts", &mts, BD_NIL,
			"oldload", &oldload, BD_NIL,
                        "options", &options, BD_ADD,
                        "pop", &pop, BD_NIL,
			"popdir", &poppath, BD_NIL,
			"ranlib", &ranlib, BD_NIL,
			"remove", &removeit, BD_NIL,
			"regtest", &regtest, BD_NIL,
			"sharedlib", &sharedlib, BD_NIL,
			"signal", &signl, BD_NIL,
			"slflags", &slflags, BD_NIL,
			"slibdir", &slibdir, BD_NIL,
			"sprintf", &sprntf, BD_NIL,
                        NULL
};

/*  */

char   *files[] = {
    "../support/bboards/bboards.daily",
    "../support/bboards/bboards.weekly",
    "../support/bboards/crontab",
    "../support/bboards/MakeBBoards",
    "../config/config.c",
    "../zotnet/mts/mts.c",
    "../zotnet/mts/mtstailor",
    "../support/general/MailAliases",
    "../mts/sendmail/bboardsMH.m4",
    "../mts/sendmail/popMH.m4",
    NULL
};

static struct pair {
    char   *infile;
    char   *outfile;
}                  makefiles[] = {
			"makefiles/MH-Makefile", ".",
			"makefiles/config","config",
			"makefiles/dist","dist",
			"makefiles/doc","doc",
			"makefiles/mtsM","mts",
			    "makefiles/mts/mh","mts/mh",
			    "makefiles/mts/mmdf","mts/mmdf",
			    "makefiles/mts/sendmail","mts/sendmail",
			"makefiles/papers","papers",
			"makefiles/sbr","sbr",
			"makefiles/supportM","support",
			    "makefiles/support/bboards","support/bboards",
			    "makefiles/support/general","support/general",
			    "makefiles/support/pop","support/pop",
			"makefiles/uip","uip",
			"makefiles/zotnetM","zotnet",
			    "makefiles/zotnet/bboards","zotnet/bboards",
			    "makefiles/zotnet/mf","zotnet/mf",
			    "makefiles/zotnet/mts","zotnet/mts",
			    "makefiles/zotnet/tws","zotnet/tws",
			NULL
};


static do_sed(), do_sedfile(), do_files(), do_doc(), do_make();
static shell(), arginit(), add_option(), trim();

static char   *stradd (), *strdup (), *tail ();

long    time ();

/*  */

/* ARGSUSED */

main (argc, argv, envp)
int     argc;
char  **argv,
      **envp;
{
    arginit (argv);

    if (sedP)
	do_sed ();
    if (filesP)
	do_files ();
    if (docP)
	do_doc ();
    if (makeP)
	do_make ();

#ifdef	notdef
    printf ("Don't forget to edit \"config/mtstailor\"\n");
#endif
    if (filesP || makeP)
	printf ("Don't forget to run \"make clean\" in the top directory before compiling\n");

    exit (0);
}

/*  */

static  do_sed () {
    if (docP != 2)
	do_sedfile (SED, 0);
    do_sedfile (SEDOC, smtp);
}

static do_alldoc (fp)
FILE *fp;
{
    fprintf (fp, "%s\n%s\n%s\n%s\n", 	/* turn @BEGIN: XX into #ifdef XX */
	"s!^@BEGIN: \\(.*\\)$!.br\\",	/* for MH wizard manual set */
	".ti 0i\\",
	"#ifdef \\1\\",
	".br!"
    );
    fprintf (fp, "%s\n%s\n%s\n%s\n", 
	"s!^@END: \\(.*\\)$!.br\\",
	".ti 0i\\",
	"#endif /* \\1\\ */",
	".br!"
    );
}

/*  */

static do_sedfile (sed, flag)
char	*sed;
int	flag;
{
    int     i;
    FILE * fp;
    struct copt *op;

    if ((fp = fopen (sed, "w")) == NULL)
	adios (sed, "unable to write");
    printf ("generating %s\n", sed);

    if (docP == 2) {
	printf ("...using special all-doc format.\n");
	printf ("Be sure to reconfigure before compiling.\n");
	do_alldoc(fp);	/* hack up doc/config.sed */
    }

    fprintf (fp, "s%%@(MHCONFIGFILE)%%%s%%g\n", SVAL(config));
    fprintf (fp, "s%%@(MHWARNING)%%%s%%g\n", WARNING);
    fprintf (fp, "s%%@(MHRELEASE)%%%s%%g\n", MHRELEASE);
    fprintf (fp, "s%%@(MHCENTERFOOT)%%%s%%g\n", MHCENTERFOOT);
    fprintf (fp, "s%%@(MHLEFTFOOT)%%%s%%g\n", MHLEFTFOOT);
    fprintf (fp, "s%%@(MHBINPATH)%%%s%%g\n", SVAL (binpath));
    fprintf (fp, "s%%@(MHETCPATH)%%%s%%g\n", SVAL (etcpath));
    fprintf (fp, "s%%@(MHPOPDPATH)%%%s%%g\n", SVAL (poppath));
    fprintf (fp, "s%%@(MHCHOWNPATH)%%%s%%g\n", SVAL (chownpath));
    fprintf (fp, "s%%@(MHDROPATH)%%%s%%g\n", SVAL (maildir));
    fprintf (fp, "s%%@(MHDROPFIL)%%%s%%g\n", SVAL (mailfile));
    if (maildir)
	fprintf (fp, "s%%@(MHDROPLOC)%%%s/$USER%%g\n", SVAL (maildir));
    else
	fprintf (fp, "s%%@(MHDROPLOC)%%$HOME/%s%%g\n", SVAL (mailfile));
    fprintf (fp, "s%%@(MHDROPHAK)%%%s%%g\n",/* HACK */
	maildir ? maildir : "/usr/spool/mail");

    fprintf (fp, "s%%@(MHMANDIR)%%%s%%g\n", mandir);
    if (strcmp (manuals, "standard") == 0 || strcmp (manuals, "gen") == 0)
	for (i = 1; i <= 8; i++) {
	    fprintf (fp, "s%%@(MHMANDIR%d)%%man%d%%g\n", i, i);
	    fprintf (fp, "s%%@(MHMANEXT%d)%%%d%%g\n", i, i);
	}
    else
	for (i = 1; i <= 8; i++) {
	    fprintf (fp, "s%%@(MHMANDIR%d)%%man%c%%g\n", i,
		    strcmp (manuals, "new") == 0 ? 'n'
		    : strcmp (manuals, "old") == 0 ? 'o'
		    : 'l');
	    fprintf (fp, "s%%@(MHMANEXT%d)%%%c%%g\n", i,
		    strcmp (manuals, "new") == 0 ? 'n'
		    : strcmp (manuals, "old") == 0 ? 'o'
		    : 'l');
	}

/*  */

    if (strcmp (regtest, "on") == 0)
	fprintf (fp, "s%%@(MHHOSTVER)%%%s%%g\n", "-n");
    else
	fprintf (fp, "s%%@(MHHOSTVER)%%%s%%g\n", "");

    if (strcmp (manuals, "gen") == 0 || strcmp (manuals, "gen") == 0)
	fprintf (fp, "s%%@(MHMANGEN)%%%s%%g\n", "#"); /* comment char */
    else
	fprintf (fp, "s%%@(MHMANGEN)%%%s%%g\n", "");

    fprintf (fp, "s%%@(MHEDITOR)%%%s%%g\n", SVAL (editor));
    fprintf (fp, "s%%@(MHCONFIG)%%%s%%g\n", SVAL (myopts));
    fprintf (fp, "s%%@(MHCOMPILER)%%%s%%g\n", SVAL (compiler));
    fprintf (fp, "s%%@(CP)%%%s%%g\n", SVAL (copy));
    fprintf (fp, "s%%@(LN)%%%s%%g\n", SVAL (ln));
    fprintf (fp, "s%%@(LEX)%%%s%%g\n", SVAL (lex));
    if (ccoptions)
	fprintf (fp, "s%%@(MHOPTIONS)%%%s %s%%g\n",
		SVAL (options), SVAL (ccoptions));
    else
	fprintf (fp, "s%%@(MHOPTIONS)%%%s %s%%g\n",
		SVAL (options), "-O");
    fprintf (fp, "s%%@(LDOPTIONS)%%%s%%g\n", SVAL (ldoptions));
    fprintf (fp, "s%%@(LDOPTLIB)%%%s%%g\n", SVAL (ldoptlibs));
    fprintf (fp, "s%%@(LDCURSES)%%%s%%g\n", SVAL (curses));

    if (strcmp (bboards, "off") == 0)
	fprintf (fp, "/^@BEGIN: BBOARDS$/,/^@END: BBOARDS$/d\n");
    else
	fprintf (fp, "/^@BEGIN: BBOARDS$/d\n/^@END: BBOARDS$/d\n");
    fprintf (fp, "s%%@(BBHOME)%%%s%%g\n", SVAL (bbhome));
    if (strcmp (bbdelivery, "off") == 0)
	fprintf (fp, "/^@BEGIN: BBSERVER$/,/^@END: BBSERVER$/d\n");
    else
	fprintf (fp, "/^@BEGIN: BBSERVER$/d\n/^@END: BBSERVER$/d\n");

    if (strcmp (debug, "on") == 0) {
	fprintf (fp, "/^@BEGIN: DEBUG$/d\n/^@END: DEBUG$/d\n");
	fprintf (fp, "/^@BEGIN: OPTIM$/,/^@END: OPTIM$/d\n");
    }
    else {
	fprintf (fp, "/^@BEGIN: DEBUG$/,/^@END: DEBUG$/d\n");
	fprintf (fp, "/^@BEGIN: OPTIM$/d\n/^@END: OPTIM$/d\n");
    }

    if (strcmp (mailgroup, "off")) { /* for SYS5 sgid-inc */
	fprintf (fp, "/^@BEGIN: MAILGROUP$/d\n/^@END: MAILGROUP$/d\n");
	fprintf (fp, "s%%@(MAILGROUP)%%%s%%g\n", SVAL (mailgroup));
    }
    else
	fprintf (fp, "/^@BEGIN: MAILGROUP$/,/^@END: MAILGROUP$/d\n");

    if (strcmp (oldload, "on") == 0)
	fprintf (fp, "/^@BEGIN: OLDLOAD$/d\n/^@END: OLDLOAD$/d\n");
    else
	fprintf (fp, "/^@BEGIN: OLDLOAD$/,/^@END: OLDLOAD$/d\n");
    if (strcmp (oldload, "off") == 0)
	fprintf (fp, "/^@BEGIN: NEWLOAD$/d\n/^@END: NEWLOAD$/d\n");
    else
	fprintf (fp, "/^@BEGIN: NEWLOAD$/,/^@END: NEWLOAD$/d\n");

/*  */

    if (strcmp (ranlib, "on") == 0) {
	fprintf (fp, "/^@BEGIN: RANLIB$/d\n/^@END: RANLIB$/d\n");
	fprintf (fp, "/^@BEGIN: LORDER$/,/^@END: LORDER$/d\n");
    }
    else {
	fprintf (fp, "/^@BEGIN: LORDER$/d\n/^@END: LORDER$/d\n");
	fprintf (fp, "/^@BEGIN: RANLIB$/,/^@END: RANLIB$/d\n");
    }

    if (strcmp (sharedlib, "off")) {	/* if not off */
	fprintf (fp, "/^@BEGIN: SHAREDLIB$/d\n/^@END: SHAREDLIB$/d\n");
	fprintf (fp, "/^@BEGIN: STDLIB$/,/^@END: STDLIB$/d\n");
	fprintf (fp, "s%%@(SLIBDIR)%%%s%%g\n", slibdir);
	fprintf (fp, "s%%@(SLIBVER)%%%s%%g\n", MHSLIBVER);
	fprintf (fp, "s%%@(SLFLAGS)%%%s%%g\n", SVAL(slflags));
	if (strcmp (sharedlib, "secure") == 0)
	    fprintf (fp, "s%%@(SLDFLAG)%%-L%s%%g\n",slibdir);
	else
	    fprintf (fp, "s%%@(SLDFLAG)%%%%g\n");
    }
    else {
	fprintf (fp, "/^@BEGIN: STDLIB$/d\n/^@END: STDLIB$/d\n");
	fprintf (fp, "/^@BEGIN: SHAREDLIB$/,/^@END: SHAREDLIB$/d\n");
    }
    if (strcmp (sharedlib, "sun4") == 0 || strcmp (sharedlib, "on") == 0)
	fprintf (fp, "/^@BEGIN: SUN4SHLIB$/d\n/^@END: SUN4SHLIB$/d\n");
    else
	fprintf (fp, "/^@BEGIN: SUN4SHLIB$/,/^@END: SUN4SHLIB$/d\n");
    if (strcmp (sharedlib, "sys5") == 0)
	fprintf (fp, "/^@BEGIN: SYS5SHLIB$/d\n/^@END: SYS5SHLIB$/d\n");
    else
	fprintf (fp, "/^@BEGIN: SYS5SHLIB$/,/^@END: SYS5SHLIB$/d\n");

/*  */

    if (flag || ((mtsnum == mmdf || mtsnum == mmdf2) && !smtp)) {
	fprintf (fp, "/^@BEGIN: MMDFMTS$/d\n/^@END: MMDFMTS$/d\n");
	if (mtsnum == mmdf)
	    fprintf (fp, "/^@BEGIN: MMDFIMTS$/d\n/^@END: MMDFIMTS$/d\n");
	else
	    fprintf (fp, "/^@BEGIN: MMDFIMTS$/,/^@END: MMDFIMTS$/d\n");
	if (mtsnum == mmdf2)
	    fprintf (fp, "/^@BEGIN: MMDFIIMTS$/d\n/^@END: MMDFIIMTS$/d\n");
	else
	    fprintf (fp, "/^@BEGIN: MMDFIIMTS$/,/^@END: MMDFIIMTS$/d\n");
    }
    else {
	fprintf (fp, "/^@BEGIN: MMDFMTS$/,/^@END: MMDFMTS$/d\n");
	fprintf (fp, "/^@BEGIN: MMDFIMTS$/,/^@END: MMDFIMTS$/d\n");
	fprintf (fp, "/^@BEGIN: MMDFIIMTS$/,/^@END: MMDFIIMTS$/d\n");
    }

    if (mtsnum == mh)
	fprintf (fp, "/^@BEGIN: MHMTS$/d\n/^@END: MHMTS$/d\n");
    else
	fprintf (fp, "/^@BEGIN: MHMTS$/,/^@END: MHMTS$/d\n");

    if (!flag && (mtsnum == sendmail || smtp))
	fprintf (fp, "/^@BEGIN: SENDMTS$/d\n/^@END: SENDMTS$/d\n");
    else
	fprintf (fp, "/^@BEGIN: SENDMTS$/,/^@END: SENDMTS$/d\n");

    switch (mtsnum) {
	case mh:
	case sendmail:
	case mmdf:
	    fprintf (fp, "/^@BEGIN: SENDMTSHACK$/d\n/^@END: SENDMTSHACK$/d\n");
	    break;

	default:
	    fprintf (fp, "/^@BEGIN: SENDMTSHACK$/,/^@END: SENDMTSHACK$/d\n");
	    break;
    }

/*  */

    /* special hack for support/pop/popser.c */
    fprintf (fp, "s%%@(POPUUMBOX)%%%s%%g\n",
	    mtsnum == sendmail ? "-DPOPUUMBOX" : "");

    if (smtp)
	fprintf (fp, "/^@BEGIN: SMTP$/d\n/^@END: SMTP$/d\n");
    else
	fprintf (fp, "/^@BEGIN: SMTP$/,/^@END: SMTP$/d\n");

    if (strcmp (pop, "on") == 0)
	fprintf (fp, "/^@BEGIN: POP$/d\n/^@END: POP$/d\n");
    else
	fprintf (fp, "/^@BEGIN: POP$/,/^@END: POP$/d\n");

    if (strcmp (bboards, "pop") == 0)
	fprintf (fp, "/^@BEGIN: BPOP$/d\n/^@END: BPOP$/d\n");
    else
	fprintf (fp, "/^@BEGIN: BPOP$/,/^@END: BPOP$/d\n");

    if (strcmp (bboards, "nntp") == 0)
	fprintf (fp, "/^@BEGIN: NNTP$/d\n/^@END: NNTP$/d\n");
    else
	fprintf (fp, "/^@BEGIN: NNTP$/,/^@END: NNTP$/d\n");

    if (strcmp (mf, "on") == 0)
	fprintf (fp, "/^@BEGIN: MF$/d\n/^@END: MF$/d\n");
    else
	fprintf (fp, "/^@BEGIN: MF$/,/^@END: MF$/d\n");

    fprintf (fp, "s%%@(MHREMOVE)%%%s%%g\n", SVAL (removeit));

    for (op = copts; op->c_name; op++) {
	fprintf (fp, op->c_value ? "/^@BEGIN: %s$/d\n/^@END: %s$/d\n" :
				 "/^@BEGIN: %s$/,/^@END: %s$/d\n",
		     op->c_name, op->c_name);
    }

    (void) fclose (fp);
}

/*  */

static  do_files () {
    char  **pp;

    for (pp = files; *pp; pp++)
	shell ("rm -f %s; sed -f %s < config/%s > %s",
		*pp, SED, tail (*pp), *pp);
}


static  do_doc () {
    shell ("./mhdoc");
}


static  do_make () {
    char    buffer[BUFSIZ];
    struct pair *pp;

    for (pp = makefiles; pp -> infile; pp++) {
	(void) sprintf (buffer, MAKE, pp -> outfile);
	shell ("rm -f %s; sed -f %s < %s > %s",
		buffer, SED, pp -> infile, buffer);
    }
}

/*  */

/* VARARGS */

static  shell (fmt, a, b, c, d)
char   *fmt,
       *a,
       *b,
       *c,
       *d;
{
    char    buffer[BUFSIZ];

    (void) sprintf (buffer, fmt, a, b, c, d);
    printf ("%s\n", buffer);
    (void) fflush (stdout);

    if (system (buffer))
	adios (NULLCP, "failed");
}

/*  */

static  arginit (vec)
char  **vec;
{
    int	    i;
    char   *ap,
           *cp,
           *dp,
            buffer[BUFSIZ];
    struct bind *bp;
    FILE * fp;

    myname = tail (*vec);
    printf (BANNER, myname);
    (void) fflush (stdout);
    
    for (vec++; *vec; vec++) {
	if (strcmp (*vec, "-D") == 0) {
	    sedP = 1; filesP = makeP = 0; docP = 2;
	    continue;
	}
	if (strcmp (*vec, "-s") == 0) {
	    sedP = !sedP;
	    continue;
	}
	if (strcmp (*vec, "-f") == 0) {
	    filesP = !filesP;
	    continue;
	}
	if (strcmp (*vec, "-d") == 0) {
	    docP = !docP;
	    continue;
	}
	if (strcmp (*vec, "-m") == 0) {
	    makeP = !makeP;
	    continue;
	}
	if (strcmp (*vec, "-help") == 0) {
	    printf ("usage: %s [opt] file\n", myname);
	    printf ("%s\n%s\n%s\n%s\n",
		" -s\tno sed files", " -f\tno config files",
		" -d\tno doc files", " -m\tno makefiles");
	    exit (0);
	}
	break;
    }
    if ((config = *vec) == NULL || vec[1] != NULL) /* no arg or too many */
	adios (NULLCP, "usage: %s file", myname);

/*  */

    if ((fp = fopen (config, "r")) == NULL)
	adios (config, "unable to read");

    while (fgets (buffer, sizeof buffer, fp)) {
	if (ap = index (buffer, '\n'))
	    *ap-- = '\0';
	else
	    ap = &buffer[strlen (buffer) - 1];
	while (ap >= buffer)
	    if (isspace (*ap))
		*ap = '\0';
	    else
		break;
	for (cp = buffer; isspace (*cp); cp++)
	    continue;
	if (*cp == '\0' || *cp == '#')
	    continue;
	for (ap = cp; *ap; ap++)
	    if (isspace (*ap) || *ap == ':') {
		*ap++ = '\0';
		while (isspace (*ap))
		    ap++;
		break;
	    }
	for (bp = binds; bp -> keyword; bp++)
	    if (strcmp (bp -> keyword, cp) == 0)
		break;
	if (bp -> keyword == NULL)
	    adios (NULLCP, "unknown option %s in %s", cp, config);
	else
	    if (*(bp -> value) == NULL || !(bp -> flags & BD_ADD))
		*(bp -> value) = strdup (ap);
	    else
		*(bp -> value) = stradd (ap, stradd (" ", *(bp -> value)));
    }

    (void) fclose (fp);

/*  */

    if (binpath == NULL)
	adios (NULLCP, "bin must be specified in %s", config);
    trim (binpath);

    if (strcmp (bboards, "on") && strcmp (bboards, "off") &&
        strcmp (bboards, "pop") && strcmp (bboards, "nntp"))
	adios (NULLCP, "bboards should be %s not %s",
		"either \"on\", \"off\", \"pop\" or \"nntp\"", bboards);
    if (strcmp (bboards, "off") == 0 && strcmp (bbdelivery, "off"))
	adios (NULLCP, 
	    "you cannot have \"bbdelivery\" \"%s\" with \"bboards\" \"%s\"",
	    bbdelivery, bboards);
    if (bbhome == NULL)
	adios (NULLCP, "bbhome must be specified in %s", config);
    trim (bbhome);

    if (strcmp (debug, "on") && strcmp (debug, "off"))
	adios (NULLCP, "debug should be either \"on\" or \"off\", not %s",
		debug);

    if (etcpath == NULL)
	adios (NULLCP, "etc must be specified in %s", config);
    trim (etcpath);

    if (strcmp (mailgroup, "off"))	/* if not "off" */
	add_option ("MAILGROUP");
    if (mailpath == NULL)
	adios (NULLCP, "mail must be specified in %s", config);
    trim (mailpath);
    if (*mailpath == '/')
	maildir = mailpath;
    else
	mailfile = mailpath;

    if (strcmp (manuals, "standard")
	    && strcmp (manuals, "local")
	    && strcmp (manuals, "new")
	    && strcmp (manuals, "old")
	    && strcmp (manuals, "gen")
	    && strcmp (manuals, "none"))
	adios (NULLCP,"invalid manuals setting: %s",manuals);

    if (mts == NULL)
	adios (NULLCP, "mts must be specified in %s", config);
    for (i = 0; mtsopts[i].mtskey; i++)
	if (strcmp (mts, mtsopts[i].mtskey) == 0)
	    break;
    if ((mtsnum = mtsopts[i].code) == unknown)
	adios (NULLCP, "invalid mts setting: %s", mts);
    add_option (mtsopts[i].optval);
    smtp = mtsopts[i].mtsflags & DOSMTP;
    if (mtsopts[i].mtsflags & NOMF)
	mf = "off";		/* hack... */

    if (strcmp (pop, "on") && strcmp (pop, "off"))
	adios (NULLCP, "pop should be either \"on\" or \"off\", not %s", pop);
    if (strcmp (pop, "on") == 0)
	add_option ("POP");

    if (strcmp (bboards, "pop") == 0) {
	add_option ("BPOP");
	if (strcmp (pop, "on"))
	    adios (NULLCP, "bboards \"pop\" requires pop \"on\"");
    }
    else if (strcmp (bboards, "nntp") == 0) {
	add_option ("BPOP");/* used by msh.c & bbc.c, but see also pshsbr.c */
	add_option ("NNTP");
	if (mpop)
	    adios (NULLCP,
		"bboards \"nntp\" is incompatible with \"options MPOP\"");
    }

    if (strcmp (mf, "on") && strcmp (mf, "off"))
	adios (NULLCP, "mf should be either \"on\" or \"off\", not %s", mf);
    if (strcmp (mf, "on") == 0)
	add_option ("MF");

    if (sprntf && strcmp (sprntf, "char *")) {/* this defaults to "char *" */
	if (strcmp (sprntf, "int"))
	    adios (NULLCP,
		    "sprintf should be either \"int\" or \"char *\", not %s",
		    sprntf);
	add_option ("SPRINTFTYPE=int");
    }

    if (strcmp (sharedlib, "on") && strcmp (sharedlib, "off")
	    && strcmp (sharedlib, "sun4") && strcmp (sharedlib, "sys5")
	    && strcmp (sharedlib, "secure"))
	adios (NULLCP,
	    "sharedlib should be either \"sun4\", \"sys5\", or \"off\", not %s",
	    sharedlib);

    (void) sprintf(buffer, "TYPESIG=%s", signl);
    add_option (buffer);

/* loop over options string and add -D ccoptions */

    begin_myopt ();
    if (cp = options) {
	for (ap = cp; *ap; ap++)
	    if (isspace (*ap))
		*ap = ' ';
	options = NULL;
	for (ap = cp; dp = index (ap, ' '); ap = dp) {
	    *dp++ = '\0';
	    add_myopt (ap);
	    (void) sprintf (buffer, "%s-D%s", options ? " " : "", ap);
	    ap = buffer;
	    options = stradd (ap, options);
	    while (isspace (*dp))
		dp++;
	}
	if (*ap) {
	    add_myopt (ap);
	    (void) sprintf (buffer, "%s-D%s", options ? " " : "", ap);
	    ap = buffer;
	    options = stradd (ap, options);
	}
	free (cp);
    }
    end_myopt ();
    if (strcmp (bboards, "nntp") == 0 && mpop)
	adios (NULLCP,
		"bboards \"nntp\" is incompatible with \"options MPOP\"");
}

/*  */

static  add_option (s)
char   *s;
{
    options = options ? stradd (s, stradd (" ", options)) : strdup (s);
}


begin_myopt () {
    myopts = strdup ("char *options[] = {");
}


add_myopt (s)
char   *s;
{
    int     len;
    char   *bp,
            buffer[BUFSIZ];
    struct copt *op;
    static int  nameoutput = 0;
    static int  linepos = 0;
    static int  outputlinelen = 72;

    for (op = copts; op->c_name; op++)
	if (!strncmp (op->c_name,s,strlen(op->c_name))) {
	    op->c_value = 1;
	    break;
	}

    if (strcmp (s, "MPOP") == 0)
	mpop = 1;
    if (smtp && strcmp (s, "BERK") == 0) {
	fprintf (stderr, "\"options BERK\" overriding smtp suffix...\n");
	smtp = 0;
    }
    if (!nameoutput) {
	printf ("%s: ", bp = "options");
	linepos += (nameoutput = strlen (bp) + 2);
    }
    len = strlen (s);
    if (linepos != nameoutput)
	if (len + linepos + 3 > outputlinelen)
	    printf ("\n%*s", linepos = nameoutput, "");
	else
	    printf (" "), linepos++;
    printf ("[%s]", s);
    linepos += len + 2;
    (void) fflush (stdout);

    bp = buffer;
    *bp++ = '"';
    while (*s) {
	if (*s == '"')
	    *bp++ = QUOTE, *bp++ = QUOTE;
	*bp++ = *s++;
    }
    (void) strcpy (bp, "\", ");
    insopt (buffer);
}


end_myopt () {
    printf ("\n");
    (void) fflush (stdout);
    mkopts();
    myopts = stradd ("NULL};", myopts);
}

/*  */

static  trim (s)
char   *s;
{
    char   *p;

    if (s == NULL || (p = rindex (s, '/')) == NULL)
	return;
    if (*++p == '\0')
	*--p = '\0';
}


static char *tail (s)
char   *s;
{
    char   *p;

    if (p = rindex (s, '/'))
	p++;
    return ((p == NULL || *p == '\0') ? s : p);
}

/*  */

static char *stradd (s1, s2)
char   *s1,
       *s2;
{
    char   *p;

    if (s1 == NULL || *s1 == '\0')
	return s2;
    if (s2 == NULL)
	return strdup (s1);
    if ((p = malloc ((unsigned) (strlen (s1) + strlen (s2) + 2))) == NULL)
	adios (NULLCP, "insufficient memory");

    (void) sprintf (p, "%s%s", s2, s1);
    if (s2)
	free (s2);
    return p;
}


static char *strdup (s)
char   *s;
{
    char   *p;

    if ((p = malloc ((unsigned) (strlen (s) + 1))) == NULL)
	adios (NULLCP, "insufficient memory");

    (void) strcpy (p, s);
    return p;
}

/*  */

#ifdef	notdef
/* these used to be for SYS5 machines, but now this is compiled
 * with -DSYS5 which redefines the SYS5 str[r]chr names to [r]index.
 */
static char *index (s, c)
char   *s,
        c;
{
    char    i;

    while (i = *s++)
	if (i == c)
	    return (s - 1);

    return NULL;
}


static char *rindex (s, c)
char   *s,
        c;
{
    char    i,
           *t;

    t = NULL;
    while (i = *s++)
	if (i == c)
	    t = s - 1;

    return t;
}
#endif /* notdef */

/*  */

/* VARARGS */

adios (what, fmt, a, b, c)
char   *what,
       *fmt,
       *a,
       *b,
       *c;
{
    (void) fflush (stdout);

    fprintf (stderr, "%s: ", myname);
    fprintf (stderr, fmt, a, b, c);
    if (what) {
	(void) fputc (' ', stderr);
	perror (what);
    }
    else
	(void) fputc ('\n', stderr);

    exit (1);
}

/* quick and dirty ordering of options for readability */

struct opt {
    char *s;
    struct opt *n;
};

static struct opt *optp = (struct opt *)NULL;

static void
insopt(s)
char *s;	/* string for sorting */
{
    struct opt  *p,
		*np,
		*lp = (struct opt *)NULL;

    if ((np = (struct opt *) malloc ((unsigned) sizeof (struct opt))) == 
	    (struct opt *)NULL)
	adios (NULL, "unable to malloc opt");
    np -> s = strdup(s);
    np -> n = (struct opt *)NULL;

    for (p = optp; p; lp = p, p = p -> n) {
	if (strcmp (p->s, s) > 0)
	    break;
    }
    if (p)
	np -> n = p;
    if (lp)
	lp -> n = np;
    else
	optp = np;
}

static void
mkopts()
{
    struct opt *p;

    for (p = optp; p; p = p -> n)
	myopts = stradd (p->s, myopts);
}

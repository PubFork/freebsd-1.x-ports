/* $Id: common.h,v 1.1 1993/07/19 20:07:00 nate Exp $
 */
/* This software is Copyright 1991 by Stan Barber. 
 *
 * Permission is hereby granted to copy, reproduce, redistribute or otherwise
 * use this software as long as: there is no monetary profit gained
 * specifically from the use or reproduction of this software, it is not
 * sold, rented, traded or otherwise marketed, and this copyright notice is
 * included prominently in any copy made. 
 *
 * The author make no claims as to the fitness or correctness of this software
 * for any use whatsoever, and it is provided as is. Any use of this software
 * is at the user's own risk. 
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include "config.h"	/* generated by installation script */
#ifndef isalnum
#   define isalnum(c) (isalpha(c) || isdigit(c))
#endif

#include <errno.h>
#include <signal.h>
#ifdef I_SYS_IOCTL
#include <sys/ioctl.h>
#endif
#ifdef I_VFORK
#  include <vfork.h>
#endif
#include <fcntl.h>

#ifdef I_TERMIO
#   include <termio.h>
#else
# ifdef I_TERMIOS
#   include <termios.h>
# else
#   include <sgtty.h>
# endif
#endif

#ifdef HAS_GETPWENT
#   include <pwd.h>
#endif

#ifdef I_PTEM
#include <sys/stream.h>
#include <sys/ptem.h>
#endif

#include <time.h>

#define BITSPERBYTE 8
#define LBUFLEN 1024	/* line buffer length */
			/* (don't worry, .newsrc lines can exceed this) */
#define CBUFLEN 512	/* command buffer length */
#define PUSHSIZE 256
#define MAXFILENAME 512
#define LONGKEY 15	/* longest keyword: currently "posting-version" */
#define FINISHCMD 0177

/* some handy defs */

#define bool char
#define bool_int int
#define char_int int
#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif
#define Null(t) ((t)0)
#define Nullch Null(char*)
#define Nullfp Null(FILE*)

#define Ctl(ch) (ch & 037)

#define strNE(s1,s2) (strcmp(s1,s2))
#define strEQ(s1,s2) (!strcmp(s1,s2))
#define strnNE(s1,s2,l) (strncmp(s1,s2,l))
#define strnEQ(s1,s2,l) (!strncmp(s1,s2,l))

/* Things we can figure out ourselves */

#ifdef SIGTSTP
#   define BERKELEY 	/* include job control signals? */
#endif

#if defined(FIONREAD) || defined(HAS_RDCHK) || defined(O_NDELAY)
#   define PENDING
#endif

#ifdef EUNICE
#   define LINKART		/* add 1 level of possible indirection */
#   define UNLINK(victim) while (!unlink(victim))
#else
#   define UNLINK(victim) unlink(victim)
#endif

/* Valid substitutions for strings marked with % comment are:
 *	%a	Current article number
 *	%A	Full name of current article (%P/%c/%a)
 *		(if LINKART defined, is the name of the real article)
 *	%b	Destination of a save command, a mailbox or command
 *	%B	The byte offset to the beginning of the article for saves
 *		with or without the header
 *	%c	Current newsgroup, directory form
 *	%C	Current newsgroup, dot form
 *	%d	%P/%c
 *	%D	Old Distribution: line
 *	%e	Extract program
 *	%E	Extract destination directory
 *	%f	Old From: line or Reply-To: line
 *	%F	Newsgroups to followup to from Newsgroups: and Followup-To:
 *	%h	Name of header file to pass to mail or news poster
 *	%H	Host name (yours)
 *	%i	Old Message-I.D.: line, with <>
 *	%I	Inclusion indicator
 *	%l	News administrator login name
 *	%L	Login name (yours)
 *	%m	The current mode of trn.
 *	%M	Number of articles marked with M
 *	%n	Newsgroups from source article
 *	%N	Full name (yours)
 *	%o	Organization (yours)
 *	%O	Original working directory (where you ran trn from)
 *	%p	Your private news directory (-d switch)
 *	%P	Public news spool directory (NEWSSPOOL)
 *	%r	Last reference (parent article id)
 *	%R	New references list
 *	%s	Subject, with all Re's and (nf)'s stripped off
 *	%S	Subject, with one Re stripped off
 *	%t	New To: line derived from From: and Reply-To (Internet always)
 *	%T	New To: line derived from Path:
 *	%u	Number of unread articles
 *	%U	Number of unread articles disregarding current article
 *	%v	Number of unselected articles disregarding current article
 *	%W	The thread directory root
 *	%x	News library directory, usually /usr/lib/news
 *	%X	Rn library directory, usually %x/rn
 *	%z	Size of current article in bytes.
 *	%Z	Number of selected threads.
 *	%~	Home directory
 *	%.	Directory containing . files
 *	%#	count of articles saved in current command (from 1 to n)
 *	%$	current process number
 *	%{name} Environment variable "name".  %{name-default} form allowed.
 *	%[name]	Header line beginning with "Name: ", without "Name: " 
 *	%"prompt"
 *		Print prompt and insert what is typed.
 *	%`command`
 *		Insert output of command.
 *	%(test_text=pattern?if_text:else_text)
 *		Substitute if_text if test_text matches pattern, otherwise
 *		substitute else_text.  Use != for negated match.
 *		% substitutions are done on test_text, if_text, and else_text.
 *		(Note: %() only works if CONDSUB defined.)
 *	%digit	Substitute the text matched by the nth bracket in the last
 *		pattern that had brackets.  %0 matches the last bracket
 *		matched, in case you had alternatives.
 *
 *	Put ^ in the middle to capitalize the first letter: %^C = Rec.humor
 *	Put _ in the middle to capitalize last component: %_c = net/Jokes
 *	Put \ in the middle to quote regexp and % characters in the result
 *	Put :FMT in the middle to format the result: %:-30.30t
 *
 *	~ interpretation in filename expansion happens after % expansion, so
 *	you could put ~%{NEWSLOGNAME-news} and it will expand correctly.
 */

/* *** System Dependent Stuff *** */

/* NOTE: many of these are defined in the config.h file */

/* name of organization */
#ifndef ORGNAME
#   define ORGNAME "ACME Widget Company, Widget Falls, Southern North Dakota"
#endif

#ifndef MBOXCHAR
#   define MBOXCHAR 'F'	/* how to recognize a mailbox by 1st char */
#endif

#ifndef ROOTID
#   define ROOTID 0        /* uid of superuser */
#endif

#ifdef NORMSIG
#   define sigset signal
#   define sigignore(sig) signal(sig,SIG_IGN)
#endif

#ifndef LOGDIRFIELD
#   define LOGDIRFIELD 6		/* Which field (origin 1) is the */
					/* login directory in /etc/passwd? */
					/* (If it is not kept in passwd, */
					/* but getpwnam() returns it, */
					/* define the symbol HAS_GETPWENT) */
#endif
#ifndef GCOSFIELD
#   define GCOSFIELD 5
#endif

#ifndef NEGCHAR
#   define NEGCHAR '!'
#endif

/* Space conservation section */

/* To save D space, cut down size of NGMAX and  VARYSIZE. */
#define NGMAX 100	/* number of newsgroups allowed on command line */
			/* undefine ONLY symbol to disable "only" feature */
#define VARYSIZE 256	/* this makes a block 1024 bytes long in DECville */
			/* (used by virtual array routines) */

/* Undefine any of the following features to save both I and D space */
/* In general, earlier ones are easier to get along without */
#define CUSTOMLINES	/* include code for HIDELINE and PAGESTOP */
#define WORDERASE	/* enable ^W to erase a word */
#define MAILCALL	/* check periodically for mail */
#define CLEAREOL	/* use clear to end-of-line instead of clear screen */
#define NOFIREWORKS	/* keep whole screen from flashing on certain */
			/* terminals such as older Televideos */
#define VERIFY		/* echo the command they just typed */
#define HASHNG		/* hash newsgroup lines for fast lookup-- */
			/* linear search used if not defined */
#define CONDSUB		/* allow %(cond?text:text) */
#define BACKTICK	/* allow %`command` */
#define PROMPTTTY	/* allow %"prompt" */
#define ULSMARTS	/* catch _^H in text and do underlining */
#define TERMMOD		/* allow terminal type modifier on switches */
#define BAUDMOD		/* allow baudrate modifier on switches */
#define GETLOGIN	/* use getlogin() routine as backup to environment */
			/* variables USER or LOGNAME */
#define ORGFILE		/* if organization begins with /, look up in file */
#define TILDENAME	/* allow ~logname expansion */
#define SETENV		/* allow command line environment variable setting */
#define MAKEDIR		/* use our makedir() instead of shell script */
#define MEMHELP		/* keep help messages in memory */
#define VERBOSE		/* compile in more informative messages */
#define TERSE		/* compile in shorter messages */
			/* (Note: both VERBOSE and TERSE can be defined; -t
			 * sets terse mode.  One or the other MUST be defined.
			 */
#define ROTATION	/* enable x, X and ^X commands to work */
#define DELBOGUS	/* ask if bogus newsgroups should be deleted */
#define RELOCATE	/* allow newsgroup rearranging */
#define ESCSUBS		/* escape substitutions in multi-character commands */
#undef MCHASE		/* unmark xrefed articles on m or M */
#define MUNGHEADER	/* allow alternate header formatting via */
			/* environment variable ALTHEADER (not impl) */
#define ASYNC_PARSE	/* allow parsing headers asyncronously to reading */
			/* used by MCHASE and MUNGHEADER */
#define FINDNEWNG	/* check for new newsgroups on startup */
#define FASTNEW		/* do optimizations on FINDNEWNG for faster startup */
			/* (this optimization can make occasional mistakes */
			/* if a group is removed and another group of the */
			/* same length is added, and if no softpointers are */
			/* affected by said change.) */
#define INNERSEARCH	/* search command 'g' with article */
#define CATCHUP		/* catchup command at newsgroup level */
#define NGSEARCH	/* newsgroup pattern matching */
#define ONLY		/* newsgroup restrictions by pattern */
#define KILLFILES	/* automatic article killer files */
#define ARTSEARCH	/* pattern searches among articles */
			/* /, ?, ^N, ^P, k, K */
#define EDIT_DISTANCE	/* Allow -G to specify a fuzzy 'go' command */
#undef	METAMAIL	/* use metamail to process mime articles */

/* some dependencies among options */

#ifndef ARTSEARCH
#   undef KILLFILES
#   undef INNERSEARCH
#endif

#ifndef SETUIDGID
#   define eaccess access
#endif

#ifdef ONLY				/* idiot lint doesn't grok #if */
#   define NGSORONLY
#else
#   ifdef NGSEARCH
#	define NGSORONLY
#   endif
#endif

#ifdef VERBOSE
#   ifdef TERSE
#	define IF(c) if (c)
#	define ELSE else
#   else
#	define IF(c)
#	define ELSE
#   endif
#else /* !VERBOSE */
#   ifndef TERSE
#	define TERSE
#   endif
#   define IF(c) "IF" outside of VERBOSE???
#   define ELSE "ELSE" outside of VERBOSE???
#endif

#ifdef DEBUG
#   define assert(ex) {if (!(ex)){fprintf(stderr,"Assertion failed: file %s, line %d\n", __FILE__, __LINE__);sig_catcher(0);}}
#else
#   define assert(ex) ;
#endif

/* If you're strapped for space use the help messages in shell scripts */
/* if {NG,ART,PAGER,SUBS}HELP is undefined, help messages are in memory */
#ifdef MEMHELP  /* undef MEMHELP above to get them all as sh scripts */
#   undef NGHELP
#   undef ARTHELP
#   undef PAGERHELP
#   undef SUBSHELP
#else
#   ifndef NGHELP			/* % and ~ */
#	define NGHELP "%X/ng.help"
#   endif
#   ifndef ARTHELP			/* % and ~ */
#	define ARTHELP "%X/art.help"
#   endif
#   ifndef PAGERHELP		/* % and ~ */
#	define PAGERHELP "%X/pager.help"
#   endif
#   ifndef SUBSHELP		/* % and ~ */
#	define SUBSHELP "%X/subs.help"
#   endif
#endif

#define TCSIZE 512	/* capacity for termcap strings */

#ifdef EDIT_DISTANCE
#   define MIN_DIST 7	/* Maximum error count for acceptable match */
#endif

/* Additional ideas:
 *	Make the do_newsgroup() routine a separate process.
 *	Keep .newsrc on disk instead of in memory.
 *	Overlays, if you have them.
 *	Get a bigger machine.
 */

/* End of Space Conservation Section */

/* More System Dependencies */

/* news library */
#ifndef NEWSLIB		/* ~ and %l only ("~%l" is permissable) */
#   define NEWSLIB "/usr/lib/news"
#endif

/* path to private executables */
#ifndef PRIVLIB		/* ~, %x and %l only */
#   define PRIVLIB "%x/trn"
#endif

/* system-wide RNINIT switches */
#ifndef GLOBINIT
#   define GLOBINIT "%X/INIT"
#endif

/* where to find news files */
#ifndef NEWSSPOOL		/* % and ~ */
#   define NEWSSPOOL "/usr/spool/news"
#endif

#ifndef THREAD_DIR
#   undef LONG_THREAD_NAMES
#endif

/* default characters to use in the selection menu */
#ifndef SELECTCHARS
#   define SELECTCHARS "abdefgijlorstuvwxyz1234567890BCFGHIKVW"
#endif

/* file containing list of active newsgroups and max article numbers */
#ifndef ACTIVE			/* % and ~ */
#   define ACTIVE "%x/active"
#endif
#ifndef DBINIT
#   define DBINIT "%W/db.init"
#endif

#ifdef USE_NNTP
# ifndef ACTIVE_TIMES
#   define APPEND_UNSUB
# endif
#else
# ifdef USE_XTHREAD
#   undef USE_XTHREAD
# endif
# ifdef USE_XOVER
#   undef USE_XOVER
# endif
#endif

/* location of history file */
#ifndef ARTFILE			/* % and ~ */
#    define ARTFILE "%x/history"
#endif

/* command to setup a new .newsrc */
#ifndef NEWSETUP		/* % and ~ */
#   define NEWSETUP "newsetup"
#endif

/* command to display a list of un-subscribed-to newsgroups */
#ifndef NEWSGROUPS		/* % and ~ */
#   define NEWSGROUPS "newsgroups"
#endif

/* preferred shell for use in doshell routine */
/*  ksh or sh would be okay here */
#ifndef PREFSHELL
#   define PREFSHELL "/bin/csh"
#endif

/* path to fastest starting shell */
#ifndef SH
#   define SH "/bin/sh"
#endif

/* default unshar'ing program */
#ifndef UNSHAR
#   define UNSHAR "/bin/sh"
#endif

#ifdef METAMAIL
/* default MIME extraction program */
#  ifndef MIMESTORE
#    define MIMESTORE "/usr/local/bin/mh/mhn -store -auto -file "
#  endif

/* default MIME show program */
#  ifndef MIMESHOW
#    define MIMESHOW "metamail -e -p -m \"trn %s\" %A"
#  endif
#endif


/* path to default editor */
#ifndef DEFEDITOR
#   define DEFEDITOR "/usr/ucb/vi"
#endif

/* location of macro file for trn and rn modes */
#ifndef TRNMACRO
#   define TRNMACRO "%./.trnmac"
#endif
#ifndef RNMACRO
#   define RNMACRO "%./.rnmac"
#endif

/* location of full name */
#ifndef FULLNAMEFILE
#   ifndef PASSNAMES
#	define FULLNAMEFILE "%./.fullname"
#   endif
#endif

/* virtual array file name template */
#ifndef VARYNAME		/* % and ~ */
#   define VARYNAME "/tmp/rnvary.%$"
#endif

/* where to compile a new newsgroup list */
#ifndef RNEWNAME
#   define RNEWNAME "/tmp/rnew.%$"
#endif

/* file to pass header to followup article poster */
#ifndef HEADNAME		/* % and ~ */
#   define HEADNAME "%./.rnhead"
/* or alternately #define HEADNAME "/tmp/rnhead.%$" */
#endif

#ifndef MAKEDIR
/* shell script to make n-deep subdirectories */
#   ifndef DIRMAKER		/* % and ~ */
#	define DIRMAKER "%X/makedir"
#   endif
#endif

/* location of newsrc file */
#ifndef RCNAME		/* % and ~ */
#   define RCNAME "%./.newsrc"
#endif

/* temporary newsrc file in case we crash while writing out */
#ifndef RCTNAME		/* % and ~ */
#   define RCTNAME "%./.newnewsrc"
#endif

/* newsrc file at the beginning of this session */
#ifndef RCBNAME		/* % and ~ */
#   define RCBNAME "%./.oldnewsrc"
#endif

/* if existent, contains process number of current or crashed trn */
#ifndef LOCKNAME		/* % and ~ */
#   define LOCKNAME "%./.rnlock"
#endif

/* information from last invocation of trn */
#ifndef LASTNAME		/* % and ~ */
#   define LASTNAME "%./.rnlast"
#endif

/* file with soft pointers into the active file */
#ifndef SOFTNAME		/* % and ~ */
#   define SOFTNAME "%./.rnsoft"
#endif

/* list of article numbers to mark as unread later (see M and Y cmmands) */
#ifndef RNDELNAME		/* % and ~ */
#   define RNDELNAME "%./.rndelay"
#endif

/* a motd-like file for trn */
#ifndef NEWSNEWSNAME		/* % and ~ */
#   define NEWSNEWSNAME "%X/newsnews"
#endif

/* command to send a reply */
#ifndef MAILPOSTER		/* % and ~ */
#   define MAILPOSTER "QUOTECHARS=%I Rnmail -h %h"
#endif

#ifdef INTERNET
#   ifndef MAILHEADER		/* % */
#	ifdef CONDSUB
#	    define MAILHEADER "To: %t\nSubject: Re: %S\n%(%{REPLYTO}=^$?:Reply-To: %{REPLYTO}\n)Newsgroups: %n\nIn-Reply-To: %i\n%(%[references]!=^$?References\\: %[references]\n)Organization: %o\nCc: \nBcc: \n\n"
#	else
#	    define MAILHEADER "To: %t\nSubject: Re: %S\nNewsgroups: %n\nIn-Reply-To: %i\nReferences: %[references]\nCc: \nBcc: \n\n"
#	endif
#   endif
#else
#   ifndef MAILHEADER		/* % */
#	ifdef CONDSUB
#	    define MAILHEADER "To: %T\nSubject: %(%i=^$?:Re: %S\nNewsgroups: %n\nIn-Reply-To: %i)\n%(%[references]!=^$?References\\: %[references]\n)Organization: %o\nCc: \nBcc: \n\n"
#	else
#	    define MAILHEADER "To: %T\nSubject: Re: %S\nNewsgroups: %n\nIn-Reply-To: %i\nReferences: %[references]\nCc: \nBcc: \n\n"
#	endif
#   endif
#endif

#ifndef YOUSAID			/* % */
#   define YOUSAID "In article %i you write:"
#endif

/* command to submit a followup article */
#ifndef NEWSPOSTER		/* % and ~ */
#   define NEWSPOSTER "QUOTECHARS=%I Pnews -h %h"
#endif

#ifndef NEWSHEADER		/* % */
#   ifdef CONDSUB
#	define NEWSHEADER "%(%[followup-to]=^$?:X-ORIGINAL-NEWSGROUPS: %n\n)Newsgroups: %(%F=^$?%C:%F)\nSubject: %(%S=^$?%\"\n\nSubject: \":Re: %S)\nSummary: \nExpires: \n%(%R=^$?:References: %R\n)Sender: \nFollowup-To: \n%(%{REPLYTO}=^$?:Reply-To: %{REPLYTO}\n)Distribution: %(%i=^$?%\"Distribution: \":%D)\nOrganization: %o\nKeywords: %[keywords]\nCc: \n\n"
#   else
#	define NEWSHEADER "Newsgroups: %F\nSubject: Re: %S\nSummary: \nExpires: \nReferences: %R\nSender: \nFollowup-To: \nDistribution: %D\nOrganization: %o\nKeywords: %[keywords]\nCc: \n\n"
#   endif
#endif

#ifndef ATTRIBUTION		/* % */
#   define ATTRIBUTION "In article %i %f writes:"
#endif

#ifndef PIPESAVER		/* % */
#   ifdef CONDSUB
#	define PIPESAVER "%(%B=^0$?<%A:tail +%Bc %A |) %b"
#   else
#	define PIPESAVER "tail +%Bc %A | %b"
#   endif
#endif

#ifndef EXSAVER
#   define EXSAVER "tail +%Bc %A | %e"
#endif

#ifdef METAMAIL
#  ifndef EXMIMESAVER
#    define EXMIMESAVER "%e %A"
#  endif
#endif

#ifndef NORMSAVER		/* % and ~ */
#   define NORMSAVER "%X/norm.saver %A %P %c %a %B %C \"%b\""
#endif

#ifndef MBOXSAVER		/* % and ~ */
#   ifndef ANCIENT_NEWS
#	define MBOXSAVER "%X/mbox.saver %A %P %c %a %B %C \"%b\" \"From %t %`LANG= date`\""
#   else
#	ifdef CONDSUB
#	    define MBOXSAVER "%X/mbox.saver %A %P %c %a %B %C \"%b\" \"From %t %(%[date]=^\\(\\w*\\), \\(\\w*\\)-\\(\\w*\\)-\\(\\w*\\) \\([^ ]*\\)?%1 %3 %(%2=..?%2: %2) %5 19%4)\""
					/* header munging with a vengeance */
#	else
#	    define MBOXSAVER "%X/mbox.saver %A %P %c %a %B %C \"%b\" \"From %t %[posted]\""
#	endif
#   endif
#endif

#ifdef MKDIRS

#   ifndef SAVEDIR			/* % and ~ */
#	define SAVEDIR "%p/%c"
#   endif
#   ifndef SAVENAME		/* % */
#	define SAVENAME "%a"
#   endif

#else

#   ifndef SAVEDIR			/* % and ~ */
#	define SAVEDIR "%p"
#   endif
#   ifndef SAVENAME		/* % */
#	define SAVENAME "%^C"
#   endif

#endif

#ifndef KILLGLOBAL		/* % and ~ */
#   define KILLGLOBAL "%p/KILL"
#endif

#ifndef KILLLOCAL		/* % and ~ */
#   define KILLLOCAL "%p/%c/KILL"
#endif

/* how to cancel an article */
#ifndef CANCEL
#   ifdef BNEWS
#	define CANCEL "%x/inews -h < %h"
#   else
#	define CANCEL "inews -h < %h"
#   endif
#endif

/* how to cancel an article, continued */
#ifndef CANCELHEADER
#   define CANCELHEADER "Newsgroups: %n\nSubject: cancel\nControl: cancel %i\nDistribution: %D\n\n%i was cancelled from within trn.\n"
#endif

/* how to supersede an article */
#ifndef SUPERSEDEHEADER
#   define SUPERSEDEHEADER "Newsgroups: %n\nSubject: %S\nSummary: %[summary]\nExpires: %[expires]\nReferences: %[references]\nSupersedes: %i\nSender: %[sender]\nFollowup-To: %[followup-to]\nDistribution: %D\nOrganization: %o\nKeywords: %[keywords]\n\n"
#endif

#ifndef LOCALTIMEFMT
#   define LOCALTIMEFMT "%a %b %d %X %Z %Y"
#endif

/* where to find the mail file */
#ifndef MAILFILE
#   define MAILFILE "/usr/spool/mail/%L"
#endif

/* how to open binary format files */
#ifndef FOPEN_RB
#   define FOPEN_RB "r"
#endif
#ifndef FOPEN_WB
#   define FOPEN_WB "w"
#endif

/* what to do with ansi prototypes -- '()' == ignore, 'x' == use */
#ifndef _
#   ifdef __STDC__
#	define _(x) x
#	ifndef CONST
#	    define CONST const
#	endif
#   else
#	define _(x) ()
#	ifndef CONST
#	    define CONST
#	endif
#   endif
#endif

/* how many characters is a newline in a text file? */
#ifndef NL_SIZE
#   define NL_SIZE 1
#endif

/* some important types */

typedef int		NG_NUM;		/* newsgroup number */
typedef long		ART_NUM;	/* article number */
typedef long		ART_UNREAD;	/* could be short to save space */
typedef long		ART_POS;	/* char position in article file */
typedef int		ART_LINE;	/* line position in article file */
typedef long		ACT_POS;	/* char position in active file */
typedef unsigned int	MEM_SIZE;	/* for passing to malloc */


/* *** end of the machine dependent stuff *** */

/* GLOBAL THINGS */

/* file statistics area */

EXT struct stat filestat;

/* various things of type char */

char	*index();
char	*rindex();
char	*getenv();
char	*strcat();
char	*strcpy();
char	*malloc();
char	*realloc();

EXT char buf[LBUFLEN+1];	/* general purpose line buffer */
EXT char cmd_buf[CBUFLEN];	/* buffer for formatting system commands */
EXT char *indstr INIT(">");	/* indent for old article embedded in followup */

EXT char *cwd INIT(Nullch);		/* current working directory */
EXT char *dfltcmd INIT(Nullch);	/* 1st char is default command */

/* switches */

#ifdef DEBUG
    EXT int debug INIT(0);				/* -D */
#   define DEB_COREDUMPSOK 2
#   define DEB_HEADER 4
#   define DEB_INTRP 8
#   define DEB_NNTP 16
#   define DEB_INNERSRCH 32
#   define DEB_FILEXP 64 
#   define DEB_HASH 128
#   define DEB_XREF_MARKER 256
#   define DEB_CTLAREA_BITMAP 512
#   define DEB_SOFT_POINTERS 1024
#   define DEB_NEWSRC_LINE 2048
#   define DEB_SEARCH_AHEAD 4096
#   define DEB_CHECKPOINTING 8192
#   define DEB_FEED_XREF 16384
#endif

#ifdef ARTSEARCH
    EXT int scanon INIT(0);				/* -S */
#endif

EXT bool use_threads INIT(THREAD_INIT);			/* -x */
EXT int max_tree_lines INIT(6);
EXT char select_order[4] INIT("lms");
EXT int select_on INIT(SELECT_INIT);			/* -X */
EXT char end_select INIT('Z');
EXT char page_select INIT('>');

EXT bool dont_filter_control INIT(FALSE);		/* -j */
EXT bool mbox_always INIT(FALSE);			/* -M */
EXT bool norm_always INIT(FALSE);			/* -N */
EXT bool thread_always INIT(FALSE);			/* -a */
EXT bool auto_arrow_macros INIT(TRUE);			/* -B */
EXT bool breadth_first INIT(FALSE);			/* -b */
EXT bool bkgnd_spinner INIT(FALSE);			/* -B */
EXT bool novice_delays INIT(TRUE);			/* +f */
EXT int olden_days INIT(FALSE);				/* -o */
EXT bool auto_select_postings INIT(FALSE);		/* -p */
EXT bool checkflag INIT(FALSE);				/* -c */
EXT bool suppress_cn INIT(FALSE);			/* -s */
EXT int countdown INIT(5);	/* how many lines to list before invoking -s */
EXT bool muck_up_clear INIT(FALSE);			/* -loco */
EXT bool erase_screen INIT(FALSE);			/* -e */
EXT bool can_home INIT(FALSE);
#ifdef CLEAREOL
EXT bool can_home_clear INIT(FALSE);		/* fancy -e */
#endif
EXT bool findlast INIT(FALSE);			/* -r */
EXT bool typeahead INIT(FALSE);			/* -T */
#ifdef EDIT_DISTANCE
EXT bool fuzzyGet INIT(FALSE);			/* -G */
#endif
#ifdef VERBOSE
#   ifdef TERSE
	EXT bool verbose INIT(TRUE);			/* +t */
#   endif
#endif
EXT bool unbroken_subjects INIT(FALSE);			/* -u */
#ifdef VERIFY
    EXT bool verify INIT(FALSE);			/* -v */
#endif
    EXT bool quickstart INIT(FALSE);			/* -q */

#define NOMARKING 0
#define STANDOUT 1
#define UNDERLINE 2
EXT int marking INIT(NOMARKING);			/* -m */

EXT ART_LINE initlines INIT(0);				/* -i */
EXT bool initlines_specified INIT(FALSE);
#ifdef APPEND_UNSUB
EXT bool append_unsub INIT(1);				/* -I */
#else
EXT bool append_unsub INIT(0);
#endif

/* miscellania */

int fseek();
long atol(), ftell();
EXT bool in_ng INIT(FALSE);		/* current state of trn */
EXT char mode INIT('i');		/* current state of trn */

EXT FILE *tmpfp INIT(Nullfp);	/* scratch fp used for .rnlock, .rnlast, etc. */

EXT NG_NUM nextrcline INIT(0);	/* 1st unused slot in rcline array */
			/* startup to avoid checking twice in a row */

extern errno;
/* Factored strings */

EXT char nullstr[1] INIT("");
EXT char sh[] INIT(SH);
EXT char defeditor[] INIT(DEFEDITOR);
EXT char hforhelp[] INIT("Type h for help.\n");
#ifdef STRICTCR
EXT char badcr[] INIT("\nUnnecessary CR ignored.\n");
#endif
EXT char readerr[] INIT("rn read error");
EXT char unsubto[] INIT("\n\nUnsubscribed to newsgroup %s\n");
EXT char cantopen[] INIT("Can't open %s\n");
EXT char cantcreate[] INIT("Can't create %s\n");
EXT char cantrecreate[] INIT("Can't recreate %s -- restoring older version.\n");

#ifdef VERBOSE
    EXT char nocd[] INIT("Can't chdir to directory %s\n");
#else
    EXT char nocd[] INIT("Can't find %s\n");
#endif

#ifdef METAMAIL
EXT bool mime_article INIT(FALSE);
#endif

#ifdef NOLINEBUF
#define FLUSH ,fflush(stdout)
#else
#define FLUSH
#endif

#ifdef lint
#undef FLUSH
#define FLUSH
#undef putchar
#define putchar(c)
#endif

#define advise(str) fputs(str,stdout)
#define fatal_error(str) fputs(str,stderr), finalize(1)

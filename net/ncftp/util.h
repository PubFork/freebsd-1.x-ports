/* Util.h */

#ifndef _util_h_
#define _util_h_

/*  $RCSfile: util.h,v $
 *  $Revision: 1.1 $
 *  $Date: 1994/03/01 00:31:51 $
 */

typedef char string[128], str32[32], longstring[512];
typedef char Hostname[64];

/* For Perror. */
#ifdef DB_ERRS
#	ifdef __LINE__
#		define PERROR(p,e) Perror(p, __LINE__, e)
		void Perror(char *, int, char *);
#	else
#		define PERROR(p,e) Perror(p, e)
		void Perror(char *, char *);
#	endif
#else
#	define PERROR(p,e) Perror(e)
	void Perror(char *);
#endif

#ifdef NO_VARARGS
#	define dbprintf if (debug) (void) printf
#else
#	ifndef DB_STREAM
#		define DB_STREAM stdout
#	endif
	void dbprintf(char *fmt, ...);
#endif

/* For 'Getopt.' */
#define BADCH   ((int)'?')
#define EMSG    ""

/* Handy macros. */
#define Strncpy(d,s) _Strncpy((char *) (d), (char *) (s), (size_t) sizeof(d))
#define Strncat(d,s) _Strncat((char *) (d), (char *) (s), (size_t) sizeof(d))
#define FGets(a,b) fgets((a), (int) (sizeof(a) - 2), (b))

#ifndef NO_CONST
typedef int (*cmp_t)(const void *, const void *);
#else
typedef int (*cmp_t)(void *, void *);
#endif

#define QSort(base,n,sz,cmp) \
	qsort(base, (size_t)(n), (size_t)(sz), (cmp_t)(cmp))

#ifndef SIG_PARAMS
#define SIG_PARAMS (int sig)
#endif
typedef void (*Sig_t) SIG_PARAMS;

#define Signal(a,proc) signal((a), (Sig_t)(proc))

/* Quiets warnings */
#if defined(sun)	/* ...actually, any UNIX system */
# if defined(__GNUC__)
#  undef  SIG_DFL
#  undef  SIG_IGN
#  define SIG_DFL	(Sig_t)0
#  define SIG_IGN	(Sig_t)1
# endif
#endif

/* Protos. */
char *_Strncat(char *dst, char *src, register size_t n);
char *_Strncpy(char *dst, char *src, register size_t n);
void StrLCase(char *dst);
char *NewString(char *oldstr);
char **re_makeargv(char *promptstr, int *argc);
char *onoff(int);
int StrToBool(char *s);
int confirm(char *cmd, char *file);
void fatal(char *msg);
char *get_cwd(char *buf, int size);
int tmp_name(char *str);
int Getopt(int argc, char **argv, char *opstring);
void Getopt_Reset(void);
char *Gets(char *promptstr, char *sline, size_t size);
size_t RemoveTrailingNewline(char *cp, int *stripped);
unsigned long UnLSDate(char *dstr);
unsigned long UnSIZEDate(char *dstr);
char *Strpcpy(char *dst, char *src);
int UserLoggedIn(void);
char *LocalPath(char *path);
char *LocalDotPath(char *path);

#ifdef NO_STRSTR
char *strstr(char *s1, char *s2);
#endif

#endif	/* _util_h_ */

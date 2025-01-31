/* expect_cf.h.  Generated automatically by configure.  */
/*
 * Check for headers
 */
#ifndef __EXPECT_CF_H__
#define __EXPECT_CF_H__

/* #undef NO_STDLIB_H */		/* Tcl requires this name */
/* #undef HAVE_STROPTS_H */
/* #undef HAVE_SYSCONF_H */
#define HAVE_SYS_FCNTL_H 1
#define HAVE_SYS_WAIT_H 1
/* #undef HAVE_SYS_BSDTYPES_H */	/* nice ISC special */
#define HAVE_UNISTD_H 1
/* #undef HAVE_SYSMACROS_H */

/* #undef pid_t */

/*
 * This section is for compile macros needed by
 * everything else.
 */

/*
 * Check for functions
 */
/* #undef NO_MEMCPY */
/* #undef NOWAITPID */
/* #undef HAVE_SYSCONF */
#define HAVE_WAIT4 1
/* #undef SIMPLE_EVENT */

/*
 * wait status type
 */
/* #undef NO_UNION_WAIT */

/*
 * Signal stuff. Setup the return type
 * and if signals need to be re-armed.
 */
#ifndef RETSIGTYPE
#define RETSIGTYPE void
#endif
/* #undef REARM_SIG */

/*
 * Generate correct type for select mask
 */
#ifndef SELECT_MASK_TYPE
#define SELECT_MASK_TYPE fd_set
#endif

/*
 * Check how stty works
 */
/* #undef STTY_READS_STDOUT */

/*
 * Check for tty/pty functions and structures
 */
#define POSIX 1
/* #undef HAVE_TERMIO */
#define HAVE_TERMIOS 1
#define HAVE_SGTTYB 1
/* #undef HAVE_GETPTY */
/* #undef HAVE__GETPTY */
/* #undef HAVE_PTC */
/* #undef HAVE_PTC_PTS */
/* #undef HAVE_PTYM */
/* #undef HAVE_PTYTRAP */
/* #undef HAVE_PTMX */
/* #undef HAVE_PTMX_BSD */

#endif	/* __EXPECT_CF_H__ */

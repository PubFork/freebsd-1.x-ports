#ifndef lint
static char *sccsid = "@(#)$Header: /a/cvs/386BSD/ports/news/nntp/inews/inews.c,v 1.1 1993/07/19 20:04:29 nate Exp $";
#endif

/*
 * Itty-bitty inews for talking to remote server.
 * Simply accept input on stdin (or via a named file) and dump this
 * to the server; add a From: and Path: line if missing in the original.
 * Print meaningful errors from the server.
 * Limit .signature files to MAX_SIGNATURE lines.
 * No processing of command line options.
 *
 * Original by Steven Grady <grady@ucbvax.Berkeley.EDU>, with thanks from
 * Phil Lapsley <phil@ucbvax.berkeley.edu>
 * Send bug reports to Stan Barber <sob@bcm.tmc.edu>
 */

#include <stdio.h>
#include <pwd.h>
#include <ctype.h>
#include "../common/conf.h"
#include "../common/nntp.h"
#ifdef USG
#include <string.h>
#else not USG
#include <strings.h>
#endif not USG
/* for gen_frompath() */
#define FROM 1
#define PATH 2

#define	MAX_SIGNATURE	4

extern	FILE	*ser_wr_fp;
extern	char	*getlogin();

#ifdef AUTH
FILE *passfile;
#endif AUTH

char	host_name[256];
struct	passwd *passwd;

#ifdef REALDOMAIN
#include <netdb.h>
#endif

main(argc, argv)
int	argc;
char	*argv[];
{
	char	line[NNTP_STRLEN], s[NNTP_STRLEN];
	int	seen_fromline, in_header, seen_header, seen_pathline;
	int	response;
	char	*server;
	char	*getserverbyfile();
	register char	*cp;
	char 	*username;

	/* find out who they REALLY are */
	username = getlogin();
	if (username)
		passwd = getpwnam(username);
	else
		{
		passwd = getpwuid(getuid());
		username = passwd->pw_name;
		}

#ifdef AUTH
	/*
	 * we have to be setuid to news to open the file, but not afterwards
	 */
	passfile = fopen(PASSFILE, "r");
#ifndef USG	/* this call does not exist on USG */
	seteuid(getuid());
	setegid(getgid());
#endif
#endif AUTH

	++argv;
	while (argc > 1)
		if (*argv[0] == '-') {
			++argv;
			--argc;
		} else
			break;

	if (argc > 1) {
		if (freopen(*argv, "r", stdin) == NULL) {
			perror(*argv);
			exit(1);
		}
	}

	uname(host_name);

#ifdef REALDOMAIN
	{
	struct hostent *hp;

	if (hp = gethostbyname(host_name))
		strcpy(host_name, hp->h_name);
	}
#endif

	server = getserverbyfile(SERVER_FILE);
	if (server == NULL) {
		fprintf(stderr,
			"Can't get the name of the news server from %s.\n",
			SERVER_FILE);
		fprintf(stderr,
	       "Either fix this file, or put NNTPSERVER in your enviroment.\n");
		exit(1);
	}

	response = server_init(server);
	if (response < 0) {
		printf("Couldn't connect to %s news server, try again later.\n",
			server);
		exit(1);
	}

	if (handle_server_response(response, server) < 0
	    || response == OK_NOPOST) {
		close_server();
		exit(1);
	}

again:
	put_server("POST");
	(void) get_server(line, sizeof(line));
	if (*line != CHAR_CONT) {
		if (atoi(line) == ERR_NOPOST) {
			close_server();
			fprintf(stderr,
				"Sorry, you can't post from this machine.\n");
			exit(1);
		}
#ifdef AUTH
		if (atoi(line) == ERR_NOAUTH) {
			postauth(server);
			goto again;
		}
#endif
		close_server();
		fprintf(stderr, "Remote error: %s\n", line);
		exit(1);
	}

	in_header = 1;
	seen_header = 0;
	seen_fromline = 0;
	seen_pathline = 0;

	while (fgets(s, sizeof(s), stdin) != NULL) {
		/* Strip trailing newline */
		cp = s + strlen(s) - 1;
		if (cp >= s && *cp == '\n')
			*cp = '\0';
		if (s[0] == '.')    /* Single . is eof, so put in extra one */
			(void) fputc('.', ser_wr_fp);
		if (in_header && strneql(s, "From:", sizeof("From:")-1)) {
	                seen_header = 1;
			seen_fromline = 1;
		}
		if (in_header && strneql(s, "Path:", sizeof("Path:")-1)) {
	                seen_header = 1;
			seen_pathline = 1;
		}
		if (in_header && s[0] == '\0') {
	                if (seen_header) {
		                in_header = 0;
				if (!seen_pathline)
					gen_frompath(PATH);
				if (!seen_fromline)
					gen_frompath(FROM);
				else
fprintf(ser_wr_fp, "Originator: %s@%s\r\n", passwd->pw_name, host_name);
			} else {
			        continue;
			}
		} else if (in_header) {
		        if (valid_header(s))
			        seen_header = 1;
	                else
                                continue;
		}
		fprintf(ser_wr_fp, "%s\r\n", s);
	}
	if (in_header) {
		/* Still in header after EOF?  Hmm... */
		in_header = 0;
		if (!seen_pathline)
			gen_frompath(PATH);
		if (!seen_fromline)
			gen_frompath(FROM);
		else
fprintf(ser_wr_fp, "Originator: %s@%s\r\n", passwd->pw_name, host_name);
		fprintf(ser_wr_fp, "\r\n");
	}
  
	append_signature();

	fprintf(ser_wr_fp, ".\r\n");
	(void) fflush(ser_wr_fp);
	(void) get_server(line, sizeof(line));
	if (*line != CHAR_OK) {
		if (atoi(line) == ERR_POSTFAIL) {
			close_server();
			printf("Article not accepted by server; not posted.\n");
			for (cp = line + 4; *cp && *cp != '\r'; cp++)
				if (*cp == '\\')
					putchar('\n');
				else
					putchar(*cp);
			exit(1);
		} else {
			close_server();
			fprintf(stderr, "Remote error: %s\n", line);
			exit(1);
		}
	}

	/*
	 * Close server sends the server a
	 * "quit" command for us, which is why we don't send it.
	 */

	close_server();

	exit(0);
}

/*
 * append_signature -- append the person's .signature file if
 * they have one.  Limit .signature to MAX_SIGNATURE lines.
 * The rn-style DOTDIR environmental variable is used if present.
 */

append_signature()
{
	char	line[256], sigfile[256];
	char	*cp;
	struct	passwd	*passwd;
	FILE	*fp;
	char	*index(), *getenv();
	int	count = 0;
	char	*dotdir;
	
	passwd = getpwuid(getuid());
	if (passwd == NULL)
	  return;
#ifdef DO_DOTDIR
	if ((dotdir = getenv("DOTDIR")) == NULL)
#endif
	{
	  dotdir = passwd->pw_dir;
	}

	if (dotdir[0] == '~') {
	  (void) strcpy(sigfile, passwd->pw_dir);
	  (void) strcat(sigfile, &dotdir[1]);
	} else {
	  (void) strcpy(sigfile, dotdir);
	}
	(void) strcat(sigfile, "/");
	(void) strcat(sigfile, ".signature");

#ifdef DEBUG
  fprintf(stderr,"sigfile = '%s'\n", sigfile);
#endif

	fp = fopen(sigfile, "r");
	if (fp == NULL)
		return;

#ifdef DEBUG
  fprintf(stderr,"sigfile opened OK\n");
#endif

	fprintf(ser_wr_fp, "-- \r\n");
	while (fgets(line, sizeof (line), fp)) {
		count++;
		if (count > MAX_SIGNATURE) {
			fprintf(stderr,
	      "Warning: .signature files should be no longer than %d lines.\n",
			MAX_SIGNATURE);
			fprintf(stderr,
			"(Only %d lines of your .signature were posted.)\n",
			MAX_SIGNATURE);
			break;
		}
		/* Strip trailing newline */
		cp = line + strlen(line) - 1;
		if (cp >= line && *cp == '\n')
			*cp = '\0';

		fprintf(ser_wr_fp, "%s\r\n", line);
	}
	(void) fclose(fp);
#ifdef DEBUG
	printf(".signature appended (from %s)\n", sigfile);
#endif
}


/*
 * gen_frompath -- generate From: and Path: lines, in the form
 *
 *	From: user@host.domain (full_name)
 *	Path: host!user
 *
 * This routine should only be called if the message doesn't have
 * a From: line in it.
 */

gen_frompath(which)
int which;
{
	char	*full_name;
	char	*cp;
	struct	passwd *passwd;
	char	*index(), *getenv();

	passwd = getpwuid(getuid());

	full_name = getenv("NAME");
	if (full_name == NULL) {
		full_name = passwd->pw_gecos;
		if ((cp = index(full_name, ',')))
			*cp = '\0';
	}

	if (which == FROM){
#if defined(DOMAIN) && ! defined(REALDOMAIN)
#ifdef HIDDENNET
			fprintf(ser_wr_fp, "From: %s@%s (",
				passwd->pw_name,
				DOMAIN);
#else /* HIDDENNET */

		/* A heuristic to see if we should tack on a domain */

		cp = index(host_name, '.');
		if (cp)
			fprintf(ser_wr_fp, "From: %s@%s (",
				passwd->pw_name,
				host_name);
		else
			fprintf(ser_wr_fp, "From: %s@%s.%s (",
				passwd->pw_name,
				host_name,
				DOMAIN);
#endif /* HIDDENNET */
#else
		fprintf(ser_wr_fp, "From: %s@%s (",
			passwd->pw_name,
			host_name);
#endif

		for (cp = full_name; *cp != '\0'; ++cp)
			if (*cp != '&')
				putc(*cp, ser_wr_fp);
			else {		/* Stupid & hack.  God damn it. */
				putc(toupper(passwd->pw_name[0]), ser_wr_fp);
				fprintf(ser_wr_fp, passwd->pw_name+1);
			}
		fprintf(ser_wr_fp, ")\r\n");
	}
	if (which == PATH){
#ifdef HIDDENNET
	/* Only the login name - nntp server will add uucp name */
		fprintf(ser_wr_fp, "Path: %s\r\n", passwd->pw_name);
#else /* HIDDENNET */
		fprintf(ser_wr_fp, "Path: %s!%s\r\n", host_name,
			passwd->pw_name);
#endif /* HIDDENNET */
	}
}


/*
 * strneql -- determine if two strings are equal in the first n
 * characters, ignoring case.
 *
 *	Parameters:	"a" and "b" are the pointers
 *			to characters to be compared.
 *			"n" is the number of characters to compare.
 *
 *	Returns:	1 if the strings are equal, 0 otherwise.
 *
 *	Side effects:	None.
 */

strneql(a, b, n)
register char *a, *b;
int	n;
{
	char	lower();

	while (n && lower(*a) == lower(*b)) {
		if (*a == '\0')
			return (1);
		a++;
		b++;
		n--;
	}
	if (n)
		return (0);
	else
		return (1);
}

/*
 * lower -- convert a character to lower case, if it's
 *	upper case.
 *
 *	Parameters:	"c" is the character to be
 *			converted.
 *
 *	Returns:	"c" if the character is not
 *			upper case, otherwise the lower
 *			case eqivalent of "c".
 *
 *	Side effects:	None.
 */

char lower(c)
register char c;
{
	if (isascii(c) && isupper(c))
		c = c - 'A' + 'a';
	return(c);
}


/*
 * valid_header -- determine if a line is a valid header line
 * 
 *	Parameters:	"h" is the header line to be checked.
 *
 *	Returns: 	1 if valid, 0 otherwise
 *
 *	Side Effects:	none
 *
 */

int valid_header(h)
register char *h;
{
  char *index();
  char *colon, *space;
  
  /*
   * blank or tab in first position implies this is a continuation header
   */
  if (h[0] == ' ' || h[0] == '\t')
    return (1);

  /*
   * just check for initial letter, colon, and space to make
   * sure we discard only invalid headers
   */
  colon = index(h, ':');
  space = index(h, ' ');
  if (isalpha(h[0]) && colon && space == colon + 1)
    return (1);

  /*
   * anything else is a bad header -- it should be ignored
   */
  return (0);
}

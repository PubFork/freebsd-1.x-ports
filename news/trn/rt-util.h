/* $Id: rt-util.h,v 1.1 1993/07/19 20:07:07 nate Exp $
*/

char *extract_name _((char*));
char *compress_from _((ARTICLE*,int));
char *compress_name _((char*,int));
char *compress_subj _((ARTICLE*,int));
char *get_subject_start _((char*));
#ifndef HAS_STRCASECMP
int strCASEcmp _((char*,char*));
int strnCASEcmp _((char*,char*,int));
#endif

EXT char spin_char INIT(' ');	/* char to put back when we're done spinning */

#define SPIN_OFF	0
#define SPIN_POP	1
#define SPIN_FOREGROUND	2
#define SPIN_BACKGROUND 3

void setspin _((int));
void spin _((int));

// utils.h                                               -*- C++ -*-
/*

Copyright (C) 1992, 1993 John W. Eaton

This file is part of Octave.

Octave is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

Octave is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with Octave; see the file COPYING.  If not, write to the Free
Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#if !defined (_utils_h)
#define _utils_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <time.h>

class istream;
class ostrstream;
class tree_constant;

extern char *strsave (const char *);
extern char *strconcat (const char *, const char *);
extern void discard_until (istream&, char);
extern void check_dimensions (int& nr, int& nc, char *warnfor);
extern void raw_mode (int);
extern int kbhit (void);
extern char **pathstring_to_vector (char *);
extern char *default_path (void);
extern char *get_site_defaults (void);
extern char *default_pager (void);
extern char *file_in_path (char *, char *);
extern char *m_file_in_path (char *);
extern char *polite_directory_format (char *);
extern int absolute_pathname (char *);
extern int absolute_program (char *);
extern char *base_pathname (char *);
extern int read_octal (char *);
extern char *sub_append_string (char *, char *, int *, int *);
extern char *decode_prompt_string (char *);
extern void pathname_backup (char *, int);
extern char *make_absolute (char *, char *);
extern char *get_working_directory (char *);
extern int change_to_directory (char *);
extern int is_newer (char *, time_t);
extern volatile void jump_to_top_level (void);
extern char *s_plural (int);
extern char *es_plural (int);
extern char *save_in_tmp_file (tree_constant& t, int nd = 2, int para = 0);
extern void mark_for_deletion (const char *);
extern void cleanup_tmp_files (void);
extern int send_to_plot_stream (const char *cmd);
extern void close_plot_stream (void);
extern int almost_match (char *std, char *s, int min_match_len = 1);
extern char **get_m_file_names (int& mfl_len, char *dir, int no_suffix);
extern char **get_m_file_names (int& mfl_len, int no_suffix);
extern int NINT (double x);
extern double D_NINT (double x);

#endif

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; page-delimiter: "^/\\*" ***
;;; End: ***
*/

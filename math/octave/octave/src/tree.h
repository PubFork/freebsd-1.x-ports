// Tree classes.                                      -*- C++ -*-
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

#if !defined (_tree_h)
#define _tree_h 1

// This seems to leave vt$tree undefined with g++ 2.3.3.
#if 0
#ifdef __GNUG__
#pragma interface
#endif
#endif

#include <stdio.h>

class ostrstream;

#include "builtins.h"
#include "tree-base.h"
#include "tree-const.h"

class symbol_record;
class symbol_table;

#ifndef TREE_FCN_TYPEDEFS
#define TREE_FCN_TYPEDEFS 1

typedef tree_constant (*Text_fcn)(int, char **);
typedef tree_constant* (*General_fcn)(tree_constant *, int, int);

#endif

#ifndef NULL_TREE
#define NULL_TREE (tree *) NULL
#endif

#ifndef NULL_TREE_CONST
#define NULL_TREE_CONST (tree_constant *) NULL
#endif

/*
 * Forward declarations.
 */
class tree;
class tree_constant_rep;
class tree_constant;
class tree_matrix;
class tree_builtin;
class tree_identifier;
class tree_function;
class tree_expression;
class tree_prefix_expression;
class tree_postfix_expression;
class tree_unary_expression;
class tree_binary_expression;
class tree_assignment_expression;
class tree_simple_assignment_expression;
class tree_multi_assignment_expression;
class tree_colon_expression;
class tree_index_expression;
class tree_argument_list;
class tree_parameter_list;
class tree_return_list;
class tree_word_list;
class tree_command;
class tree_command_list;
class tree_while_command;
class tree_for_command;
class tree_if_command;
class tree_break_command;
class tree_continue_command;
class tree_return_command;
class tree_word_list_command;;
class tree_plot_limits;
class tree_plot_range;
class tree_subplot;
class tree_subplot_using;
class tree_subplot_style;
class tree_subplot_list;
class tree_plot_command;

/*
 * General matrices.  This allows us to construct matrices from
 * other matrices, variables, and functions.
 */
class
tree_matrix : public tree
{
public:
  tree_matrix (void);
  tree_matrix (tree *t, tree::matrix_dir d);

  ~tree_matrix (void);

  tree_matrix *chain (tree *t, tree::matrix_dir d);
  tree_matrix *reverse (void);
  int length (void);

  tree_return_list *to_return_list (void);

  tree_constant eval (int print);

private:
  tree::matrix_dir dir; // Direction to the next element.
  tree *element;
  tree_matrix *next;
};

/*
 * Builtin functions.
 */
class
tree_builtin : public tree
{
public:
  tree_builtin (void);
  tree_builtin (int i_max, int o_max, Mapper_fcn& m_fcn, symbol_record *s);
  tree_builtin (int i_max, int o_max, Text_fcn t_fcn, symbol_record *s);
  tree_builtin (int i_max, int o_max, General_fcn t_fcn, symbol_record *s);

  ~tree_builtin (void);

  int is_builtin (void);

  tree_constant eval (int print);

  tree_constant *eval (int print, int nargout);

  tree_constant eval (int argc, char **argv, int print);

  tree_constant *eval (tree_constant *args, int n_in, int n_out, int print);

  tree *def (void);
  char *name (void);

  int max_expected_args (void);

private:
  int nargin_max;
  int nargout_max;
  Mapper_fcn mapper_fcn;
  Text_fcn text_fcn;
  General_fcn general_fcn;
  symbol_record *sym;       // Is this really needed?  It points back
			    // to the symbol record that contains this
			    // builtin function...
};

/*
 * Symbols from the symbol table.
 */
class
tree_identifier : public tree
{
  friend class tree_index_expression;

public:
  tree_identifier (void);
  tree_identifier (symbol_record *s);

  ~tree_identifier (void);

  int is_identifier (void);

  tree *def (void);
  char *name (void);
  symbol_record *symrec (void);

  tree_identifier *define (tree_constant *t);
  tree_identifier *define (tree_function *t);

  void document (char *s);

  tree_constant assign (tree_constant& t);
  tree_constant assign (tree_constant& t, tree_constant *args, int nargs);

  void bump_value (tree::expression_type);

  int parse_m_file (int exec_script = 1);
  int parse_m_file (char *mf, int exec_script = 1);
  void parse_m_file (FILE *mfile, char *mf);

  tree *do_lookup (int& script_file_executed);

  void mark_as_formal_parameter (void);

  tree_constant eval (int print);

  tree_constant *eval (int print, int nargout);

  tree_constant eval (int argc, char **argv, int print);
  
  tree_constant *eval (tree_constant *args, int n_in, int n_out, int print);

  void eval_undefined_error (void);

private:
  symbol_record *sym;
};

/*
 * User defined functions.
 */
class
tree_function : public tree
{
public:
  tree_function (void);
  tree_function (tree *cl, symbol_table *st);

  ~tree_function (void);

  tree_function *define (tree *t);
  tree_function *define_param_list (tree_parameter_list *t);
  tree_function *define_ret_list (tree_parameter_list *t);

  void stash_m_file_name (char * s);
  void stash_m_file_time (time_t t);

  char *m_file_name (void);
  time_t time_parsed (void);

  tree_constant eval (int print);

  tree_constant *eval (int print, int nargout);

  tree_constant eval (int argc, char **argv, int print);

  tree_constant *eval (tree_constant *args, int n_in, int n_out, int print);

  int max_expected_args (void);

private:
  int call_depth;
  tree_parameter_list *param_list;
  tree_parameter_list *ret_list;
  symbol_table *sym_tab;
  tree *cmd_list;
  time_t t_parsed;
  char *file_name;
};

/*
 * A base class for expressions.
 */
class
tree_expression : public tree
{
public:
  tree_expression (void);

  ~tree_expression (void);

  tree_constant eval (int print);

protected:
  expression_type etype;
};

/*
 * Prefix expressions.
 */
class
tree_prefix_expression : public tree_expression
{
 public:
  tree_prefix_expression (void);
  tree_prefix_expression (tree_identifier *t, tree::expression_type et);

  ~tree_prefix_expression (void);

  tree_constant eval (int print);

 private:
  tree_identifier *id;
};

/*
 * Postfix expressions.
 */
class
tree_postfix_expression : public tree_expression
{
 public:
  tree_postfix_expression (void);
  tree_postfix_expression (tree_identifier *t, tree::expression_type et);

  ~tree_postfix_expression (void);

  tree_constant eval (int print);

 private:
  tree_identifier *id;
};

/*
 * Unary expressions.
 */
class
tree_unary_expression : public tree_expression
{
 public:
  tree_unary_expression (void);
  tree_unary_expression (tree *a, tree::expression_type t);

  ~tree_unary_expression (void);

  tree_constant eval (int print);

 private:
  tree *op;
};

/*
 * Binary expressions.
 */
class
tree_binary_expression : public tree_expression
{
 public:
  tree_binary_expression (void);
  tree_binary_expression (tree *a, tree *b, tree::expression_type t);

  ~tree_binary_expression (void);

  tree_constant eval (int print);

 private:
  tree *op1;
  tree *op2;
};

/*
 * Assignment expressions.
 */
class
tree_assignment_expression : public tree_expression
{
public:
  int in_parens;

  tree_assignment_expression (void);

  ~tree_assignment_expression (void);

  tree_constant eval (int print);

  int is_assignment_expression (void);
};

/*
 * Simple assignment expressions.
 */
class
tree_simple_assignment_expression : public tree_assignment_expression
{
 public:
  tree_simple_assignment_expression (void);
  tree_simple_assignment_expression (tree_identifier *i, tree *r);
  tree_simple_assignment_expression (tree_index_expression *idx_expr, tree *r);

  ~tree_simple_assignment_expression (void);

  tree_constant eval (int print);

 private:
  tree_identifier *lhs;
  tree_argument_list *index;
  tree *rhs;
};

/*
 * Multi-valued assignment expressions.
 */
class
tree_multi_assignment_expression : public tree_assignment_expression
{
 public:
  tree_multi_assignment_expression (void);
  tree_multi_assignment_expression (tree_return_list *l, tree *r);

  ~tree_multi_assignment_expression (void);

  tree_constant eval (int print);

  tree_constant *eval (int print, int nargout);

 private:
  tree_return_list *lhs;
  tree *rhs;
};

/*
 * Colon expressions.
 */
class
tree_colon_expression : public tree_expression
{
 public:
  tree_colon_expression (void);
  tree_colon_expression (tree *a, tree *b);

  ~tree_colon_expression (void);

  tree_colon_expression *chain (tree *t);

  tree_constant eval (int print);

 private:
  tree *op1;
  tree *op2;
  tree *op3;
};

/*
 * Index expressions.
 */
class
tree_index_expression : public tree_expression
{
 public:
  tree_index_expression (void);
  tree_index_expression (tree_identifier *i);
  tree_index_expression (tree_identifier *i, tree_argument_list *l);

  ~tree_index_expression (void);

  int is_index_expression (void);

  tree_identifier *ident (void);

  tree_argument_list *arg_list (void);

  tree_constant eval (int print);

  tree_constant *eval (int print, int nargout);

 private:
  tree_identifier *id;
  tree_argument_list *list;
};

/*
 * Argument lists.
 */
class
tree_argument_list : public tree
{
 public:
  tree_argument_list (void);
  tree_argument_list (tree *t);

  ~tree_argument_list (void);

  tree_argument_list *chain (tree *t);
  tree_argument_list *reverse (void);
  int length (void);

  tree_argument_list *next_elem (void);

  tree_constant *convert_to_const_vector (int& nargs);

  tree_constant eval (int print);

 private:
  tree *arg;
  tree_argument_list *next;
};

/*
 * Parameter lists.  Almost like argument lists, except that the
 * elements are only supposed to be identifiers, never constants or
 * expressions.
 */
class
tree_parameter_list : public tree
{
 public:
  tree_parameter_list (void);
  tree_parameter_list (tree_identifier *t);

  ~tree_parameter_list (void);

  tree_parameter_list *chain (tree_identifier *t);
  tree_parameter_list *reverse (void);
  int length (void);

  char *name (void);

  void mark_as_formal_parameters (void);

  tree_identifier *define (tree_constant *t);

  tree_parameter_list *next_elem (void);

  tree_constant eval (int print);

 private:
  tree_identifier *param;
  tree_parameter_list *next;
};

/*
 * Return lists.  Almost like parameter lists, except that the
 * elements may also be index expressions.
 */
class
tree_return_list : public tree
{
 public:
  tree_return_list (void);
  tree_return_list (tree_identifier *t);
  tree_return_list (tree_index_expression *t);

  ~tree_return_list (void);

  tree_return_list *chain (tree_identifier *t);
  tree_return_list *chain (tree_index_expression *t);
  tree_return_list *reverse (void);
  int length (void);

  tree_index_expression *idx_expr (void);

  tree_return_list *next_elem (void);

  tree_constant eval (int print);

 private:
  tree_index_expression *retval;
  tree_return_list *next;
};

/*
 * Word lists.
 */
class
tree_word_list : public tree
{
 public:
  tree_word_list (void);
  tree_word_list (char *s);

  ~tree_word_list (void);

  tree_word_list *chain (char *s);
  tree_word_list *reverse (void);
  int length (void);

  char *name (void);

  tree_word_list *next_elem (void);

  tree_constant eval (int print);

 private:
  char *word;
  tree_word_list *next;
};

/*
 * A base class for commands.
 */
class
tree_command : public tree
{
};

/*
 * A command or two to be executed.
 */
class
tree_command_list : public tree_command
{
 public:
  tree_command_list (void);
  tree_command_list (tree *t);

  ~tree_command_list (void);

  tree_command_list *chain (tree *t);
  tree_command_list *reverse (void);

  void set_print_flag (int print);

  tree_constant eval (int print);

 private:
  tree *command;		// Command to execute.
  int print_flag;		// Print result of eval for this command?
  tree_command_list *next;	// Next command in list.
};

/*
 * While.
 */
class
tree_while_command : public tree_command
{
 public:
  tree_while_command (void);
  tree_while_command (tree *e);
  tree_while_command (tree *e, tree *l);

  ~tree_while_command (void);

  tree_constant eval (int print);

 private:
  tree *expr;			// Expression to test.
  tree *list;			// List of commands to execute.
};

/*
 * For.
 */
class
tree_for_command : public tree_command
{
 public:
  tree_for_command (void);
  tree_for_command (tree_identifier *id, tree *e, tree *l);

  ~tree_for_command (void);

  tree_constant eval (int print);

 private:
  tree_identifier *id;		// Identifier to modify.
  tree *expr;			// Expression to evaluate.
  tree *list;			// List of commands to execute.
};

/*
 * Simple if.
 */
class
tree_if_command : public tree_command
{
 public:
  tree_if_command (void);
  tree_if_command (tree *t);
  tree_if_command (tree *e, tree *t);

  ~tree_if_command (void);

  tree_if_command *chain (tree *t);
  tree_if_command *chain (tree *t1, tree *t2);
  tree_if_command *reverse (void);

  tree_constant eval (int print);

 private:
  tree *expr;			// Expression to test.
  tree *list;			// Commands to execute.
  tree_if_command *next;	// Next if command.
};

/*
 * Break.
 */
class
tree_break_command : public tree_command
{
 public:
  tree_break_command (void);

  ~tree_break_command (void);

  tree_constant eval (int print);
};

/*
 * Continue.
 */
class
tree_continue_command : public tree_command
{
 public:
  tree_continue_command (void);

  ~tree_continue_command (void);

  tree_constant eval (int print);
};

/*
 * Return.
 */
class
tree_return_command : public tree_command
{
 public:
  tree_return_command (void);

  ~tree_return_command (void);

  tree_constant eval (int print);
};

/*
 * Functions that take a list of strings as arguments.
 */
class
tree_word_list_command : public tree_command
{
 public:
  tree_word_list_command (void);
  tree_word_list_command (tree_identifier *id, tree_word_list *wl);

  ~tree_word_list_command (void);

  tree_constant eval (int print);

 private:
  tree_identifier *ident;
  tree_word_list *word_list;
};

class
tree_plot_command : public tree_command
{
 public:
  tree_plot_command (void);
  tree_plot_command (tree_subplot_list *plt, int nd);
  tree_plot_command (tree_subplot_list *plt, tree_plot_limits *rng, int nd);

  ~tree_plot_command (void);

  tree_constant eval (int print);

 private:
  int ndim;
  tree_plot_limits *range;
  tree_subplot_list *plot_list;
};

class
tree_subplot_list : public tree
{
 public:
  tree_subplot_list (void);
  tree_subplot_list (tree *data);
  tree_subplot_list (tree_subplot_list *t);
  tree_subplot_list (tree_subplot_using *u, tree *t, tree_subplot_style *s);

  ~tree_subplot_list (void);

  tree_subplot_list *set_data (tree *data);

  tree_subplot_list *chain (tree_subplot_list *t);

  tree_subplot_list *reverse (void);

  tree_subplot_list *next_elem (void);

  tree_constant eval (int print);
//  tree_constant *eval (int print, int nargout);

  int print (int ndim, ostrstream& plot_buf);

 private:
  tree *plot_data;
  tree_subplot_using *using;
  tree *title;
  tree_subplot_style *style;
  tree_subplot_list *next;
};

class
tree_plot_limits : public tree
{
 public:
  tree_plot_limits (void);
  tree_plot_limits (tree_plot_range *xlim);
  tree_plot_limits (tree_plot_range *xlim, tree_plot_range *ylim);
  tree_plot_limits (tree_plot_range *xlim, tree_plot_range *ylim,
		    tree_plot_range *zlim);

  ~tree_plot_limits (void);

  tree_constant eval (int print);

  void print (int print, ostrstream& plot_buf);

 private:
  tree_plot_range *x_range;
  tree_plot_range *y_range;
  tree_plot_range *z_range;
};

class
tree_plot_range : public tree
{
 public:
  tree_plot_range (void);
  tree_plot_range (tree *l, tree *u);

  ~tree_plot_range (void);

  tree_constant eval (int print);

  void print (ostrstream& plot_buf);

 private:
  tree *lower;
  tree *upper;
};

class
tree_subplot_using : public tree
{
 public:
  tree_subplot_using (void);
  tree_subplot_using (tree *fmt);

  ~tree_subplot_using (void);

  tree_subplot_using *set_format (tree *fmt);

  tree_subplot_using *add_qualifier (tree *t);

  tree_constant eval (int print);

  int print (int ndim, int n_max, ostrstream& plot_buf);

 private:
  int qualifier_count;
  tree *x[4];
  tree *scanf_fmt;
};

class
tree_subplot_style : public tree
{
 public:
  tree_subplot_style (void);
  tree_subplot_style (char *s);
  tree_subplot_style (char *s, tree *lt);
  tree_subplot_style (char *s, tree *lt, tree *pt);

  ~tree_subplot_style (void);

  tree_constant eval (int print);

  int print (ostrstream& plot_buf);

 private:
  char *style;
  tree *linetype;
  tree *pointtype;
};

#endif

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; page-delimiter: "^/\\*" ***
;;; End: ***
*/

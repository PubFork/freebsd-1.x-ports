//
// hfstools - a Macintosh filesystem access tool
// (C) Copyright 1993 by Equivalence
//
// This file part of hfs.
//
// hfs is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
// 
// hfs is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with hfs; see the file COPYING.  If not, write to
// the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  
//
//
// $Id: args.cxx,v 1.1 1994/05/15 05:44:51 rich Exp $
// $Log: args.cxx,v $
// Revision 1.1  1994/05/15 05:44:51  rich
// Initial revision
//
// Revision 1.6  1994/01/06  03:05:08  craigs
// Final checkin to include GNU header
//
// Revision 1.5  1993/12/30  07:47:48  craigs
// Changed to use DIR_SEP macro
// Added strlwr on programName for MSDOS/NT
//
// Revision 1.4  1993/12/23  15:12:56  craigs
// Fixed error regarding undeclared variable for DOS/NT
//
// Revision 1.3  1993/12/16  20:12:18  craigs
// Separated program name parsing from option parsing, and
// removed indirection from argc and argv
//
// Revision 1.2  1993/11/23  20:03:32  craigs
// Added MSDOS/Windows compatibility
//   by robertj
//
// Revision 1.1  1993/11/22  22:25:55  craigs
// Initial revision
//
//
//

#include <string.h>
#include <iostream.h>
#include <stdlib.h>
#include <malloc.h>

#include "args.h"

/////////////////////////////////////////////////////
//
//  ArgList::ParseProgname
//

void ArgList::ParseProgname(char * argv0)

{
  // extract the program name 
  if ((programName = strrchr (argv0, DIR_SEP)) == NULL) {
    programName = argv0;
    programPath = "";
  } else {
    *programName++ = '\0';
    programPath = argv0;
  }

  // for MSDOS and NT, remove the .EXE or .COM on the end
#if defined(_MSDOS) || defined(WIN32)
  char *p;
  if ((p = strrchr(programName, '.')) != NULL) 
    *p ='\0';
  strlwr(programName);
#endif
}


/////////////////////////////////////////////////////
//
//  ArgList::ArgList
//

ArgList::ArgList (char *argv0)

{
  // get the program name and path
  ParseProgname(argv0);

  arg_values = NULL;
  arg_count  = 0;
}


/////////////////////////////////////////////////////
//
//  ArgList::ArgList
//

ArgList::ArgList (int theArgc,
              char ** theArgv,
         const char * theArgumentSpec = NULL)

{
  // get the program name and path
  ParseProgname(theArgv[0]);

  // if we have no argument spec, then delay parsing the arguments
  // until later
  if (theArgumentSpec == NULL) {
    arg_values = NULL;
    arg_count  = 0;
    return;
  }

  // we got an argument spec - so skip argv[0] and process them
  Parse(theArgc-1, theArgv+1, theArgumentSpec);
}

/////////////////////////////////////////////////////
//
//  ArgList::ParseArgs
//

void ArgList::Parse (int theArgc,
                 char ** theArgv,
            const char * theArgumentSpec)


{
  char c;
  char *p;
  int l;

  // save argv and and argc for later
  arg_values = theArgv;
  arg_count = theArgc;
  shift = 0;

  // allocate and initialise storage
  argumentSpec = strdup (theArgumentSpec);
  l = strlen (argumentSpec);
  optionList   = (int *) calloc (l, sizeof (int));
  argumentList = (char **) calloc (l, sizeof (char *));

next_argv:
  for (;arg_count > 0 && *arg_values[0] == '-';--arg_count,++arg_values) {
    while ((c = *++arg_values[0]) != 0)
      if ((p = strchr (argumentSpec, c)) != NULL) {
        optionList [p-argumentSpec]++;
        if (p[1] == ':') {
          if (*++(arg_values[0]))
            argumentList[p-argumentSpec] = arg_values[0];
          else {
            if (arg_count < 2) {
              optionList [p-argumentSpec] = 0;
              MissingArgument (c);
            } else {
              --arg_count;
              argumentList [p-argumentSpec] = *++arg_values;
            }
          }
          goto next_argv;
        }
      } else 
        UnknownOption (c);
  }
}



/////////////////////////////////////////////////////
//
//  ArgList::~ArgList
//

ArgList::~ArgList ()

{
  free(optionList);
  free(argumentList);
}

/////////////////////////////////////////////////////
//
//  ArgList::OptionSpecified
//

int ArgList::OptionSpecified (const char option) const

{
  char *p = strchr (argumentSpec, option);
  return (p == NULL ? 0 : optionList[p-argumentSpec]);
}

/////////////////////////////////////////////////////
//
//  ArgList::GetOption
//

char * ArgList::GetOption (const char option) const

{
  char *p = strchr (argumentSpec, option);
  return (p == NULL ? NULL : argumentList[p-argumentSpec]);
}


/////////////////////////////////////////////////////
//
//  ArgList::GetArgument
//

char * ArgList::GetArgument (const int argnum) const

{
  if ((argnum+shift) < arg_count)
    return arg_values[argnum+shift];
  else {
    IllegalArgumentIndex(argnum+shift);
    return NULL;
  }
}

/*
 * Dialog definitions
 *
 * Copyright 1993 Alexandre Julliard
 */

#ifndef DIALOG_H
#define DIALOG_H

#include "windows.h"


  /* Dialog info structure.
   * This structure is stored into the window extra bytes (cbWndExtra).
   * sizeof(DIALOGINFO) must be <= DLGWINDOWEXTRA (=30).
   */
typedef struct
{
    LONG      msgResult;
    FARPROC   dlgProc;
    LONG      userInfo;
    HWND      hwndFocus;
    HFONT     hUserFont;
    HMENU     hMenu;
    WORD      xBaseUnit;
    WORD      yBaseUnit;
    WORD      fEnd;
} DIALOGINFO;


  /* Dialog template header */
typedef struct
{
    DWORD     style;    
    BYTE      nbItems WINE_PACKED;
    WORD      x WINE_PACKED;
    WORD      y WINE_PACKED;
    WORD      cx WINE_PACKED;
    WORD      cy WINE_PACKED;
} DLGTEMPLATEHEADER;


  /* Dialog control header */
typedef struct
{
    WORD       x;
    WORD       y;
    WORD       cx;
    WORD       cy;
    WORD       id;
    DWORD      style WINE_PACKED;
} DLGCONTROLHEADER;


  /* Dialog template */
typedef struct
{
    DLGTEMPLATEHEADER * header;
    unsigned char *     menuName;
    LPSTR               className;
    LPSTR               caption;
    WORD                pointSize;
    LPSTR               faceName;
} DLGTEMPLATE;


#endif  /* DIALOG_H */

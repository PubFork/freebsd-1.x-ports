/*
 * GDI Device Context functions
 *
 * Copyright 1993 Alexandre Julliard
 */

static char Copyright[] = "Copyright  Alexandre Julliard, 1993";

#include <stdlib.h>
#include <string.h>
#include "gdi.h"
#include "bitmap.h"
#include "metafile.h"

static DeviceCaps * displayDevCaps = NULL;

extern const WIN_DC_INFO DCVAL_defaultValues;

extern void CLIPPING_SetDeviceClipping( DC * dc );     /* objects/clipping.c */
extern WORD COLOR_ToPhysical( DC *dc, COLORREF color );/* objects/color.c */
extern void COLOR_SetMapping( DC *dc, HANDLE, WORD );  /* objects/color.c */


  /* ROP code to GC function conversion */
const int DC_XROPfunction[16] =
{
    GXclear,        /* R2_BLACK */
    GXnor,          /* R2_NOTMERGEPEN */
    GXandInverted,  /* R2_MASKNOTPEN */
    GXcopyInverted, /* R2_NOTCOPYPEN */
    GXandReverse,   /* R2_MASKPENNOT */
    GXinvert,       /* R2_NOT */
    GXxor,          /* R2_XORPEN */
    GXnand,         /* R2_NOTMASKPEN */
    GXand,          /* R2_MASKPEN */
    GXequiv,        /* R2_NOTXORPEN */
    GXnoop,         /* R2_NOP */
    GXorInverted,   /* R2_MERGENOTPEN */
    GXcopy,         /* R2_COPYPEN */
    GXorReverse,    /* R2_MERGEPENNOT */
    GXor,           /* R2_MERGEPEN */
    GXset           /* R2_WHITE */
};


/***********************************************************************
 *           DC_FillDevCaps
 *
 * Fill the device caps structure.
 */
void DC_FillDevCaps( DeviceCaps * caps )
{
    caps->version       = 0x300; 
    caps->technology    = DT_RASDISPLAY;
    caps->horzSize      = WidthMMOfScreen(screen) * screenWidth / WidthOfScreen(screen);
    caps->vertSize      = HeightMMOfScreen(screen) * screenHeight / HeightOfScreen(screen);
    caps->horzRes       = screenWidth;
    caps->vertRes       = screenHeight;
    caps->bitsPixel     = screenDepth;
    caps->planes        = 1;
    caps->numBrushes    = 0;
    caps->numPens       = 0;
    caps->numMarkers    = 0;
    caps->numFonts      = 0;
    caps->numColors     = 1 << caps->bitsPixel;
    caps->pdeviceSize   = 0;
    caps->curveCaps     = CC_CIRCLES | CC_PIE | CC_CHORD | CC_ELLIPSES |
	                  CC_WIDE | CC_STYLED | CC_WIDESTYLED | 
			  CC_INTERIORS | CC_ROUNDRECT;
    caps->lineCaps      = LC_POLYLINE | LC_MARKER | LC_POLYMARKER | LC_WIDE |
	                  LC_STYLED | LC_WIDESTYLED | LC_INTERIORS;
    caps->polygonalCaps = PC_POLYGON | PC_RECTANGLE | PC_WINDPOLYGON |
	                  PC_SCANLINE | PC_WIDE | PC_STYLED | 
			  PC_WIDESTYLED | PC_INTERIORS;
    caps->textCaps      = TC_OP_CHARACTER | TC_OP_STROKE | TC_CP_STROKE |
	                  TC_IA_ABLE | TC_UA_ABLE | TC_SO_ABLE | TC_RA_ABLE;
    caps->clipCaps      = CP_REGION;
    caps->rasterCaps    = RC_BITBLT | RC_BANDING | RC_SCALING | RC_BITMAP64 |
	                  RC_DI_BITMAP | RC_PALETTE | RC_DIBTODEV | RC_BIGFONT|
			  RC_STRETCHBLT | RC_STRETCHDIB | RC_DEVBITS;
    caps->aspectX       = 36;  /* ?? */
    caps->aspectY       = 36;  /* ?? */
    caps->aspectXY      = 51;
    caps->logPixelsX    = (int)(caps->horzRes * 25.4 / caps->horzSize);
    caps->logPixelsY    = (int)(caps->vertRes * 25.4 / caps->vertSize);
    caps->sizePalette   = DefaultVisual(display,DefaultScreen(display))->map_entries;
    caps->numReserved   = 0;
    caps->colorRes      = 0;
}


/***********************************************************************
 *           DC_InitDC
 *
 * Setup device-specific DC values for a newly created DC.
 */
static void DC_InitDC( HDC hdc )
{
    DC * dc = (DC *) GDI_GetObjPtr( hdc, DC_MAGIC );
    RealizeDefaultPalette( hdc );
    SetTextColor( hdc, dc->w.textColor );
    SetBkColor( hdc, dc->w.backgroundColor );
    SelectObject( hdc, dc->w.hPen );
    SelectObject( hdc, dc->w.hBrush );
    SelectObject( hdc, dc->w.hFont );
    XSetGraphicsExposures( XT_display, dc->u.x.gc, False );
    CLIPPING_SetDeviceClipping( dc );
    FONT_SelectObject(dc, STOCK_SYSTEM_FIXED_FONT, NULL);
}


/***********************************************************************
 *           DC_SetupDCForBrush
 *
 * Setup dc->u.x.gc for drawing operations using current brush.
 * Return 0 if brush is BS_NULL, 1 otherwise.
 */
int DC_SetupGCForBrush( DC * dc )
{
    XGCValues val;
    unsigned long mask = 0;

    if (dc->u.x.brush.style == BS_NULL) return 0;
    if (dc->u.x.brush.pixel == -1)
    {
	/* Special case used for monochrome pattern brushes.
	 * We need to swap foreground and background because
	 * Windows does it the wrong way...
	 */
	val.foreground = dc->w.backgroundPixel;
	val.background = dc->w.textPixel;
    }
    else
    {
	val.foreground = dc->u.x.brush.pixel;
	val.background = dc->w.backgroundPixel;
    }
    val.function = DC_XROPfunction[dc->w.ROPmode-1];
    val.fill_style = dc->u.x.brush.fillStyle;
    if (val.fill_style == FillStippled)
    {
	if (dc->w.backgroundMode==OPAQUE) val.fill_style = FillOpaqueStippled;
	val.stipple = dc->u.x.brush.pixmap;
	mask = GCStipple;
    }
    else if (val.fill_style == FillTiled)
    {
	val.tile = dc->u.x.brush.pixmap;
	mask = GCTile;
    }
    val.ts_x_origin = dc->w.DCOrgX + dc->w.brushOrgX;
    val.ts_y_origin = dc->w.DCOrgY + dc->w.brushOrgY;
    val.fill_rule = (dc->w.polyFillMode==WINDING) ? WindingRule : EvenOddRule;
    XChangeGC( display, dc->u.x.gc, 
	       GCFunction | GCForeground | GCBackground | GCFillStyle |
	       GCFillRule | GCTileStipXOrigin | GCTileStipYOrigin | mask,
	       &val );
    return 1;
}


/***********************************************************************
 *           DC_SetupDCForPen
 *
 * Setup dc->u.x.gc for drawing operations using current pen.
 * Return 0 if pen is PS_NULL, 1 otherwise.
 */
int DC_SetupGCForPen( DC * dc )
{
    XGCValues val;

    if (dc->u.x.pen.style == PS_NULL) return 0;
    val.function   = DC_XROPfunction[dc->w.ROPmode-1];
    val.foreground = dc->u.x.pen.pixel;
    val.background = dc->w.backgroundPixel;
    val.fill_style = FillSolid;
    if ((dc->u.x.pen.style!=PS_SOLID) && (dc->u.x.pen.style!=PS_INSIDEFRAME))
    {
	XSetDashes( display, dc->u.x.gc, 0,
		    dc->u.x.pen.dashes, dc->u.x.pen.dash_len );
	val.line_style = (dc->w.backgroundMode == OPAQUE) ?
	                      LineDoubleDash : LineOnOffDash;
    }
    else val.line_style = LineSolid;
    val.line_width = dc->u.x.pen.width;
    val.cap_style  = CapRound;
    val.join_style = JoinBevel;
    XChangeGC( display, dc->u.x.gc, 
	       GCFunction | GCForeground | GCBackground | GCLineWidth |
	       GCLineStyle | GCCapStyle | GCJoinStyle | GCFillStyle, &val );
    return 1;    
}


/***********************************************************************
 *           DC_SetupDCForText
 *
 * Setup dc->u.x.gc for text drawing operations.
 * Return 0 if the font is null, 1 otherwise.
 */
int DC_SetupGCForText( DC * dc )
{
    XGCValues val;

    if (!dc->u.x.font.fstruct)
    {
	FONT_SelectObject(dc, STOCK_SYSTEM_FIXED_FONT, NULL);
    }
    val.function   = DC_XROPfunction[dc->w.ROPmode-1];
    val.foreground = dc->w.textPixel;
    val.background = dc->w.backgroundPixel;
    val.fill_style = FillSolid;
    val.font       = dc->u.x.font.fstruct->fid;
    XChangeGC( display, dc->u.x.gc, 
	       GCFunction | GCForeground | GCBackground | GCFillStyle |
	       GCFont, &val );
    return 1;
}


/***********************************************************************
 *           GetDCState    (GDI.179)
 */
HDC GetDCState( HDC hdc )
{
    DC * newdc, * dc;
    HANDLE handle;
    
    if (!(dc = (DC *) GDI_GetObjPtr( hdc, DC_MAGIC ))) return 0;
    if (!(handle = GDI_AllocObject( sizeof(DC), DC_MAGIC ))) return 0;
    newdc = (DC *) GDI_HEAP_ADDR( handle );

#ifdef DEBUG_DC
    printf( "GetDCState(%d): returning %d\n", hdc, handle );
#endif

    memcpy( &newdc->w, &dc->w, sizeof(dc->w) );
    newdc->saveLevel = 0;
    newdc->w.flags |= DC_SAVED;

    if (dc->w.hClipRgn)
    {
	newdc->w.hClipRgn = CreateRectRgn( 0, 0, 0, 0 );
	CombineRgn( newdc->w.hClipRgn, dc->w.hClipRgn, 0, RGN_COPY );
    }
    if (dc->w.hVisRgn)
    {
	newdc->w.hVisRgn = CreateRectRgn( 0, 0, 0, 0 );
	CombineRgn( newdc->w.hVisRgn, dc->w.hVisRgn, 0, RGN_COPY );	
    }
    newdc->w.hGCClipRgn = 0;
    COLOR_SetMapping( newdc, dc->u.x.pal.hMapping, dc->u.x.pal.mappingSize );
    return handle;
}


/***********************************************************************
 *           SetDCState    (GDI.180)
 */
void SetDCState( HDC hdc, HDC hdcs )
{
    DC * dc, * dcs;
    
    if (!(dc = (DC *) GDI_GetObjPtr( hdc, DC_MAGIC ))) return;
    if (!(dcs = (DC *) GDI_GetObjPtr( hdcs, DC_MAGIC ))) return;
    if (!dcs->w.flags & DC_SAVED) return;
#ifdef DEBUG_DC
    printf( "SetDCState: %d %d\n", hdc, hdcs );
#endif
    if (dc->w.hClipRgn)	DeleteObject( dc->w.hClipRgn );
    if (dc->w.hVisRgn) DeleteObject( dc->w.hVisRgn );
    if (dc->w.hGCClipRgn) DeleteObject( dc->w.hGCClipRgn );

    memcpy( &dc->w, &dcs->w, sizeof(dc->w) );
    memcpy( &dc->u.x.pen, &dcs->u.x.pen, sizeof(dc->u.x.pen) );
    dc->w.hClipRgn = dc->w.hVisRgn = dc->w.hGCClipRgn = 0;
    dc->w.flags &= ~DC_SAVED;

    SelectObject( hdc, dcs->w.hBrush );
    SelectObject( hdc, dcs->w.hFont );
    COLOR_SetMapping( dc, dcs->u.x.pal.hMapping, dcs->u.x.pal.mappingSize );

    SelectClipRgn( hdc, dcs->w.hClipRgn );
    SelectVisRgn( hdc, dcs->w.hVisRgn );
}


/***********************************************************************
 *           SaveDC    (GDI.30)
 */
int SaveDC( HDC hdc )
{
    HDC hdcs;
    DC * dc, * dcs;

    dc = (DC *) GDI_GetObjPtr( hdc, DC_MAGIC );
    if (!dc) 
    {
	dc = (DC *)GDI_GetObjPtr(hdc, METAFILE_DC_MAGIC);
	if (!dc) return 0;
	MF_MetaParam0(dc, META_SAVEDC);
	return 1;  /* ?? */
    }
    if (!(hdcs = GetDCState( hdc ))) return 0;
    dcs = (DC *) GDI_HEAP_ADDR( hdcs );
    dcs->header.hNext = dc->header.hNext;
    dc->header.hNext = hdcs;
#ifdef DEBUG_DC
    printf( "SaveDC(%d): returning %d\n", hdc, dc->saveLevel+1 );
#endif    
    return ++dc->saveLevel;
}


/***********************************************************************
 *           RestoreDC    (GDI.39)
 */
BOOL RestoreDC( HDC hdc, short level )
{
    DC * dc, * dcs;

#ifdef DEBUG_DC
    printf( "RestoreDC: %d %d\n", hdc, level );
#endif    
    dc = (DC *) GDI_GetObjPtr( hdc, DC_MAGIC );
    if (!dc) 
    {
	dc = (DC *)GDI_GetObjPtr(hdc, METAFILE_DC_MAGIC);
	if (!dc) return FALSE;
	if (level != -1) return FALSE;
	MF_MetaParam1(dc, META_RESTOREDC, level);
	return TRUE;
    }
    if (level == -1) level = dc->saveLevel;
    if ((level < 1) || (level > (short)dc->saveLevel)) return FALSE;
    
    while ((short)dc->saveLevel >= level)
    {
	HDC hdcs = dc->header.hNext;
	if (!(dcs = (DC *) GDI_GetObjPtr( hdcs, DC_MAGIC ))) return FALSE;
	dc->header.hNext = dcs->header.hNext;
	if ((short)--dc->saveLevel < level) SetDCState( hdc, hdcs );
	DeleteDC( hdcs );
    }
    return TRUE;
}


/***********************************************************************
 *           CreateDC    (GDI.53)
 */
HDC CreateDC( LPSTR driver, LPSTR device, LPSTR output, LPSTR initData )
{
    DC * dc;
    HANDLE handle;
    
    handle = GDI_AllocObject( sizeof(DC), DC_MAGIC );
    if (!handle) return 0;
    dc = (DC *) GDI_HEAP_ADDR( handle );

#ifdef DEBUG_DC
    printf( "CreateDC(%s %s %s): returning %d\n", driver, device, output, handle );
#endif

    if (!displayDevCaps)
    {
	displayDevCaps = (DeviceCaps *) malloc( sizeof(DeviceCaps) );
	DC_FillDevCaps( displayDevCaps );
    }

    dc->saveLevel = 0;
    memcpy( &dc->w, &DCVAL_defaultValues, sizeof(DCVAL_defaultValues) );
    memset( &dc->u.x, 0, sizeof(dc->u.x) );

    dc->u.x.drawable   = rootWindow;
    dc->u.x.gc         = XCreateGC( display, dc->u.x.drawable, 0, NULL );
    dc->w.flags        = 0;
    dc->w.devCaps      = displayDevCaps;
    dc->w.bitsPerPixel = displayDevCaps->bitsPixel;
    dc->w.DCSizeX      = displayDevCaps->horzRes;
    dc->w.DCSizeY      = displayDevCaps->vertRes;

    DC_InitDC( handle );

    return handle;
}


/***********************************************************************
 *           CreateIC    (GDI.153)
 */
HDC CreateIC( LPSTR driver, LPSTR device, LPSTR output, LPSTR initData )
{
      /* Nothing special yet for ICs */
    return CreateDC( driver, device, output, initData );
}


/***********************************************************************
 *           CreateCompatibleDC    (GDI.52)
 */
HDC CreateCompatibleDC( HDC hdc )
{
    DC * dc;
    HANDLE handle;
    
    handle = GDI_AllocObject( sizeof(DC), DC_MAGIC );
    if (!handle) return 0;
    dc = (DC *) GDI_HEAP_ADDR( handle );

#ifdef DEBUG_DC
    printf( "CreateCompatibleDC(%d): returning %d\n", hdc, handle );
#endif

    dc->saveLevel = 0;
    memcpy( &dc->w, &DCVAL_defaultValues, sizeof(DCVAL_defaultValues) );
    memset( &dc->u.x, 0, sizeof(dc->u.x) );

    dc->u.x.drawable   = XCreatePixmap( display, rootWindow, 1, 1, 1 );
    dc->u.x.gc         = XCreateGC( display, dc->u.x.drawable, 0, NULL );
    dc->w.flags        = DC_MEMORY;
    dc->w.bitsPerPixel = 1;
    dc->w.devCaps      = displayDevCaps;
    dc->w.DCSizeX      = 1;
    dc->w.DCSizeY      = 1;

    SelectObject( handle, BITMAP_hbitmapMemDC );
    DC_InitDC( handle );

    return handle;
}


/***********************************************************************
 *           DeleteDC    (GDI.68)
 */
BOOL DeleteDC( HDC hdc )
{
    DC * dc = (DC *) GDI_GetObjPtr( hdc, DC_MAGIC );
    if (!dc) return FALSE;

#ifdef DEBUG_DC
    printf( "DeleteDC: %d\n", hdc );
#endif

    while (dc->saveLevel)
    {
	DC * dcs;
	HDC hdcs = dc->header.hNext;
	if (!(dcs = (DC *) GDI_GetObjPtr( hdcs, DC_MAGIC ))) break;
	dc->header.hNext = dcs->header.hNext;
	dc->saveLevel--;
	DeleteDC( hdcs );
    }
    
    if (!(dc->w.flags & DC_SAVED))
    {
	SelectObject( hdc, STOCK_BLACK_PEN );
	SelectObject( hdc, STOCK_WHITE_BRUSH );
	SelectObject( hdc, STOCK_SYSTEM_FONT );
	XFreeGC( display, dc->u.x.gc );
    }
    
    if (dc->w.hClipRgn) DeleteObject( dc->w.hClipRgn );
    if (dc->w.hVisRgn) DeleteObject( dc->w.hVisRgn );
    if (dc->w.hGCClipRgn) DeleteObject( dc->w.hGCClipRgn );
    
    return GDI_FreeObject( hdc );
}


/***********************************************************************
 *           GetDeviceCaps    (GDI.80)
 */
int GetDeviceCaps( HDC hdc, WORD cap )
{
    DC * dc = (DC *) GDI_GetObjPtr( hdc, DC_MAGIC );
    if (!dc) return 0;

    if (cap > sizeof(DeviceCaps)-sizeof(WORD)) return 0;
    
#ifdef DEBUG_DC
    printf( "GetDeviceCaps(%d,%d): returning %d\n",
	    hdc, cap, *(WORD *)(((char *)dc->w.devCaps) + cap) );
#endif
    return *(WORD *)(((char *)dc->w.devCaps) + cap);
}


/***********************************************************************
 *           SetBkColor    (GDI.1)
 */
COLORREF SetBkColor( HDC hdc, COLORREF color )
{
    COLORREF oldColor;
    DC * dc = (DC *) GDI_GetObjPtr( hdc, DC_MAGIC );
    if (!dc) 
    {
	dc = (DC *)GDI_GetObjPtr(hdc, METAFILE_DC_MAGIC);
	if (!dc) return 0x80000000;
	MF_MetaParam2(dc, META_SETBKCOLOR, HIWORD(color), LOWORD(color));
	return 0;  /* ?? */
    }

    oldColor = dc->w.backgroundColor;
    dc->w.backgroundColor = color;
    dc->w.backgroundPixel = COLOR_ToPhysical( dc, color );
    return oldColor;
}


/***********************************************************************
 *           SetTextColor    (GDI.9)
 */
COLORREF SetTextColor( HDC hdc, COLORREF color )
{
    COLORREF oldColor;
    DC * dc = (DC *) GDI_GetObjPtr( hdc, DC_MAGIC );
    if (!dc) 
    {
	dc = (DC *)GDI_GetObjPtr(hdc, METAFILE_DC_MAGIC);
	if (!dc) return 0x80000000;
	MF_MetaParam2(dc, META_SETTEXTCOLOR, HIWORD(color), LOWORD(color));
	return 0;  /* ?? */
    }

    oldColor = dc->w.textColor;
    dc->w.textColor = color;
    dc->w.textPixel = COLOR_ToPhysical( dc, color );
    return oldColor;
}


/***********************************************************************
 *           SetDCOrg    (GDI.117)
 */
DWORD SetDCOrg( HDC hdc, short x, short y )
{
    DWORD prevOrg;
    DC * dc = (DC *) GDI_GetObjPtr( hdc, DC_MAGIC );
    if (!dc) return 0;
    prevOrg = dc->w.DCOrgX | (dc->w.DCOrgY << 16);
    dc->w.DCOrgX = x;
    dc->w.DCOrgY = y;
    return prevOrg;
}

# $Id: gdi.spec,v 1.1.1.3 1994/05/19 07:54:31 hsu Exp $
#
name	gdi
id	3
length	490

1   pascal SetBkColor(word long) SetBkColor(1 2)
2   pascal SetBkMode(word word) SetBkMode(1 2)
3   pascal SetMapMode(word word) SetMapMode(1 2)
4   pascal SetROP2(word word) SetROP2(1 2)
5   pascal SetRelAbs(word word) SetRelAbs(1 2)
6   pascal SetPolyFillMode(word word) SetPolyFillMode(1 2)
7   pascal SetStretchBltMode(word word) SetStretchBltMode(1 2)
8   pascal SetTextCharacterExtra(word s_word) SetTextCharacterExtra(1 2)
9   pascal SetTextColor(word long) SetTextColor(1 2)
10  pascal SetTextJustification(word s_word s_word) SetTextJustification(1 2 3)
11  pascal SetWindowOrg(word s_word s_word) SetWindowOrg(1 2 3)
12  pascal SetWindowExt(word s_word s_word) SetWindowExt(1 2 3)
13  pascal SetViewportOrg(word s_word s_word) SetViewportOrg(1 2 3)
14  pascal SetViewportExt(word s_word s_word) SetViewportExt(1 2 3)
15  pascal OffsetWindowOrg(word s_word s_word) OffsetWindowOrg(1 2 3)
16  pascal ScaleWindowExt(word s_word s_word s_word s_word)
	   ScaleWindowExt(1 2 3 4 5)
17  pascal OffsetViewportOrg(word s_word s_word) OffsetViewportOrg(1 2 3)
18  pascal ScaleViewportExt(word s_word s_word s_word s_word)
	   ScaleViewportExt(1 2 3 4 5)
19  pascal LineTo(word s_word s_word) LineTo(1 2 3)
20  pascal MoveTo(word s_word s_word) MoveTo(1 2 3)
21  pascal ExcludeClipRect(word s_word s_word s_word s_word)
	   ExcludeClipRect(1 2 3 4 5)
22  pascal IntersectClipRect(word s_word s_word s_word s_word)
	   IntersectClipRect(1 2 3 4 5)
23  pascal Arc(word s_word s_word s_word s_word s_word s_word s_word s_word)
	   Arc(1 2 3 4 5 6 7 8 9)
24  pascal Ellipse(word s_word s_word s_word s_word) Ellipse(1 2 3 4 5)
25  pascal FloodFill(word word word long) FloodFill(1 2 3 4)
26  pascal Pie(word s_word s_word s_word s_word s_word s_word s_word s_word)
	   Pie(1 2 3 4 5 6 7 8 9)
27  pascal Rectangle(word s_word s_word s_word s_word) Rectangle(1 2 3 4 5)
28  pascal RoundRect(word s_word s_word s_word s_word s_word s_word)
	   RoundRect(1 2 3 4 5 6 7)
29  pascal PatBlt(word s_word s_word s_word s_word long) PatBlt(1 2 3 4 5 6)
30  pascal SaveDC(word) SaveDC(1)
31  pascal SetPixel(word s_word s_word long) SetPixel(1 2 3 4)
32  pascal OffsetClipRgn(word s_word s_word) OffsetClipRgn(1 2 3)
33  pascal TextOut(word s_word s_word ptr word) TextOut(1 2 3 4 5)
34  pascal BitBlt( word s_word s_word s_word s_word word s_word s_word long)
	   BitBlt(1 2 3 4 5 6 7 8 9)
35  pascal StretchBlt( word s_word s_word s_word s_word word s_word s_word s_word s_word long)
           StretchBlt(1 2 3 4 5 6 7 8 9 10 11)
36  pascal Polygon (word ptr word) Polygon (1 2 3)
37  pascal Polyline (word ptr word) Polyline (1 2 3)
#38  pascal Escape
39  pascal RestoreDC(word s_word) RestoreDC(1 2)
40  pascal FillRgn(word word word) FillRgn(1 2 3)
#41  pascal FrameRgn
#42  pascal InvertRgn
43  pascal PaintRgn(word word) PaintRgn(1 2)
44  pascal SelectClipRgn(word word) SelectClipRgn(1 2)
45  pascal SelectObject(word word) SelectObject(1 2)
#46  pascal __GP?
47  pascal CombineRgn(word word word word) CombineRgn(1 2 3 4)
48  pascal CreateBitmap(word word word word ptr) CreateBitmap(1 2 3 4 5)
49  pascal CreateBitmapIndirect(ptr) CreateBitmapIndirect(1)
50  pascal CreateBrushIndirect(ptr) CreateBrushIndirect(1)
51  pascal CreateCompatibleBitmap(word word word) CreateCompatibleBitmap(1 2 3)
52  pascal CreateCompatibleDC(word) CreateCompatibleDC(1)
53  pascal CreateDC(ptr ptr ptr ptr) CreateDC(1 2 3 4)
54  pascal CreateEllipticRgn(s_word s_word s_word s_word)
	   CreateEllipticRgn(1 2 3 4)
55  pascal CreateEllipticRgnIndirect(ptr) CreateEllipticRgnIndirect(1)
56  pascal CreateFont(s_word s_word s_word s_word s_word word word word 
	              word word word word word ptr)
	   CreateFont(1 2 3 4 5 6 7 8 9 10 11 12 13 14)
57  pascal CreateFontIndirect(ptr) CreateFontIndirect(1)
58  pascal CreateHatchBrush(word long) CreateHatchBrush(1 2)
60  pascal CreatePatternBrush(word) CreatePatternBrush(1)
61  pascal CreatePen(s_word s_word long) CreatePen(1 2 3)
62  pascal CreatePenIndirect(ptr) CreatePenIndirect(1)
63  pascal CreatePolygonRgn(ptr word word) CreatePolygonRgn(1 2 3)
64  pascal CreateRectRgn(s_word s_word s_word s_word) CreateRectRgn(1 2 3 4)
65  pascal CreateRectRgnIndirect(ptr) CreateRectRgnIndirect(1)
66  pascal CreateSolidBrush(long) CreateSolidBrush(1)
67  pascal DPtoLP(word ptr s_word) DPtoLP(1 2 3)
68  pascal DeleteDC(word) DeleteDC(1)
69  pascal DeleteObject(word) DeleteObject(1)
70  pascal EnumFonts(word ptr ptr ptr) EnumFonts(1 2 3 4)
#71  pascal EnumObjects
72  pascal EqualRgn(word word) EqualRgn(1 2)
73  pascal ExcludeVisRect(word s_word s_word s_word s_word)
	   ExcludeVisRect(1 2 3 4 5)
74  pascal GetBitmapBits(word long ptr) GetBitmapBits(1 2 3)
75  pascal GetBkColor(word) GetBkColor(1)
76  pascal GetBkMode(word) GetBkMode(1)
77  pascal GetClipBox(word ptr) GetClipBox(1 2)
78  pascal GetCurrentPosition(word) GetCurrentPosition(1)
79  pascal GetDCOrg(word) GetDCOrg(1)
80  pascal GetDeviceCaps(word s_word) GetDeviceCaps(1 2)
81  pascal GetMapMode(word) GetMapMode(1)
82  pascal GetObject(word word ptr) GetObject(1 2 3)
83  pascal GetPixel(word s_word s_word) GetPixel(1 2 3)
84  pascal GetPolyFillMode(word) GetPolyFillMode(1)
85  pascal GetROP2(word) GetROP2(1)
86  pascal GetRelAbs(word) GetRelAbs(1)
87  pascal GetStockObject(word) GetStockObject(1)
88  pascal GetStretchBltMode(word) GetStretchBltMode(1)
89  pascal GetTextCharacterExtra(word) GetTextCharacterExtra(1)
90  pascal GetTextColor(word) GetTextColor(1)
91  pascal GetTextExtent(word ptr s_word) GetTextExtent(1 2 3)
#92  pascal GetTextFace
93  pascal GetTextMetrics(word ptr) GetTextMetrics(1 2)
94  pascal GetViewportExt(word) GetViewportExt(1)
95  pascal GetViewportOrg(word) GetViewportOrg(1)
96  pascal GetWindowExt(word) GetWindowExt(1)
97  pascal GetWindowOrg(word) GetWindowOrg(1)
98  pascal IntersectVisRect(word s_word s_word s_word s_word)
	   IntersectVisRect(1 2 3 4 5)
99  pascal LPtoDP(word ptr s_word) LPtoDP(1 2 3)
100 pascal LineDDA(s_word s_word s_word s_word ptr long)
	   LineDDA(1 2 3 4 5 6)
101 pascal OffsetRgn(word s_word s_word) OffsetRgn(1 2 3)
102 pascal OffsetVisRgn(word s_word s_word) OffsetVisRgn(1 2 3)
103 pascal PtVisible(word s_word s_word) PtVisible(1 2 3)
104 pascal RectVisibleOld(word ptr) RectVisible(1 2)
105 pascal SelectVisRgn(word word) SelectVisRgn(1 2)
106 pascal SetBitmapBits(word long ptr) SetBitmapBits(1 2 3)
117 pascal SetDCOrg(word s_word s_word) SetDCOrg(1 2 3)
#121 pascal Death
#122 pascal ReSurRection
#123 pascal PlayMetaFile
#124 pascal GetMetaFile
125 pascal CreateMetaFile(ptr) CreateMetaFile(1)
126 pascal CloseMetaFile(word) CloseMetaFile(1)
127 pascal DeleteMetaFile(word) DeleteMetaFile(1)
128 pascal MulDiv(s_word s_word s_word) MulDiv(1 2 3)
129 pascal SaveVisRgn(word) SaveVisRgn(1)
130 pascal RestoreVisRgn(word) RestoreVisRgn(1)
131 pascal InquireVisRgn(word) InquireVisRgn(1)
#132 pascal SetEnvironment
#133 pascal GetEnvironment
134 pascal GetRgnBox(word ptr) GetRgnBox(1 2)
#135 pascal ScanLr
#136 pascal RemoveFontResource
148 pascal SetBrushOrg(word s_word s_word) SetBrushOrg(1 2 3)
149 pascal GetBrushOrg(word) GetBrushOrg(1)
150 pascal UnrealizeObject(word) UnrealizeObject(1)
#151 pascal CopyMetaFile
153 pascal CreateIC(ptr ptr ptr ptr) CreateIC(1 2 3 4)
154 pascal GetNearestColor(word long) GetNearestColor(1 2)
#155 pascal QueryAbort
#156 pascal CreateDiscardableBitmap
#159 pascal GetMetaFileBits
#160 pascal SetMetaFileBits
161 pascal PtInRegion(word s_word s_word) PtInRegion(1 2 3)
162 pascal GetBitmapDimension(word) GetBitmapDimension(1)
163 pascal SetBitmapDimension(word s_word s_word) SetBitmapDimension(1 2 3)
#169 pascal IsDCDirty
#170 pascal SetDCStatus
172 pascal SetRectRgn(word s_word s_word s_word s_word) SetRectRgn(1 2 3 4 5)
173 pascal GetClipRgn(word) GetClipRgn(1)
#175 pascal EnumMetaFile
179 pascal GetDCState(word) GetDCState(1)
180 pascal SetDCState(word word) SetDCState(1 2)
181 pascal RectInRegionOld(word ptr) RectInRegion(1 2)
#190 pascal SetDCHook
#191 pascal GetDCHook
#192 pascal SetHookFlags
#193 pascal SetBoundsRect
#194 pascal GetBoundsRect
#195 pascal SelectBitmap
#196 pascal SetMetaFileBitsBetter
#201 pascal DMBITBLT
#202 pascal DMCOLORINFO
#206 pascal DMENUMDFONTS
#207 pascal DMENUMOBJ
#208 pascal DMOUTPUT
#209 pascal DMPIXEL
#210 pascal DMREALIZEOBJECT
#211 pascal DMSTRBLT
#212 pascal DMSCANLR
#213 pascal BRUTE
#214 pascal DMEXTTEXTOUT
#215 pascal DMGETCHARWIDTH
#216 pascal DMSTRETCHBLT
#217 pascal DMDIBBITS
#218 pascal DMSTRETCHDIBITS
#219 pascal DMSETDIBTODEV
#220 pascal DMTRANSPOSE
#230 pascal CREATEPQ
#231 pascal MINPQ
#232 pascal EXTRACTPQ
#233 pascal INSERTPQ
#234 pascal SIZEPQ
#235 pascal DELETEPQ
#240 pascal OPENJOB
#241 pascal WRITESPOOL
#242 pascal WRITEDIALOG
#243 pascal CLOSEJOB
#244 pascal DELETEJOB
#245 pascal GETSPOOLJOB
#246 pascal STARTSPOOLPAGE
#247 pascal ENDSPOOLPAGE
#248 pascal QUERYJOB
250 pascal Copy(ptr ptr word) Copy(1 2 3)
#253 pascal DeleteSpoolPage
#254 pascal SpoolFile
#300 pascal ENGINEENUMERATEFONT
#301 pascal ENGINEDELETEFONT
#302 pascal ENGINEREALIZEFONT
#303 pascal ENGINEGETCHARWIDTH
#304 pascal ENGINESETFONTCONTEXT
#305 pascal ENGINEGETGLYPHBMP
#306 pascal ENGINEMAKEFONTDIR
#307 pascal GETCHARABCWIDTHS
#308 pascal GETOUTLINETEXTMETRICS
#309 pascal GETGLYPHOUTLINE
#310 pascal CREATESCALABLEFONTRESOURCE
#311 pascal GETFONTDATA
#312 pascal CONVERTOUTLINEFONTFILE
#313 pascal GETRASTERIZERCAPS
#314 pascal ENGINEEXTTEXTOUT
330 pascal EnumFontFamilies(word ptr ptr ptr) EnumFontFamilies(1 2 3 4)
#332 pascal GETKERNINGPAIRS
345 pascal GetTextAlign(word) GetTextAlign(1)
346 pascal SetTextAlign(word word) SetTextAlign(1 2)
348 pascal Chord(word s_word s_word s_word s_word s_word s_word s_word s_word)
	   Chord(1 2 3 4 5 6 7 8 9)
349 pascal SetMapperFlags(word word) SetMapperFlags(1 2)
350 pascal GetCharWidth(word word word ptr) GetCharWidth(1 2 3 4)
351 pascal ExtTextOut(word s_word s_word word ptr ptr s_word ptr) 
		ExtTextOut(1 2 3 4 5 6 7 8)
#352 pascal GETPHYSICALFONTHANDLE
#353 pascal GETASPECTRATIOFILTER
#354 pascal SHRINKGDIHEAP
360 pascal CreatePalette(ptr) CreatePalette(1)
361 pascal GDISelectPalette(word word) GDISelectPalette(1 2)
362 pascal GDIRealizePalette(word) GDIRealizePalette(1)
363 pascal GetPaletteEntries(word word word ptr) GetPaletteEntries(1 2 3 4)
364 pascal SetPaletteEntries(word word word ptr) SetPaletteEntries(1 2 3 4)
365 pascal RealizeDefaultPalette(word) RealizeDefaultPalette(1)
#366 pascal UPDATECOLORS
#367 pascal ANIMATEPALETTE
#368 pascal RESIZEPALETTE
370 pascal GetNearestPaletteIndex(word long) GetNearestPaletteIndex(1 2)
375 pascal GetSystemPaletteEntries(word word word ptr)
	   GetSystemPaletteEntries(1 2 3 4)
#376 pascal RESETDC
#377 pascal STARTDOC
#378 pascal ENDDOC
#379 pascal STARTPAGE
#380 pascal ENDPAGE
#381 pascal SETABORTPROC
#382 pascal ABORTDOC
#400 pascal FASTWINDOWFRAME
#401 pascal GDIMOVEBITMAP
#403 pascal GDIINIT2
#405 pascal FINALGDIINIT
#407 pascal CREATEUSERBITMAP
#409 pascal CREATEUSERDISCARDABLEBITMAP
#410 pascal ISVALIDMETAFILE
411 pascal GetCurLogFont(word) GetCurLogFont(1)
#412 pascal ISDCCURRENTPALETTE
#439 pascal STRETCHDIBITS
440 pascal SetDIBits(word word word word ptr ptr word) SetDIBits(1 2 3 4 5 6 7)
441 pascal GetDIBits(word word word word ptr ptr word) GetDIBits(1 2 3 4 5 6 7)
442 pascal CreateDIBitmap(word ptr long ptr ptr word)
	   CreateDIBitmap(1 2 3 4 5 6)
443 pascal SetDIBitsToDevice(word s_word s_word word word word word word word ptr ptr word)
	   SetDIBitsToDevice(1 2 3 4 5 6 7 8 9 10 11 12)
444 pascal CreateRoundRectRgn(s_word s_word s_word s_word s_word s_word)
	   CreateRoundRectRgn(1 2 3 4 5 6)
445 pascal CreateDIBPatternBrush(word word) CreateDIBPatternBrush(1 2)
#449 pascal DEVICECOLORMATCH
#450 pascal POLYPOLYGON
451 pascal CreatePolyPolygonRgn(ptr ptr word word)
	   CreatePolyPolygonRgn(1 2 3 4)
#452 pascal GDISEEGDIDO
#460 pascal GDITASKTERMINATION
#461 pascal SETOBJECTOWNER
#462 pascal ISGDIOBJECT
#463 pascal MAKEOBJECTPRIVATE
#464 pascal FIXUPBOGUSPUBLISHERMETAFILE
465 pascal RectVisible(word ptr) RectVisible(1 2)
466 pascal RectInRegion(word ptr) RectInRegion(1 2)
#467 pascal UNICODETOANSI
468 pascal GetBitmapDimensionEx(word ptr) GetBitmapDimensionEx(1 2)
469 pascal GetBrushOrgEx(word ptr) GetBrushOrgEx(1 2)
470 pascal GetCurrentPositionEx(word ptr) GetCurrentPositionEx(1 2)
471 pascal GetTextExtentPoint(word ptr s_word ptr) GetTextExtentPoint(1 2 3 4)
472 pascal GetViewportExtEx(word ptr) GetViewportExtEx(1 2)
473 pascal GetViewportOrgEx(word ptr) GetViewportOrgEx(1 2)
474 pascal GetWindowExtEx(word ptr) GetWindowExtEx(1 2)
475 pascal GetWindowOrgEx(word ptr) GetWindowOrgEx(1 2)
476 pascal OffsetViewportOrgEx(word s_word s_word ptr)
	   OffsetViewportOrgEx(1 2 3 4)
477 pascal OffsetWindowOrgEx(word s_word s_word ptr) OffsetWindowOrgEx(1 2 3 4)
478 pascal SetBitmapDimensionEx(word s_word s_word ptr)
	   SetBitmapDimensionEx(1 2 3 4)
479 pascal SetViewportExtEx(word s_word s_word ptr) SetViewportExtEx(1 2 3 4)
480 pascal SetViewportOrgEx(word s_word s_word ptr) SetViewportOrgEx(1 2 3 4)
481 pascal SetWindowExtEx(word s_word s_word ptr) SetWindowExtEx(1 2 3 4)
482 pascal SetWindowOrgEx(word s_word s_word ptr) SetWindowOrgEx(1 2 3 4)
483 pascal MoveToEx(word s_word s_word ptr) MoveToEx(1 2 3 4)
484 pascal ScaleViewportExtEx(word s_word s_word s_word s_word ptr)
	   ScaleViewportExtEx(1 2 3 4 5 6)
485 pascal ScaleWindowExtEx(word s_word s_word s_word s_word ptr)
	   ScaleWindowExtEx(1 2 3 4 5 6)
#486 pascal GETASPECTRATIOFILEREX

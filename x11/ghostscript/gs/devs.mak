#    Copyright (C) 1989, 1992, 1993 Aladdin Enterprises.  All rights reserved.
#
# This file is part of Ghostscript.
#
# Ghostscript is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY.  No author or distributor accepts responsibility
# to anyone for the consequences of using it or for whether it serves any
# particular purpose or works at all, unless he says so in writing.  Refer
# to the Ghostscript General Public License for full details.
#
# Everyone is granted permission to copy, modify and redistribute
# Ghostscript, but only under the conditions described in the Ghostscript
# General Public License.  A copy of this license is supposed to have been
# given to you along with Ghostscript so you can know your rights and
# responsibilities.  It should be in a file named COPYING.  Among other
# things, the copyright notice and this notice must be preserved on all
# copies.

# makefile for Ghostscript device drivers.

# -------------------------------- Catalog ------------------------------- #

# It is possible to build Ghostscript with an arbitrary collection of
# device drivers, although some drivers are supported only on a subset
# of the target platforms.  The currently available drivers are:

# Displays:
#   MS-DOS EGA and VGA:
#	ega	EGA (640x350, 16-color)
#	vga	VGA (640x480, 16-color)
#   MS-DOS SuperVGA:
# +	atiw	ATI Wonder SuperVGA, 256-color modes
# +     atiw16  ATI Wonder SuperVGA in 800x600, 16-color mode
#	s3vga	SuperVGA with S3 86C911 chip (e.g., Diamond Stealth board)
#	tseng	SuperVGA using Tseng Labs ET3000/4000 chips, 256-color modes
#	tseng16  Tseng Labs SuperVGA in 800x600, 16-color mode (256K memory)
# +	tvga	Trident SuperVGA, 256-color modes
# +	tvga16	Trident SuperVGA in 800x600, 16-color mode (256K memory)
#   ****** NOTE: The vesa device does not work with the Watcom (32-bit MS-DOS)
#   ****** compiler or executable.
#	vesa	SuperVGA with VESA standard API driver
#   MS-DOS other:
#	bgi	Borland Graphics Interface (CGA)  [MS-DOS only]
# *	herc	Hercules Graphics display   [MS-DOS only]
#	mswin	Microsoft Windows 3.0, 3.1  [MS Windows only]
#	mswinprn  Microsoft Windows 3.0, 3.1 printer  [MS Windows only]
# *	pe	Private Eye display
#   Unix and VMS:
#   ****** NOTE: For direct frame buffer addressing under SCO Unix or Xenix,
#   ****** edit the definition of EGAVGA below.
# *	att3b1	AT&T 3b1/Unixpc monochrome display   [3b1 only]
# *	sonyfb	Sony Microsystems monochrome display   [Sony only]
# *	sunview  SunView window system   [SunOS only]
#	x11	X Windows version 11, release >=4   [Unix and VMS only]
# Printers:
# *	appledmp  Apple Dot Matrix Printer (should also work with Imagewriter)
#	bj10e	Canon BubbleJet BJ10e
# *	bj200	Canon BubbleJet BJ200
# *	cdeskjet  H-P DeskJet 500C with 1 bit/pixel color
# *	cdjcolor  H-P DeskJet 500C with 24 bit/pixel color and
#		high-quality color (Floyd-Steinberg) dithering
# *	cdjmono  H-P DeskJet 500C printing black only
# *	cdj500	H-P DeskJet 500C (same as cdjcolor)
# *	cdj550	H-P DeskJet 550C
# *	declj250  alternate DEC LJ250 driver
# +	deskjet  H-P DeskJet and DeskJet Plus
# *	dfaxhigh  DigiBoard, Inc.'s DigiFAX software format (high resolution)
# *	dfaxlow  DigiFAX low (normal) resolution
#	djet500  H-P DeskJet 500
# *	djet500c  H-P DeskJet 500C
#	epson	Epson-compatible dot matrix printers (9- or 24-pin)
# +	eps9high  Epson-compatible 9-pin, interleaved lines
#		(triple resolution)
# *	epsonc	Epson LQ-2550 and Fujitsu 3400/2400/1200 color printers
# *	escp2	Epson ESC/P 2 language printers, including Stylus 800
# +     ibmpro  IBM 9-pin Proprinter
# *	jetp3852  IBM Jetprinter ink-jet color printer (Model #3852)
# +	laserjet  H-P LaserJet
# *	la50	DEC LA50 printer
# *	la75	DEC LA75 printer
# *	lbp8	Canon LBP-8II laser printer
# *	ln03	DEC LN03 printer
# *	lj250	DEC LJ250 Companion color printer
# +	ljet2p	H-P LaserJet IId/IIp/III* with TIFF compression
# +	ljet3	H-P LaserJet III* with Delta Row compression
# +	ljet4	H-P LaserJet 4 (defaults to 600 dpi)
# +	ljetplus  H-P LaserJet Plus
# *	m8510	C.Itoh M8510 printer
# *	necp6	NEC P6/P6+/P60 printers at 360 x 360 DPI resolution
# *	nwp533  Sony Microsystems NWP533 laser printer   [Sony only]
# *	oki182	Okidata MicroLine 182
#	paintjet  H-P PaintJet color printer
# *	pj	alternate PaintJet XL driver 
# *	pjxl	H-P PaintJet XL color printer
# *	pjxl300  H-P PaintJet XL300 color printer
# *	r4081	Ricoh 4081 laser printer
# *	sparc	SPARCprinter
# *	t4693d2  Tektronix 4693d color printer, 2 bits per R/G/B component
# *	t4693d4  Tektronix 4693d color printer, 4 bits per R/G/B component
# *	t4693d8  Tektronix 4693d color printer, 8 bits per R/G/B component
# *	tek4696  Tektronix 4695/4696 inkjet plotter
# *	trufax	TruFax facsimile driver  [Unix only]
# File formats and others:
#	bit	A plain "bit bucket" device
#	bmpmono	Monochrome MS Windows .BMP file format
#	bmp16	4-bit (EGA/VGA) .BMP file format
#	bmp256	8-bit (256-color) .BMP file format
#	bmp16m	24-bit .BMP file format
#	gifmono	Monochrome GIF file format
#	gif8	8-bit color GIF file format
#	pcxmono	Monochrome PCX file format
#	pcxgray	8-bit gray scale PCX file format
#	pcx16	Older color PCX file format (EGA/VGA, 16-color)
#	pcx256	Newer color PCX file format (256-color)
#	pbm	Portable Bitmap (plain format)
#	pbmraw	Portable Bitmap (raw format)
#	pgm	Portable Graymap (plain format)
#	pgmraw	Portable Graymap (raw format)
#	ppm	Portable Pixmap (plain format)
#	ppmraw	Portable Pixmap (raw format)
# *	tiffg3	TIFF/F (G3 fax)

# User-contributed drivers marked with * require hardware or software
# that is not available to Aladdin Enterprises.  Please contact the
# original contributors, not Aladdin Enterprises, if you have questions.
# Contact information appears in the driver entry below.
#
# Drivers marked with a + are maintained by Aladdin Enterprises with
# the assistance of users, since Aladdin Enterprises doesn't have access to
# the hardware for these either.

# If you add drivers, it would be nice if you kept each list
# in alphabetical order.

# Each platform-specific makefile contains a line of the form
#	DEVICE_DEVS=<dev1>.dev ... <devn>.dev
# where dev1 ... devn are the devices to be included in the build.
# You may edit this line to select any desired set of devices.
# dev1 will be used as the default device (unless overridden from
# the command line with -sDEVICE=xxx, of course.)  If you can't fit all the
# devices on a single line, you may add lines defining
#	DEVICE_DEVS2=<dev21>.dev ... <dev2n>.dev
#	DEVICE_DEVS3=<dev31>.dev ... <dev3n>.dev
# etc. up to DEVICE_DEVS9.
# Don't use continuation lines, since this may break the MS-DOS command
# processor.

# ---------------------------- End of catalog ---------------------------- #

# If you want to add a new device driver, the examples below should be
# enough of a guide to the correct form for the makefile rules.

# All device drivers depend on the following:
GDEV=$(AK) echogs$(XE) $(gserrors_h) $(gx_h) $(gxdevice_h)

# Define the header files for device drivers.  Every header file used by
# more than one device driver must be listed here.
gdevpccm_h=gdevpccm.h
gdevpcfb_h=gdevpcfb.h $(dos__h)
gdevpcl_h=gdevpcl.h
gdevsvga_h=gdevsvga.h
gdevx_h=gdevx.h

###### ------------------- MS-DOS display devices ------------------- ######

# There are really only three drivers: an EGA/VGA driver (4 bit-planes,
# plane-addressed), a SuperVGA driver (8 bit-planes, byte addressed),
# and a special driver for the S3 chip.
# To make A4 paper the default, change the compilation line from
#	$(CCD) ...
# to
#	$(CCD) -DA4

### ----------------------- EGA and VGA displays ----------------------- ###

gdevegaa.$(OBJ): gdevegaa.asm

ETEST=ega.$(OBJ) $(ega_) gdevpcfb.$(OBJ) gdevegaa.$(OBJ)
ega.exe: $(ETEST) libc$(MM).tr
	$(COMPDIR)\tlink $(LCT) $(LO) $(LIBDIR)\c0$(MM) @ega.tr @libc$(MM).tr

ega.$(OBJ): ega.c $(GDEV)
	$(CCC) -v ega.c

# The shared MS-DOS makefile defines PCFBASM as either gdevegaa.$(OBJ)
# or an empty string.

# NOTE: for direct frame buffer addressing under SCO Unix or Xenix,
# change gdevevga to gdevsco in the following line.
EGAVGA=gdevevga.$(OBJ) gdevpcfb.$(OBJ) $(PCFBASM)

gdevevga.$(OBJ): gdevevga.c $(GDEV) $(gdevpcfb_h)
	$(CCD) gdevevga.c

gdevsco.$(OBJ): gdevsco.c $(GDEV) $(gdevpcfb_h)
	$(CCD) gdevsco.c

# Common code for MS-DOS and SCO.
gdevpcfb.$(OBJ): gdevpcfb.c $(GDEV) $(MAKEFILE) $(gdevpcfb_h)
	$(CCD) -DUSE_ASM=0$(USE_ASM) gdevpcfb.c

# The EGA/VGA family includes: EGA, VGA, and
# the ATI Wonder, Tseng ET3000/4000, and Trident SuperVGA in 16-color mode.

ega.dev: $(EGAVGA)
	$(SHP)gssetdev ega $(EGAVGA)

vga.dev: $(EGAVGA)
	$(SHP)gssetdev vga $(EGAVGA)

atiw16.dev: $(EGAVGA)
	$(SHP)gssetdev atiw16 $(EGAVGA)

tseng16.dev: $(EGAVGA)
	$(SHP)gssetdev tseng16 $(EGAVGA)

tvga16.dev: $(EGAVGA)
	$(SHP)gssetdev tvga16 $(EGAVGA)

### ------------------------- SuperVGA displays ------------------------ ###

SVGA=gdevsvga.$(OBJ) $(PCFBASM)

gdevsvga.$(OBJ): gdevsvga.c $(GDEV) $(MAKEFILE) \
  $(gdevpcfb_h) $(gdevsvga_h)
	$(CCD) -DUSE_ASM=0$(USE_ASM) gdevsvga.c

# The SuperVGA family includes: ATI Wonder, S3, Trident, Tseng ET3000/4000,
# and VESA.

atiw.dev: $(SVGA)
	$(SHP)gssetdev atiw $(SVGA)

tseng.dev: $(SVGA)
	$(SHP)gssetdev tseng $(SVGA)

tvga.dev: $(SVGA)
	$(SHP)gssetdev tvga $(SVGA)

vesa.dev: $(SVGA)
	$(SHP)gssetdev vesa $(SVGA)

# The S3 driver doesn't share much code with the others.

s3vga_=$(SVGA) gdevs3ga.$(OBJ)
s3vga.dev: $(s3vga_)
	$(SHP)gssetdev s3vga $(s3vga_)

gdevs3ga.$(OBJ): gdevs3ga.c $(GDEV) $(MAKEFILE) $(gdevpcfb_h) $(gdevsvga_h)
	$(CCD) gdevs3ga.c

### ------------ The BGI (Borland Graphics Interface) device ----------- ###

cgaf.$(OBJ): $(BGIDIR)\cga.bgi
	$(BGIDIR)\bgiobj /F $(BGIDIR)\cga

egavgaf.$(OBJ): $(BGIDIR)\egavga.bgi
	$(BGIDIR)\bgiobj /F $(BGIDIR)\egavga

# Include egavgaf.$(OBJ) for debugging only.
bgi_=gdevbgi.$(OBJ) cgaf.$(OBJ)
bgi.dev: $(bgi_)
	$(SHP)gssetdev bgi $(bgi_)
	$(SHP)gsaddmod bgi -lib $(LIBDIR)\graphics

gdevbgi.$(OBJ): gdevbgi.c $(GDEV) $(MAKEFILE) $(gxxfont_h)
	$(CCC) -DBGI_LIB=$(QQ)$(BGIDIRSTR)$(QQ) gdevbgi.c

### ------------------- The Hercules Graphics display ------------------- ###

herc_=gdevherc.$(OBJ)
herc.dev: $(herc_)
	$(SHP)gssetdev herc $(herc_)

gdevherc.$(OBJ): gdevherc.c $(GDEV)
	$(CCC) gdevherc.c

###### ------------------- The Private Eye display ------------------- ######
### Note: this driver was contributed by a user:                          ###
###   please contact narf@media-lab.media.mit.edu if you have questions.  ###

pe_=gdevpe.$(OBJ)
pe.dev: $(pe_)
	$(SHP)gssetdev pe $(pe_)

gdevpe.$(OBJ): gdevpe.c $(GDEV)

###### ----------------- The MS-Windows 3.n display ------------------ ######

gdevmswn_h=gdevmswn.h $(GDEV) gp_mswin.h

# Choose one of gdevwddb or gdevwdib here.
mswin_=gdevmswn.$(OBJ) gdevmsxf.$(OBJ) gdevwdib.$(OBJ) \
  gdevemap.$(OBJ) gdevpccm.$(OBJ)
mswin.dev: $(mswin_)
	$(SHP)gssetdev mswin $(mswin_)

gdevmswn.$(OBJ): gdevmswn.c $(gdevmswn_h) $(gp_h) $(gpcheck_h) \
  $(gsprops_h) $(gdevpccm_h)

gdevmsxf.$(OBJ): gdevmsxf.c $(ctype__h) $(math__h) $(memory__h) \
  $(gdevmswn_h) $(gsutil_h) $(gxxfont_h)

# An implementation using a device-dependent bitmap.
gdevwddb.$(OBJ): gdevwddb.c $(gdevmswn_h)

# An implementation using a DIB filled by an image device.
gdevwdib.$(OBJ): gdevwdib.c $(dos__h) $(gdevmswn_h)

###### ----------------- The MS-Windows 3.n printer ------------------ ######

mswinprn_=gdevwprn.$(OBJ) gdevmsxf.$(OBJ)
mswinprn.dev: $(mswinprn_)
	$(SHP)gssetdev mswinprn $(mswinprn_)

gdevwprn.$(OBJ): gdevwprn.c $(gdevmswn_h) $(gp_h) $(gpcheck_h) \
  $(gsprops_h) $(gdevpccm_h)

###### ----------- The AT&T 3b1 Unixpc monochrome display ------------ ######
### Note: this driver was contributed by a user: please contact           ###
###       Andy Fyfe (andy@cs.caltech.edu) if you have questions.          ###

att3b1_=gdev3b1.$(OBJ)
att3b1.dev: $(att3b1_)
	$(SHP)gssetdev att3b1 $(att3b1_)

gdev3b1.$(OBJ): gdev3b1.c

###### --------------- Memory-buffered printer devices --------------- ######

# The dependency list for printers includes devs.mak because
# you can specify -DA4 to make A4 paper the default.
# See below under, e.g., gdevdjet.c.
PDEVH=$(GDEV) $(gdevprn_h) devs.mak

gdevprn.$(OBJ): gdevprn.c $(PDEVH) $(gp_h) $(gsprops_h)

### ------------------- The Apple DMP printer device ------------------- ###

appledmp_=gdevadmp.$(OBJ) gdevprn.$(OBJ)

appledmp.dev: $(appledmp_)
	$(SHP)gssetdev appledmp $(appledmp_)

### ------------ The Canon BubbleJet BJ10e and BJ200 devices ------------ ###

bj10e_=gdevbj10.$(OBJ) gdevprn.$(OBJ)

bj10e.dev: $(bj10e_)
	$(SHP)gssetdev bj10e $(bj10e_)

bj200.dev: $(bj10e_)
	$(SHP)gssetdev bj200 $(bj10e_)

gdevbj10.$(OBJ): gdevbj10.c $(PDEVH)

### -------------------------- The DigiFAX device ----------------------- ###
###    This driver outputs images in a format suitable for use with       ###
###    DigiBoard, Inc.'s DigiFAX software.  Use -sDEVICE=dfaxhigh for     ###
###    high resolution output, -sDEVICE=dfaxlow for normal output.        ###
### Note: this driver was contributed by a user: please contact           ###
###       Rick Richardson (rick@digibd.com) if you have questions.        ###

digifax_=gdevdfax.$(OBJ) gdevprn.$(OBJ)
dfaxhigh.dev: $(digifax_)
	$(SHP)gssetdev dfaxhigh $(digifax_)

dfaxlow.dev: $(digifax_)
	$(SHP)gssetdev dfaxlow $(digifax_)

gdevdfax.$(OBJ): gdevdfax.c $(GDEV) $(gdevprn_h) gdevdfg3.h

### ----------- The H-P DeskJet and LaserJet printer devices ----------- ###

### These are essentially the same device.
### You can make A4 paper the default: see below.
### NOTE: printing at full resolution (300 DPI) requires a printer
###   with at least 1.5 Mb of memory.  150 DPI only requires .5 Mb.

HPPCL=gdevprn.$(OBJ) gdevpcl.$(OBJ)
HPMONO=gdevdjet.$(OBJ) $(HPPCL)

gdevpcl.$(OBJ): gdevpcl.c $(PDEVH) $(gdevpcl_h)

# To make A4 paper the default, change the second line below this to
#	$(CCC) -DA4 gdevdjet.c
gdevdjet.$(OBJ): gdevdjet.c $(PDEVH) $(gdevpcl_h)
	$(CCC) gdevdjet.c

deskjet.dev: $(HPMONO)
	$(SHP)gssetdev deskjet $(HPMONO)

djet500.dev: $(HPMONO)
	$(SHP)gssetdev djet500 $(HPMONO)

laserjet.dev: $(HPMONO)
	$(SHP)gssetdev laserjet $(HPMONO)

ljetplus.dev: $(HPMONO)
	$(SHP)gssetdev ljetplus $(HPMONO)

### Selecting ljet2p provides TIFF (mode 2) compression on LaserJet III,
### IIIp, IIId, IIIsi, IId, and IIp. 

ljet2p.dev: $(HPMONO)
	$(SHP)gssetdev ljet2p $(HPMONO)

### Selecting ljet3 provides Delta Row (mode 3) compression on LaserJet III,
### IIIp, IIId, IIIsi.

ljet3.dev: $(HPMONO)
	$(SHP)gssetdev ljet3 $(HPMONO)

### Selecting ljet4 also provides Delta Row compression on LaserJet IV series.

ljet4.dev: $(HPMONO)
	$(SHP)gssetdev ljet4 $(HPMONO)

###- The H-P DeskJet 500C/550C and PaintJet family color printer devices -###
### Note: there are two different 500C drivers, both contributed by users.###
###   If you have questions about the djet500c driver,                    ###
###       please contact AKayser@et.tudelft.nl.                           ###
###   If you have questions about the cdj* drivers,                       ###
###       please contact g.cameron@biomed.abdn.ac.uk.                     ###

cdeskjet_=gdevcdj.$(OBJ) $(HPPCL)

cdeskjet.dev: $(cdeskjet_)
	$(SHP)gssetdev cdeskjet $(cdeskjet_)

cdjcolor.dev: $(cdeskjet_)
	$(SHP)gssetdev cdjcolor $(cdeskjet_)

cdjmono.dev: $(cdeskjet_)
	$(SHP)gssetdev cdjmono $(cdeskjet_)

cdj500.dev: $(cdeskjet_)
	$(SHP)gssetdev cdj500 $(cdeskjet_)

cdj550.dev: $(cdeskjet_)
	$(SHP)gssetdev cdj550 $(cdeskjet_)

declj250.dev: $(cdeskjet_)
	$(SHP)gssetdev declj250 $(cdeskjet_)

pj.dev: $(cdeskjet_)
	$(SHP)gssetdev pj $(cdeskjet_)

pjxl.dev: $(cdeskjet_)
	$(SHP)gssetdev pjxl $(cdeskjet_)

pjxl300.dev: $(cdeskjet_)
	$(SHP)gssetdev pjxl300 $(cdeskjet_)

# NB: you can also customise the build if required, using -DA4 (for A4 paper)
# and -DBitsPerPixel=<number> if you wish the default to be other than 24
# for the generic drivers (cdj500, cdj550, pjxl300, pjtest, pjxltest).
# E.g,. to make A4 paper the default, change the second line below this to
#	$(CCC) -DA4 gdevdjet.c
gdevcdj.$(OBJ): gdevcdj.c $(PDEVH) $(gdevpcl_h)
	$(CCC) gdevcdj.c

djet500c_=gdevdjtc.$(OBJ) $(HPPCL)
djet500c.dev: $(djet500c_)
	$(SHP)gssetdev djet500c $(djet500c_)

gdevdjtc.$(OBJ): gdevdjtc.c $(PDEVH) $(gdevpcl_h)

### ----------------- The generic Epson printer device ----------------- ###

epson_=gdevepsn.$(OBJ) gdevprn.$(OBJ)

epson.dev: $(epson_)
	$(SHP)gssetdev epson $(epson_)

eps9high.dev: $(epson_)
	$(SHP)gssetdev eps9high $(epson_)

gdevepsn.$(OBJ): gdevepsn.c $(PDEVH)

### ----------------- The IBM Proprinter printer device ---------------- ###

ibmpro.dev: $(epson_)
	$(SHP)gssetdev ibmpro $(epson_)

### -------------- The Epson LQ-2550 color printer device -------------- ###
### Note: this driver was contributed by users: please contact           ###
###       Dave St. Clair (dave@exlog.com) if you have questions.         ###

epsonc_=gdevepsc.$(OBJ) gdevprn.$(OBJ)
epsonc.dev: $(epsonc_)
	$(SHP)gssetdev epsonc $(epsonc_)

gdevepsc.$(OBJ): gdevepsc.c $(PDEVH)

### -------------- The Epson ESC/P 2 language printer device ----------- ###
### Note: this driver was contributed by a user: if you have questions,  ###
###       please contact Richard Brown (rab@tauon.ph.unimelb.edu.au).    ###

escp2_=gdevescp.$(OBJ) gdevprn.$(OBJ)
escp2.dev: $(escp2_)
	$(SHP)gssetdev escp2 $(escp2_)

gdevescp.$(OBJ): gdevescp.c $(PDEVH)

### ------------ The H-P PaintJet color printer device ----------------- ###
### Note: this driver also supports the DEC LJ250 color printer, which   ###
###       has a PaintJet-compatible mode, and the PaintJet XL.           ###
### If you have questions about the XL, please contact Rob Reiss         ###
###       (rob@moray.berkeley.edu).                                      ###

PJET=gdevpjet.$(OBJ) $(HPPCL)

gdevpjet.$(OBJ): gdevpjet.c $(PDEVH) $(gdevpcl_h)

lj250.dev: $(PJET)
	$(SHP)gssetdev lj250 $(PJET)

paintjet.dev: $(PJET)
	$(SHP)gssetdev paintjet $(PJET)

pjetxl.dev: $(PJET)
	$(SHP)gssetdev pjetxl $(PJET)

### ------- The IBM 3852 JetPrinter color inkjet printer device -------- ###
### Note: this driver was contributed by users: please contact           ###
###       Kevin Gift (kgift@draper.com) if you have questions.           ###
### Note that the paper size that can be addressed by the graphics mode  ###
###   used in this driver is fixed at 7-1/2 inches wide (the printable   ###
###   width of the jetprinter itself.)                                   ###

jetp3852_=gdev3852.$(OBJ) gdevprn.$(OBJ)
jetp3852.dev: $(jetp3852_)
	$(SHP)gssetdev jetp3852 $(jetp3852_)

gdevjetp.$(OBJ): gdevjetp.c $(PDEVH) $(gdevpcl_h)

### ----------------- The Canon LBP-8II printer device ----------------- ###
### Note: this driver was contributed by users: please contact           ###
###       Tom Quinn (trq@prg.oxford.ac.uk) if you have questions.        ###
### Note that the standard paper size for this driver is the European    ###
###   A4 size, not the American 8.5" x 11" size.                         ###

lbp8_=gdevlbp8.$(OBJ) gdevprn.$(OBJ)
lbp8.dev: $(lbp8_)
	$(SHP)gssetdev lbp8 $(lbp8_)

gdevlbp8.$(OBJ): gdevlbp8.c $(PDEVH)

### -------------- The DEC LN03/LA50/LA75 printer devices -------------- ###
### Note: this driver was contributed by users: please contact           ###
###       Ulrich Mueller (ulm@vsnhd1.cern.ch) if you have questions.     ###
### For questions about LA50 and LA75: please contact                    ###
###       Ian MacPhedran (macphed@dvinci.USask.CA).                     ###
### For the LN03, you can make A4 paper the default: see below.          ###

ln03_=gdevln03.$(OBJ) gdevprn.$(OBJ)
ln03.dev: $(ln03_)
	$(SHP)gssetdev ln03 $(ln03_)

la50.dev: $(ln03_)
	$(SHP)gssetdev la50 $(ln03_)

la75.dev: $(ln03_)
	$(SHP)gssetdev la75 $(ln03_)

# To make A4 paper the default, change the second line below this to
#	$(CCC) -DA4 gdevln03.c
gdevln03.$(OBJ): gdevln03.c $(PDEVH)
	$(CCC) gdevln03.c

### -------------- The C.Itoh M8510 printer device --------------------- ###
### Note: this driver was contributed by a user: please contact Bob      ###
###       Smith <bob@snuffy.penfield.ny.us> if you have questions.       ###

m8510_=gdev8510.$(OBJ) gdevprn.$(OBJ)
m8510.dev: $(m8510_)
	$(SHP)gssetdev m8510 $(m8510_)

gdev8510.$(OBJ): gdev8510.c $(PDEVH)

### --------------------- The NEC P6 family devices -------------------- ###

necp6_=gdevnp6.$(OBJ) gdevprn.$(OBJ)
necp6.dev: $(necp6_)
	$(SHP)gssetdev necp6 $(necp6_)

gdevnp6.$(OBJ): gdevnp6.c $(PDEVH)

### ----------------- The Okidata MicroLine 182 device ----------------- ###
### Note: this driver was contributed by a user: please contact          ###
###       Maarten Koning (smeg@bnr.ca) if you have questions.            ###

oki182_=gdevo182.$(OBJ) gdevprn.$(OBJ)
oki182.dev: $(oki182_)
	$(SHP)gssetdev oki182 $(oki182_)

gdevo182.$(OBJ): gdevo182.c $(PDEVH)

### ------------- The Ricoh 4081 laser printer device ------------------ ###
### Note: this driver was contributed by users:                          ###
###       please contact kdw@oasis.icl.co.uk if you have questions.      ###

r4081_=gdev4081.$(OBJ) gdevprn.$(OBJ)
r4081.dev: $(r4081_)
	$(SHP)gssetdev r4081 $(r4081_)

gdev4081.$(OBJ): gdev4081.c $(PDEVH)

###### ------------------------ Sony devices ------------------------ ######
### Note: these drivers were contributed by users: please contact        ###
###       Mike Smolenski (mike@intertech.com) if you have questions.     ###

### ------------------- Sony NeWS frame buffer device ------------------ ###

sonyfb_=gdevsnfb.$(OBJ) gdevprn.$(OBJ)
sonyfb.dev: $(sonyfb_)
	$(SHP)gssetdev sonyfb $(sonyfb_)

gdevsnfb.$(OBJ): gdevsnfb.c $(PDEVH)

### -------------------- Sony NWP533 printer device -------------------- ###
### Note: this driver was contributed by a user: please contact Tero     ###
###       Kivinen (kivinen@joker.cs.hut.fi) if you have questions.       ###

nwp533_=gdevn533.$(OBJ) gdevprn.$(OBJ)
nwp533.dev: $(nwp533_)
	$(SHP)gssetdev nwp533 $(nwp533_)

gdevn533.$(OBJ): gdevn533.c $(PDEVH)

### ------------------------- The SPARCprinter ------------------------- ###
### Note: this driver was contributed by users: please contact Martin    ###
###       Schulte (schulte@thp.uni-koeln.de) if you have questions.      ###
###       He would also like to hear from anyone using the driver.       ###
### Please consult the source code for additional documentation.         ###

sparc_=gdevsppr.$(OBJ) gdevprn.$(OBJ)
sparc.dev: $(sparc_)
	$(SHP)gssetdev sparc $(sparc_)

gdevsppr.$(OBJ): gdevsppr.c $(PDEVH)

###### --------------------- The SunView device --------------------- ######
### Note: this driver is maintained by a user: if you have questions,    ###
###       please contact Andreas Stolcke (stolcke@icsi.berkeley.edu).    ###

sunview_=gdevsun.$(OBJ)
sunview.dev: $(sunview_)
	$(SHP)gssetdev sunview $(sunview_)
	$(SHP)gsaddmod sunview -lib suntool sunwindow pixrect

gdevsun.$(OBJ): gdevsun.c $(GDEV) $(arch_h)

### ----------------- Tektronix 4396d color printer -------------------- ###
### Note: this driver was contributed by a user: please contact          ###
###       Karl Hakimian (hakimian@haney.eecs.wsu.edu)                    ###
###       if you have questions.                                         ###

t4693d_=gdev4693.$(OBJ) gdevprn.$(OBJ)
t4693d2.dev: $(t4693d_)
	$(SHP)gssetdev t4693d2 $(t4693d_)

t4693d4.dev: $(t4693d_)
	$(SHP)gssetdev t4693d4 $(t4693d_)

t4693d8.dev: $(t4693d_)
	$(SHP)gssetdev t4693d8 $(t4693d_)

gdev4693.$(OBJ): gdev4693.c $(GDEV)

### -------------------- Tektronix ink-jet printers -------------------- ###
### Note: this driver was contributed by a user: please contact          ###
###       Karsten Spang (spang@nbivax.nbi.dk) if you have questions.     ###

tek4696_=gdevtknk.$(OBJ) gdevprn.$(OBJ)
tek4696.dev: $(tek4696_)
	$(SHP)gssetdev tek4696 $(tek4696_)

gdevtknk.$(OBJ): gdevtknk.c $(PDEVH)

### ----------------- The TruFax facsimile device ---------------------- ###
### Note: this driver was contributed by users: please contact           ###
###       Neil Ostroff (nao@maestro.bellcore.com) if you have questions. ###
### Note that the driver requires a file encode_l.o supplied by the      ###
###   makers of the TruFax product.                                      ###

trufax_=gdevtrfx.$(OBJ) gdevprn.$(OBJ) encode_l.$(OBJ)
trufax.dev: $(trufax_)
	$(SHP)gssetdev trufax $(trufax_)

gdevtrfx.$(OBJ): gdevtrfx.c $(GDEV)

###### ----------------------- The X11 device ----------------------- ######

# Aladdin Enterprises does not support Ghostview.  For more information
# about Ghostview, please contact Tim Theisen (ghostview@cs.wisc.edu).

x11_=gdevx.$(OBJ) gdevxini.$(OBJ) gdevxxf.$(OBJ) gdevemap.$(OBJ)
x11.dev: $(x11_)
	$(SHP)gssetdev x11 $(x11_)
	$(SHP)gsaddmod x11 -lib Xt X11 Xext

# See the main makefile for the definition of XINCLUDE.
GDEVX=$(GDEV) x_.h gdevx.h $(MAKEFILE)
gdevx.$(OBJ): gdevx.c $(GDEVX) $(gsprops_h) $(gsutil_h)
	$(CCC) $(XINCLUDE) gdevx.c

gdevxini.$(OBJ): gdevxini.c $(GDEVX) $(ctype__h)
	$(CCC) $(XINCLUDE) gdevxini.c

gdevxxf.$(OBJ): gdevxxf.c $(GDEVX) $(gsutil_h) $(gxxfont_h)
	$(CCC) $(XINCLUDE) gdevxxf.c

### ---------------------- The bit bucket device ----------------------- ###

bit_=gdevbit.$(OBJ) gdevprn.$(OBJ)
bit.dev: $(bit_)
	$(SHP)gssetdev bit $(bit_)

gdevbit.$(OBJ): gdevbit.c $(PDEVH)

###### ----------------------- PC file formats ---------------------- ######

gdevpccm.$(OBJ): gdevpccm.c $(AK) \
  $(gs_h) $(gsmatrix_h) $(gxdevice_h) $(gdevpccm_h)

### ------------------------- .BMP file formats ------------------------- ###

bmp_=gdevbmp.$(OBJ) gdevpccm.$(OBJ) gdevprn.$(OBJ)

gdevbmp.$(OBJ): gdevbmp.c $(PDEVH) $(gdevpccm_h)

bmpmono.dev: $(bmp_)
	$(SHP)gssetdev bmpmono $(bmp_)

bmp16.dev: $(bmp_)
	$(SHP)gssetdev bmp16 $(bmp_)

bmp256.dev: $(bmp_)
	$(SHP)gssetdev bmp256 $(bmp_)

bmp16m.dev: $(bmp_)
	$(SHP)gssetdev bmp16m $(bmp_)

### ------------------------- GIF file formats ------------------------- ###

GIF=gdevgif.$(OBJ) gdevpccm.$(OBJ) gdevprn.$(OBJ)

gdevgif.$(OBJ): gdevgif.c $(PDEVH) $(gdevpccm_h)

gifmono.dev: $(GIF)
	$(SHP)gssetdev gifmono $(GIF)

gif8.dev: $(GIF)
	$(SHP)gssetdev gif8 $(GIF)

### ------------------------- PCX file formats ------------------------- ###

pcx_=gdevpcx.$(OBJ) gdevpccm.$(OBJ) gdevprn.$(OBJ)

gdevpcx.$(OBJ): gdevpcx.c $(PDEVH) $(gdevpccm_h) $(gxlum_h)

pcxmono.dev: $(pcx_)
	$(SHP)gssetdev pcxmono $(pcx_)

pcxgray.dev: $(pcx_)
	$(SHP)gssetdev pcxgray $(pcx_)

pcx16.dev: $(pcx_)
	$(SHP)gssetdev pcx16 $(pcx_)

pcx256.dev: $(pcx_)
	$(SHP)gssetdev pcx256 $(pcx_)

###### ------------------- Portable Bitmap devices ------------------ ######
### For more information, see the pbm(5), pgm(5), and ppm(5) man pages.  ###

pxm_=gdevpbm.$(OBJ) gdevprn.$(OBJ)

gdevpbm.$(OBJ): gdevpbm.c $(PDEVH) $(gxlum_h)

### Portable Bitmap (PBM, plain or raw format, magic numbers "P1" or "P4")

pbm.dev: $(pxm_)
	$(SHP)gssetdev pbm $(pxm_)

pbmraw.dev: $(pxm_)
	$(SHP)gssetdev pbmraw $(pxm_)

### Portable Graymap (PGM, plain or raw format, magic numbers "P2" or "P5")

pgm.dev: $(pxm_)
	$(SHP)gssetdev pgm $(pxm_)

pgmraw.dev: $(pxm_)
	$(SHP)gssetdev pgmraw $(pxm_)

### Portable Pixmap (PPM, plain or raw format, magic numbers "P3" or "P6")

ppm.dev: $(pxm_)
	$(SHP)gssetdev ppm $(pxm_)

ppmraw.dev: $(pxm_)
	$(SHP)gssetdev ppmraw $(pxm_)

### -------------------------- TIFF/F device ---------------------------- ###
###    This driver outputs images in a TIFF format 			  ###
###    Use -sDEVICE=tiffg3 and						  ###
###	  -r204x98 for low resolution output, or			  ###
###	  -r204x196 for high resolution output				  ###
###    Note also that 3 page sizes are understood: letter, A4, and B4	  ###
### Note: this driver was contributed by a user: please contact           ###
###       Sam Leffler (sam@sgi.com) if you have questions.        	  ###

tiffg3_=gdevtiff.$(OBJ) gdevprn.$(OBJ)
tiffg3.dev: $(tiffg3_)
	$(SHP)gssetdev tiffg3 $(tiffg3_)
gdevtiff.$(OBJ): gdevtiff.c $(GDEV) $(gdevprn_h) gdevdfg3.h gdevtiff.h

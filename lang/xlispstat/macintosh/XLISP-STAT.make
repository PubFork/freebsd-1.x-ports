#############################################################
## Uncomment one of the following two groups for a generic
## application or one compiled to use 68020/68881 intructions.
##
## Generic application
##
#CC = C -d MPWC -b2 -mbg off -sym off
#LIBS = {ALIBS}
#LFLAGS = -w -t APPL -c '????' 
##
## MC68020/MC68881 application
##
CC = C -d MPWC -b2 -mbg off -sym off -mc68881 -elems881 -mc68020
LIBS = {ALIBS881}
LFLAGS = -w -t APPL -c '????' 
#############################################################

#############################################################
##
## Change this if you want to mess with this file without
## having everything recompiled
##
MAKEFILE = XLISP-STAT.make
#MAKEFILE =
#############################################################

#############################################################
#############################################################
##                                                         ##
##                DO NOT EDIT BELOW THIS LINE              ##
##                                                         ##
#############################################################
#############################################################

##
## Should reorder objects to correspond to segment order
##

GRAPHOBJECTS = �
		edit.c.o �
		editwindows.c.o �
		FakeAlert.c.o �
		macdialogs1.c.o �
		macdialogs2.c.o �
		maciviewwindow.c.o �
		maciviewwindow2.c.o �
		maciviewwindow3.c.o �
		macmenus.c.o �
		macresizebrush.c.o �
		macstuff.c.o �
		macwindows.c.o �
		macxsgraph.c.o �
		TransEdit1.c.o �
		TransSkel1.c.o
		
OBJECTS1 = �
		basics.c.o �
		betabase.c.o �
		bivnor.c.o �
		cfft.c.o �
		cholesky.c.o �
		common.c.o �
		commonarrays.c.o �
		commonmath.c.o �
		complex.c.o �
		compound.c.o �
		ddistribs.c.o �
		derivatives.c.o �
		dialogs.c.o �
		distribs.c.o �
		functions.c.o �
		gamln.c.o �
		gammabase.c.o �
		hardwareobs.c.o
		
OBJECTS2 = �
		iview.c.o �
		iviewdata.c.o �
		iviewintrn.c.o �
		iviewscale.c.o �
		kernel.c.o �
		linalg.c.o �
		lowess.c.o �
		ludecomp.c.o �
		makerotation.c.o �
		math.c.o �
		matrices1.c.o �
		matrices2.c.o �
		menus.c.o �
		minimize.c.o
OBJECTS3 = �
		nor.c.o �
		objectinit.c.o �
		objects.c.o �
		optimize.c.o �
		ppnd.c.o �
		qrdecomp.c.o �
		rcondest.c.o �
		sortdata.c.o �
		splines.c.o �
		statfloat.c.o �
		statinit.c.o �
		statistics.c.o �
		stmem.c.o �
		studentbase.c.o �
		svdecomp.c.o �
		uni.c.o �
		utilities.c.o �
		utilities2.c.o �
		windows.c.o
OBJECTS4 = �
		xlbfun.c.o �
		xlcont.c.o �
		xldbug.c.o �
		xldmem.c.o �
		xleval.c.o �
		xlfio.c.o �
		xlftab.c.o �
		xlglob.c.o �
		xlimage.c.o �
		xlinit.c.o �
		xlio.c.o �
		xlisp.c.o �
		xljump.c.o �
		xllist.c.o �
		xlpp.c.o �
		xlprin.c.o �
		xlread.c.o �
		xlstr.c.o �
		xlstruct.c.o �
		xlsubr.c.o �
		xlsym.c.o �
		xlsys.c.o
OBJECTS5 = �
		macdynload.c.o �
		xsbayes.c.o �
		xsgraphics.c.o �
		xshistogram.c.o �
		xsiview.c.o �
		xsiview2.c.o �
		xsiview3.c.o �
		xsiviewintrn.c.o �
		xsiviewwin.c.o �
		xsiviewwin2.c.o �
		xsnamelist.c.o �
		xsnewplots.c.o �
		xsscatmat.c.o �
		xsscatterpl.c.o �
		xsspin.c.o


edit.c.o � {MAKEFILE} edit.c
	 {CC}  -s XLSCSEG1 edit.c
editwindows.c.o � {MAKEFILE} editwindows.c
	 {CC}  -s XLSCSEG1 editwindows.c
FakeAlert.c.o � {MAKEFILE} FakeAlert.c
	 {CC}  -s XLSCSEG1 FakeAlert.c
macstuff.c.o � {MAKEFILE} macstuff.c
	 {CC}  -s XLSCSEG1 macstuff.c
macwindows.c.o � {MAKEFILE} macwindows.c
	 {CC}  -s XLSCSEG1 macwindows.c
macxsgraph.c.o � {MAKEFILE} macxsgraph.c
	 {CC}  -s XLSCSEG1 macxsgraph.c
statfloat.c.o � {MAKEFILE} statfloat.c
	 {CC}  -s XLSCSEG1 statfloat.c
TransEdit1.c.o � {MAKEFILE} TransEdit1.c
	 {CC}  -s XLSCSEG1 TransEdit1.c
TransSkel1.c.o � {MAKEFILE} TransSkel1.c
	 {CC}  -s XLSCSEG1 TransSkel1.c
windows.c.o � {MAKEFILE} windows.c
	 {CC}  -s XLSCSEG1 windows.c

xlbfun.c.o � {MAKEFILE} xlbfun.c
	 {CC}  -s XLSCSEG2 xlbfun.c
xlcont.c.o � {MAKEFILE} xlcont.c
	 {CC}  -s XLSCSEG2 xlcont.c
xldbug.c.o � {MAKEFILE} xldbug.c
	 {CC}  -s XLSCSEG2 xldbug.c
xldmem.c.o � {MAKEFILE} xldmem.c
	 {CC}  -s XLSCSEG2 xldmem.c
xleval.c.o � {MAKEFILE} xleval.c
	 {CC}  -s XLSCSEG2 xleval.c
xlfio.c.o � {MAKEFILE} xlfio.c
	 {CC}  -s XLSCSEG2 xlfio.c
xlftab.c.o � {MAKEFILE} xlftab.c osdefs.h osptrs.h
	 {CC}  -s XLSCSEG2 xlftab.c
xlglob.c.o � {MAKEFILE} xlglob.c
	 {CC}  -s XLSCSEG2 xlglob.c
xlimage.c.o � {MAKEFILE} xlimage.c
	 {CC}  -s XLSCSEG2 xlimage.c
xlio.c.o � {MAKEFILE} xlio.c
	 {CC}  -s XLSCSEG2 xlio.c

xlisp.c.o � {MAKEFILE} xlisp.c
	 {CC}  -s XLSCSEG3 xlisp.c
xljump.c.o � {MAKEFILE} xljump.c
	 {CC}  -s XLSCSEG3 xljump.c
xllist.c.o � {MAKEFILE} xllist.c
	 {CC}  -s XLSCSEG3 xllist.c
xlpp.c.o � {MAKEFILE} xlpp.c
	 {CC}  -s XLSCSEG3 xlpp.c
xlprin.c.o � {MAKEFILE} xlprin.c
	 {CC}  -s XLSCSEG3 xlprin.c
xlread.c.o � {MAKEFILE} xlread.c
	 {CC}  -s XLSCSEG3 xlread.c
xlstr.c.o � {MAKEFILE} xlstr.c
	 {CC}  -s XLSCSEG3 xlstr.c
xlstruct.c.o � {MAKEFILE} xlstruct.c
	 {CC}  -s XLSCSEG3 xlstruct.c
xlsubr.c.o � {MAKEFILE} xlsubr.c
	 {CC}  -s XLSCSEG3 xlsubr.c
xlsym.c.o � {MAKEFILE} xlsym.c
	 {CC}  -s XLSCSEG3 xlsym.c
xlsys.c.o � {MAKEFILE} xlsys.c
	 {CC}  -s XLSCSEG3 xlsys.c

basics.c.o � {MAKEFILE} basics.c
	 {CC}  -s XLSCSEG6 basics.c -o basics.c.o
	 
common.c.o � {MAKEFILE} common.c
	 {CC}  -s XLSCSEG7 common.c
commonmath.c.o � {MAKEFILE} commonmath.c
	 {CC}  -s XLSCSEG7 commonmath.c
	 
commonarrays.c.o � {MAKEFILE} commonarrays.c
	 {CC}  -s XLSCSEG8 commonarrays.c
complex.c.o � {MAKEFILE} complex.c
	 {CC}  -s XLSCSEG8 complex.c
compound.c.o � {MAKEFILE} compound.c
	 {CC}  -s XLSCSEG8 compound.c
hardwareobs.c.o � {MAKEFILE} hardwareobs.c
	 {CC}  -s XLSCSEG8 hardwareobs.c
math.c.o � {MAKEFILE} math.c
	 {CC}  -s XLSCSEG8 math.c
sortdata.c.o � {MAKEFILE} sortdata.c
	 {CC}  -s XLSCSEG8 sortdata.c
statistics.c.o � {MAKEFILE} statistics.c
	 {CC}  -s XLSCSEG8 statistics.c
uni.c.o � {MAKEFILE} uni.c
	 {CC}  -s XLSCSEG8 uni.c

objects.c.o � {MAKEFILE} objects.c
	 {CC}  -s XLSCSEG9 objects.c

maciviewwindow.c.o � {MAKEFILE} maciviewwindow.c
	 {CC}  -s XLSCSEG10 maciviewwindow.c
maciviewwindow2.c.o � {MAKEFILE} maciviewwindow2.c
	 {CC}  -s XLSCSEG10 maciviewwindow2.c
maciviewwindow3.c.o � {MAKEFILE} maciviewwindow3.c
	 {CC}  -s XLSCSEG10 maciviewwindow3.c
xsiview.c.o � {MAKEFILE} xsiview.c
	 {CC}  -s XLSCSEG10 xsiview.c
xsiview2.c.o � {MAKEFILE} xsiview2.c
	 {CC}  -s XLSCSEG10 xsiview2.c

xsiview3.c.o � {MAKEFILE} xsiview3.c
	 {CC}  -s XLSCSEG11 xsiview3.c
xsiviewintrn.c.o � {MAKEFILE} xsiviewintrn.c
	 {CC}  -s XLSCSEG11 xsiviewintrn.c
xsiviewwin.c.o � {MAKEFILE} xsiviewwin.c
	 {CC}  -s XLSCSEG11 xsiviewwin.c
xsiviewwin2.c.o � {MAKEFILE} xsiviewwin2.c
	 {CC}  -s XLSCSEG11 xsiviewwin2.c

iview.c.o � {MAKEFILE} iview.c
	 {CC}  -s XLSCSEG12 iview.c
iviewdata.c.o � {MAKEFILE} iviewdata.c
	 {CC}  -s XLSCSEG12 iviewdata.c
	 
macresizebrush.c.o � {MAKEFILE} macresizebrush.c
	 {CC}  -s XLSCSEG12a macresizebrush.c
iviewscale.c.o � {MAKEFILE} iviewscale.c
	 {CC}  -s XLSCSEG12a iviewscale.c
stmem.c.o � {MAKEFILE} stmem.c
	 {CC}  -s XLSCSEG12a stmem.c

xsgraphics.c.o � {MAKEFILE} xsgraphics.c
	 {CC}  -s XLSCSEG13 xsgraphics.c
xsnewplots.c.o � {MAKEFILE} xsnewplots.c
	 {CC}  -s XLSCSEG13 xsnewplots.c

iviewintrn.c.o � {MAKEFILE} iviewintrn.c
	 {CC}  -s XLSCSEG14 iviewintrn.c

optimize.c.o � {MAKEFILE} optimize.c
	 {CC}  -s XLSCSEG15 optimize.c

objectinit.c.o � {MAKEFILE} objectinit.c
	 {CC}  -s XLSCSEG16 objectinit.c
statinit.c.o � {MAKEFILE} statinit.c
	 {CC}  -s XLSCSEG16 statinit.c

kernel.c.o � {MAKEFILE} kernel.c
	 {CC}  -s XLSCSEG17 kernel.c
linalg.c.o � {MAKEFILE} linalg.c
	 {CC}  -s XLSCSEG17 linalg.c
makerotation.c.o � {MAKEFILE} makerotation.c
	 {CC}  -s XLSCSEG17 makerotation.c
rcondest.c.o � {MAKEFILE} rcondest.c
	 {CC}  -s XLSCSEG17 rcondest.c

ludecomp.c.o � {MAKEFILE} ludecomp.c
	 {CC}  -s XLSCSEG18 ludecomp.c

cholesky.c.o � {MAKEFILE} cholesky.c
	 {CC}  -s XLSCSEG19 cholesky.c

qrdecomp.c.o � {MAKEFILE} qrdecomp.c
	 {CC}  -s XLSCSEG20 qrdecomp.c

svdecomp.c.o � {MAKEFILE} svdecomp.c
	 {CC}  -s XLSCSEG21 svdecomp.c

xshistogram.c.o � {MAKEFILE} xshistogram.c
	 {CC}  -s XLSCSEG22 xshistogram.c

xsnamelist.c.o � {MAKEFILE} xsnamelist.c
	 {CC}  -s XLSCSEG23 xsnamelist.c

xsscatmat.c.o � {MAKEFILE} xsscatmat.c
	 {CC}  -s XLSCSEG24 xsscatmat.c

xsspin.c.o � {MAKEFILE} xsspin.c
	 {CC}  -s XLSCSEG25 xsspin.c

macdialogs1.c.o � {MAKEFILE} macdialogs1.c dialogs.h
	 {CC}  -s XLSCSEG26 macdialogs1.c

macdialogs2.c.o � {MAKEFILE} macdialogs2.c dialogs.h
	 {CC}  -s XLSCSEG27 macdialogs2.c

dialogs.c.o � {MAKEFILE} dialogs.c
	 {CC}  -s XLSCSEG28 dialogs.c

macmenus.c.o � {MAKEFILE} macmenus.c
	 {CC}  -s XLSCSEG29 macmenus.c
menus.c.o � {MAKEFILE} menus.c
	 {CC}  -s XLSCSEG29 menus.c

matrices1.c.o � {MAKEFILE} matrices1.c
	 {CC}  -s XLSCSEG30 matrices1.c

matrices2.c.o � {MAKEFILE} matrices2.c
	 {CC}  -s XLSCSEG31 matrices2.c

ddistribs.c.o � {MAKEFILE} ddistribs.c
	 {CC}  -s XLSCSEG32 ddistribs.c

distribs.c.o � {MAKEFILE} distribs.c
	 {CC}  -s XLSCSEG33 distribs.c

betabase.c.o � {MAKEFILE} betabase.c
	 {CC}  -s XLSCSEG34 betabase.c
bivnor.c.o � {MAKEFILE} bivnor.c
	 {CC}  -s XLSCSEG34 bivnor.c
gamln.c.o � {MAKEFILE} gamln.c
	 {CC}  -s XLSCSEG34 gamln.c
gammabase.c.o � {MAKEFILE} gammabase.c
	 {CC}  -s XLSCSEG34 gammabase.c
nor.c.o � {MAKEFILE} nor.c
	 {CC}  -s XLSCSEG34 nor.c
ppnd.c.o � {MAKEFILE} ppnd.c
	 {CC}  -s XLSCSEG34 ppnd.c
studentbase.c.o � {MAKEFILE} studentbase.c
	 {CC}  -s XLSCSEG34 studentbase.c

xlinit.c.o � {MAKEFILE} xlinit.c
	 {CC}  -s XLSCSEG35 xlinit.c

xsscatterpl.c.o � {MAKEFILE} xsscatterpl.c
	 {CC}  -s XLSCSEG36 xsscatterpl.c

utilities.c.o � {MAKEFILE} utilities.c
	 {CC}  -s XLSCSEG37 utilities.c
utilities2.c.o � {MAKEFILE} utilities2.c
	 {CC}  -s XLSCSEG37 utilities2.c

derivatives.c.o � {MAKEFILE} derivatives.c
	 {CC}  -s XLSCSEG38 derivatives.c
functions.c.o � {MAKEFILE} functions.c
	 {CC}  -s XLSCSEG38 functions.c
minimize.c.o � {MAKEFILE} minimize.c
	 {CC}  -s XLSCSEG38 minimize.c
xsbayes.c.o � {MAKEFILE} xsbayes.c
	 {CC}  -s XLSCSEG38 xsbayes.c
	 
lowess.c.o � {MAKEFILE} lowess.c
	 {CC}  -s XLSCSEG38a lowess.c
splines.c.o � {MAKEFILE} splines.c
	 {CC}  -s XLSCSEG38a splines.c

macdynload.c.o � {MAKEFILE} macdynload.c xlsx.h
	 {CC}  -s XLSCSEG39 macdynload.c
	 
cfft.c.o � {MAKEFILE} cfft.c
	 {CC}  -s XLSCSEG40 cfft.c

ALIBS = �
		"{CLibraries}"CRuntime.o �
		"{Libraries}"Interface.o �
		"{CLibraries}"StdCLib.o �
		"{CLibraries}"CSANELib.o �
		"{CLibraries}"Math.o �
		"{CLibraries}"CInterface.o
		
ALIBS881 = �
		"{CLibraries}"CLib881.o �
		"{CLibraries}"CRuntime.o �
		"{Libraries}"Interface.o �
		"{CLibraries}"StdCLib.o �
		"{CLibraries}"CSANELib881.o �
		"{CLibraries}"Math881.o �
		"{CLibraries}"CInterface.o

TLIBS = �
		"{Libraries}"Stubs.o �
		"{CLibraries}"CRuntime.o �
		"{Libraries}"Interface.o �
		"{CLibraries}"StdCLib.o �
		"{CLibraries}"CSANELib.o �
		"{CLibraries}"Math.o �
		"{CLibraries}"CInterface.o �
		"{Libraries}"ToolLibs.o

OBJECTS = {OBJECTS1} {OBJECTS2} {OBJECTS3} {OBJECTS4} {OBJECTS5} {GRAPHOBJECTS}

XLISP-STAT �� XLISP-STAT.make {OBJECTS} XLISP.proj.Rsrc
	duplicate -y XLISP.proj.Rsrc XLISP-STAT
	Link {LFLAGS} {OBJECTS} {LIBS} -o XLISP-STAT

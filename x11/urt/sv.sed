
s/svfb_global\.h/rle.h/g
s/svfb\.h/rle_put.h/g
s/XtndRunsv\.h/rle_code.h/g
s/rle_getraw\.h/rle_raw.h/g
s/enum\([ 	]*\)sv_dispatch/enum\1rle_dispatch/g
s/SV_RED/RLE_RED/g
s/SV_GREEN/RLE_GREEN/g
s/SV_BLUE/RLE_BLUE/g
s/SV_ALPHA/RLE_ALPHA/g
s/struct\([ 	]*\)sv_globals/rle_hdr/g
s/sv_dispatch/dispatch/g
s/sv_ncolors/ncolors/g
s/sv_bg_color/bg_color/g
s/sv_alpha/alpha/g
s/sv_background/background/g
s/sv_xmin/xmin/g
s/sv_xmax/xmax/g
s/sv_ymin/ymin/g
s/sv_ymax/ymax/g
s/sv_ncmap/ncmap/g
s/sv_cmaplen/cmaplen/g
s/sv_cmap/cmap/g
s/sv_comments/comments/g
s/svfb_fd/rle_file/g
s/sv_bits/bits/g
s/SV_SET_BIT/RLE_SET_BIT/g
s/SV_CLR_BIT/RLE_CLR_BIT/g
s/SV_BIT/RLE_BIT/g
s/sv_private/private/g
s/sv_globals/rle_dflt_hdr/g
s/\([^A-Za-z_]\)globals/\1the_hdr/g
s/sv_putrow/rle_putrow/g
s/sv_putbuf/rle_putbuf/g
s/sv_puteof/rle_puteof/g
s/sv_putraw/rle_putraw/g
s/sv_setup(\( *\)RUN_DISPATCH, */rle_put_setup(\1/g
s/sv_skiprow/rle_skiprow/g
s/dispatch_tab/rle_dispatch_tab/g
s/sv_magic/rle_magic/g
s/sv_DTable/rle_DTable/g
s/rle_glob/rle_f_hdr/g
s/_globals/_hdr/g
s/\([a-zA-Z]\)globals/\1_hdr/g
s/_globs/_hdr/g
s/\([a-zA-Z]\)globs/\1_hdr/g
s/_glob/_hdr/g
s/\([a-zA-Z]\)glob/\1_hdr/g
s/\([^_a-zA-Z0-9]\)glob\([^_a-zA-Z0-9]\)/\1the_hdr\2/g
s/sv_fn/rle_fn/g
s/XtndRMAGIC/RLE_MAGIC/g
s/Globals/Header/g
s/\$Header: /a/cvs/386BSD/ports/x11/urt/sv.sed,v 1.1 1993/10/05 08:51:26 ljo Exp $Id:/

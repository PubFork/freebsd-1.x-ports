/* cursors.c - cursor definitions (automatically generated - don't edit)  */

/*  Copyright 1991 Mark Russell, University of Kent at Canterbury.
 *
 *  You can do what you like with this source code as long as
 *  you don't try to make money out of it and you include an
 *  unaltered copy of this message (including the copyright).
 */

char ups_cursors_c_sccsid[] = "@(#)cursors.c	1.13 26/4/92 (UKC)";

/*  THIS FILE IS AUTOMATICALLY GENERATED BY THE MAKEFILE - DON'T EDIT IT.
 *
 *  The first part of this file comes from cursors/start, the middle from
 *  the various cursor files, and the end from cursors/end.
 *
 *  We do this rather than just #including the cursor files for two reasons:
 *
 *	- we don't have to have an sccs id for each cursor file
 *
 *	- it makes distribution of the source easier.
 *
 *  See the cursorfile target in the Makefile.
 */

#include <local/wn.h>
#include <local/ukcprog.h>
#include "cursors.h"

#define cu_dead_width 16
#define cu_dead_height 16
#define cu_dead_x_hot 8
#define cu_dead_y_hot 8
static char cu_dead_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x80, 0x03, 0x80, 0x03, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#define cu_dead_mask_width 16
#define cu_dead_mask_height 16
#define cu_dead_mask_x_hot 8
#define cu_dead_mask_y_hot 8
static char cu_dead_mask_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0xc0, 0x07, 0xc0, 0x07, 0xc0, 0x07, 0xc0, 0x07, 0xc0, 0x07, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#define cu_display_width 16
#define cu_display_height 16
#define cu_display_x_hot 7
#define cu_display_y_hot 8
static char cu_display_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0xf0, 0x07, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00,
   0x80, 0x00, 0x80, 0x00, 0xf0, 0x07, 0x00, 0x00};
#define cu_display_mask_width 16
#define cu_display_mask_height 16
#define cu_display_mask_x_hot 7
#define cu_display_mask_y_hot 8
static char cu_display_mask_bits[] = {
   0x00, 0x00, 0xf8, 0x0f, 0xf8, 0x0f, 0xf8, 0x0f, 0xc0, 0x01, 0xc0, 0x01,
   0xc0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x01, 0xc0, 0x01,
   0xc0, 0x01, 0xf8, 0x0f, 0xf8, 0x0f, 0xf8, 0x0f};
#define cu_drag_box_width 16
#define cu_drag_box_height 16
#define cu_drag_box_x_hot 7
#define cu_drag_box_y_hot 7
static char cu_drag_box_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00,
   0xc0, 0x01, 0x78, 0x0f, 0xc0, 0x01, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#define cu_drag_box_mask_width 16
#define cu_drag_box_mask_height 16
#define cu_drag_box_mask_x_hot 7
#define cu_drag_box_mask_y_hot 7
static char cu_drag_box_mask_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0xc0, 0x01, 0xc0, 0x01, 0xc0, 0x01, 0xe0, 0x03,
   0xfc, 0x1f, 0xfc, 0x1f, 0xfc, 0x1f, 0xe0, 0x03, 0xc0, 0x01, 0xc0, 0x01,
   0xc0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#define cu_menu_width 16
#define cu_menu_height 16
#define cu_menu_x_hot 7
#define cu_menu_y_hot 7
static char cu_menu_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01,
   0x80, 0x01, 0xfc, 0x3f, 0xfc, 0x3f, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01,
   0x80, 0x01, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00};
#define cu_menu_mask_width 16
#define cu_menu_mask_height 16
#define cu_menu_mask_x_hot 7
#define cu_menu_mask_y_hot 7
static char cu_menu_mask_bits[] = {
   0x00, 0x00, 0xc0, 0x03, 0xc0, 0x03, 0xc0, 0x03, 0xc0, 0x03, 0xc0, 0x03,
   0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f, 0xc0, 0x03, 0xc0, 0x03,
   0xc0, 0x03, 0xc0, 0x03, 0xc0, 0x03, 0x00, 0x00};
#define cu_sbar_width 16
#define cu_sbar_height 16
#define cu_sbar_x_hot 4
#define cu_sbar_y_hot 6
static char cu_sbar_bits[] = {
   0x10, 0x00, 0x38, 0x00, 0x7c, 0x00, 0xd6, 0x00, 0x93, 0x09, 0x10, 0x08,
   0x10, 0x1c, 0x10, 0x77, 0x10, 0x1c, 0x10, 0x08, 0x93, 0x09, 0xd6, 0x00,
   0x7c, 0x00, 0x38, 0x00, 0x10, 0x00, 0x00, 0x00};
#define cu_sbar_mask_width 16
#define cu_sbar_mask_height 16
#define cu_sbar_mask_x_hot 4
#define cu_sbar_mask_y_hot 6
static char cu_sbar_mask_bits[] = {
   0x38, 0x00, 0x7c, 0x00, 0xfe, 0x00, 0xff, 0x1d, 0xff, 0x1f, 0xbb, 0x1f,
   0xb8, 0xff, 0xb8, 0xff, 0xb8, 0xff, 0xbb, 0x1f, 0xff, 0x1f, 0xff, 0x1d,
   0xfe, 0x00, 0x7c, 0x00, 0x38, 0x00, 0x10, 0x00};
#define cu_sc_pressed_width 16
#define cu_sc_pressed_height 16
#define cu_sc_pressed_x_hot 7
#define cu_sc_pressed_y_hot 7
static char cu_sc_pressed_bits[] = {
   0x00, 0x00, 0x80, 0x00, 0xc0, 0x01, 0x60, 0x03, 0x30, 0x06, 0x18, 0x0c,
   0x0c, 0x18, 0x06, 0x30, 0x0c, 0x18, 0x18, 0x0c, 0x30, 0x06, 0x60, 0x03,
   0xc0, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00};
#define cu_sc_pressed_mask_width 16
#define cu_sc_pressed_mask_height 16
#define cu_sc_pressed_mask_x_hot 7
#define cu_sc_pressed_mask_y_hot 7
static char cu_sc_pressed_mask_bits[] = {
   0x80, 0x00, 0xc0, 0x01, 0xe0, 0x03, 0xf0, 0x07, 0x78, 0x0f, 0x3c, 0x1e,
   0x1e, 0x3c, 0x0f, 0x78, 0x1e, 0x3c, 0x3c, 0x1e, 0x78, 0x0f, 0xf0, 0x07,
   0xe0, 0x03, 0xc0, 0x01, 0x80, 0x00, 0x00, 0x00};
#define cu_scroll_down_width 16
#define cu_scroll_down_height 16
#define cu_scroll_down_x_hot 7
#define cu_scroll_down_y_hot 6
static char cu_scroll_down_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x06, 0x30, 0x0c, 0x18, 0x18, 0x0c, 0x30, 0x06, 0x60, 0x03,
   0xc0, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00};
#define cu_scroll_down_mask_width 16
#define cu_scroll_down_mask_height 16
#define cu_scroll_down_mask_x_hot 7
#define cu_scroll_down_mask_y_hot 6
static char cu_scroll_down_mask_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x07, 0x70, 0x0f, 0x78, 0x1e, 0x3c, 0x3c, 0x1e, 0x78, 0x0f, 0xf0, 0x07,
   0xe0, 0x03, 0xc0, 0x01, 0x80, 0x00, 0x00, 0x00};
#define cu_scroll_up_width 16
#define cu_scroll_up_height 16
#define cu_scroll_up_x_hot 7
#define cu_scroll_up_y_hot 7
static char cu_scroll_up_bits[] = {
   0x00, 0x00, 0x80, 0x00, 0xc0, 0x01, 0x60, 0x03, 0x30, 0x06, 0x18, 0x0c,
   0x0c, 0x18, 0x06, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#define cu_scroll_up_mask_width 16
#define cu_scroll_up_mask_height 16
#define cu_scroll_up_mask_x_hot 7
#define cu_scroll_up_mask_y_hot 7
static char cu_scroll_up_mask_bits[] = {
   0x80, 0x00, 0xc0, 0x01, 0xe0, 0x03, 0xf0, 0x07, 0x78, 0x0f, 0x3c, 0x1e,
   0x1e, 0x3c, 0x0f, 0x78, 0x07, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#define cu_wait_width 16
#define cu_wait_height 16
#define cu_wait_x_hot 8
#define cu_wait_y_hot 8
static char cu_wait_bits[] = {
   0xfe, 0x7f, 0x02, 0x40, 0x04, 0x30, 0x58, 0x1c, 0xf0, 0x0f, 0xe0, 0x07,
   0xc0, 0x03, 0x80, 0x01, 0x80, 0x01, 0x40, 0x02, 0xa0, 0x04, 0x10, 0x08,
   0x88, 0x10, 0xc4, 0x23, 0xe2, 0x47, 0xfe, 0x7f};
#define cu_wait_mask_width 16
#define cu_wait_mask_height 16
#define cu_wait_mask_x_hot 8
#define cu_wait_mask_y_hot 8
static char cu_wait_mask_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xfc, 0x3f, 0xf8, 0x1f, 0xf0, 0x0f,
   0xe0, 0x07, 0xc0, 0x03, 0xc0, 0x03, 0xe0, 0x07, 0xf0, 0x0f, 0xf8, 0x1f,
   0xfc, 0x3f, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff};
#define cu_line_width 16
#define cu_line_height 16
#define cu_line_x_hot 8
#define cu_line_y_hot 7
static char cu_line_bits[] = {
   0x00, 0x00, 0x00, 0x01, 0x80, 0x03, 0xc0, 0x07, 0x00, 0x00, 0x10, 0x10,
   0x18, 0x30, 0x1c, 0x70, 0x18, 0x30, 0x10, 0x10, 0x00, 0x00, 0xc0, 0x07,
   0x80, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00};
#define cu_line_mask_width 16
#define cu_line_mask_height 16
#define cu_line_mask_x_hot 8
#define cu_line_mask_y_hot 7
static char cu_line_mask_bits[] = {
   0x00, 0x01, 0x80, 0x03, 0xc0, 0x07, 0xe0, 0x0f, 0xf0, 0x1f, 0x38, 0x38,
   0x3c, 0x78, 0x3e, 0xf8, 0x3c, 0x78, 0x38, 0x38, 0xf0, 0x1f, 0xe0, 0x0f,
   0xc0, 0x07, 0x80, 0x03, 0x00, 0x01, 0x00, 0x00};
#define cu_dragbox_width 16
#define cu_dragbox_height 16
#define cu_dragbox_x_hot 8
#define cu_dragbox_y_hot 7
static char cu_dragbox_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x02, 0x40, 0x05, 0x00, 0x01,
   0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x40, 0x05, 0x80, 0x02,
   0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#define cu_dragbox_mask_width 16
#define cu_dragbox_mask_height 16
#define cu_dragbox_mask_x_hot 8
#define cu_dragbox_mask_y_hot 7
static char cu_dragbox_mask_bits[] = {
   0x00, 0x00, 0x00, 0x01, 0x80, 0x03, 0xc0, 0x07, 0xe0, 0x0f, 0xc0, 0x07,
   0x80, 0x03, 0x80, 0x03, 0x80, 0x03, 0xc0, 0x07, 0xe0, 0x0f, 0xc0, 0x07,
   0x80, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00};

static bmcursor_t Cursortab[] = {
#define CU_DEAD		0
	{ wn_make_x11_bmc_cursor(cu_dead, cu_dead_mask) },

#define CU_DISPLAY	1
	{ wn_make_x11_bmc_cursor(cu_display, cu_display_mask) },

#define CU_DRAG_BOX	2
	{ wn_make_x11_bmc_cursor(cu_drag_box, cu_drag_box_mask) },

#define CU_MENU		3
	{ wn_make_x11_bmc_cursor(cu_menu, cu_menu_mask) },

#define CU_SBAR		4
	{ wn_make_x11_bmc_cursor(cu_sbar, cu_sbar_mask) },

#define CU_SC_PRESSED	5
	{ wn_make_x11_bmc_cursor(cu_sc_pressed, cu_sc_pressed_mask) },

#define CU_SCROLL_DOWN	6
	{ wn_make_x11_bmc_cursor(cu_scroll_down, cu_scroll_down_mask) },

#define CU_SCROLL_UP	7
	{ wn_make_x11_bmc_cursor(cu_scroll_up, cu_scroll_up_mask) },

#define CU_WAIT		8
	{ wn_make_x11_bmc_cursor(cu_wait, cu_wait_mask) },

#define CU_LINE		9
	{ wn_make_x11_bmc_cursor(cu_line, cu_line_mask) },

#define CU_DRAGBOX	10
	{ wn_make_x11_bmc_cursor(cu_dragbox, cu_dragbox_mask) },
};

#define NUM_CURSORS	(sizeof(Cursortab) / sizeof(Cursortab[0]))

void
set_bm_cursor(wn, cno)
int wn, cno;
{
	bmcursor_t *bmc;

	if (cno < 0 || cno >= NUM_CURSORS)
		panic("sbc cno botch");
	bmc = Cursortab + cno;
	if (bmc->bmc_cid == 0)
		bmc->bmc_cid = wn_create_cursor(&bmc->bmc_cursor, &bmc->bmc_mask);
	wn_define_cursor(wn, bmc->bmc_cid);
}

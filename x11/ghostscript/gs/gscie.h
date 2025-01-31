/* Copyright (C) 1992, 1993 Aladdin Enterprises.  All rights reserved.

This file is part of Ghostscript.

Ghostscript is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY.  No author or distributor accepts responsibility
to anyone for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.  Refer
to the Ghostscript General Public License for full details.

Everyone is granted permission to copy, modify and redistribute
Ghostscript, but only under the conditions described in the Ghostscript
General Public License.  A copy of this license is supposed to have been
given to you along with Ghostscript so you can know your rights and
responsibilities.  It should be in a file named COPYING.  Among other
things, the copyright notice and this notice must be preserved on all
copies.  */

/* gscie.h */
/* Structures for Ghostscript CIE color algorithms */
/* (requires gscspace.h, gscolor2.h, gxrefct.h)

/* ------ Common definitions ------ */

/* A 3-element vector. */
typedef struct gs_vector3_s {
	float u, v, w;
} gs_vector3;

/* A 3x3 matrix, stored in column order. */
typedef struct gs_matrix3_s {
	gs_vector3 cu, cv, cw;
	int is_identity;
} gs_matrix3;

/* A 3-element vector of ranges. */
typedef struct gs_range_s {
	float rmin, rmax;
} gs_range;
typedef struct gs_range3_s {
	gs_range u, v, w;
} gs_range3;

/* Client-supplied transformation procedures. */
struct gs_cie_common_s;
typedef struct gs_cie_common_s gs_cie_common;
struct gs_cie_wbsd_s;
typedef struct gs_cie_wbsd_s gs_cie_wbsd;
typedef int (*gs_cie_a_proc)(P3(const float *,
  const gs_cie_a *, float *));
typedef int (*gs_cie_abc_proc3)(P3(const gs_vector3 *,
  const gs_cie_abc *, gs_vector3 *));
typedef int (*gs_cie_common_proc3)(P3(const gs_vector3 *,
  const gs_cie_common *, gs_vector3 *));
typedef int (*gs_cie_render_proc3)(P3(const gs_vector3 *,
  const gs_cie_render *, gs_vector3 *));
typedef int (*gs_cie_transform_proc3)(P4(const gs_vector3 *,
  const gs_cie_wbsd *, const gs_cie_render *, gs_vector3 *));
typedef int (*gs_cie_render_table_proc)(P4(const byte *, int,
  const gs_cie_render *, float *));

/* CIE white and black points. */
typedef struct gs_cie_wb_s {
	gs_vector3 WhitePoint;
	gs_vector3 BlackPoint;
} gs_cie_wb;

/* ------ Caches ------ */

/*
 * Given that all the client-supplied procedures involved in CIE color
 * mapping and rendering are monotonic, and given that we can determine
 * the minimum and maximum input values for them, we can cache their values.
 * This takes quite a lot of space, but eliminates the need for callbacks
 * deep in the graphics code (particularly the image operator).
 *
 * The procedures, and how we determine their domains, are as follows:

Stage		Name		Domain determination
-----		----		--------------------
color space	DecodeA		RangeA
color space	DecodeABC	RangeABC
color space	DecodeLMN	RangeLMN
rendering	TransformPQR	RangePQR
  (but depends on color space White/BlackPoints)
rendering	EncodeLMN	RangePQR transformed by the inverse of
				  MatrixPQR and then by MatrixLMN
rendering	EncodeABC	RangeLMN transformed by MatrixABC
rendering	RenderTable.T	[0..1]*m

 * Note that we can mostly cache the results of the color space procedures
 * without knowing the color rendering parameters, and vice versa,
 * because of the range parameters supplied in the dictionaries.
 * We suspect it's no accident that Adobe specified things this way.  :-)
 * Unfortunately, TransformPQR is an exception.
 *
 * If we wish, we can often cache the values as fracs rather than floats,
 * when we can determine the range as well as the domain.  We don't
 * do this currently, because it's extra work for a modest space payoff.
 */

/* The index into a cache is (value - base) * factor, where */
/* factor is computed as (cie_cache_size - 1) / (rmax - rmin). */
#define gx_cie_log2_cache_size 8
#define gx_cie_cache_size (1 << gx_cie_log2_cache_size)
typedef struct gx_cie_cache_s {
	float base, factor;
	float values[gx_cie_cache_size];
	int is_identity;
} gx_cie_cache;
#if gx_cie_log2_cache_size < 8
#  define gx_cie_byte_to_cache_index(b)\
     ((b) >> (8 - gx_cie_log2_cache_size))
#else
#  if gx_cie_log2_cache_size > 8
#    define gx_cie_byte_to_cache_index(b)\
       (((b) << (gx_cie_log2_cache_size - 8)) +\
        ((b) >> (16 - gx_cie_log2_cache_size)))
#  else
#    define gx_cie_byte_to_cache_index(b) (b)
#  endif
#endif

/* ------ Color space dictionaries ------ */

/* Elements common to ABC and A dictionaries. */
struct gs_cie_common_s {
	gs_range3 RangeLMN;
	gs_cie_common_proc3 DecodeLMN;
	gs_matrix3 MatrixLMN;
	gs_cie_wb points;
		/* Following are computed when structure is initialized. */
	struct {
		gx_cie_cache DecodeLMN[3];
	} caches;
};

/* A CIEBasedABC dictionary. */
struct gs_cie_abc_s {
	gs_cie_common common;		/* must be first */
	rc_header rc;
	gs_range3 RangeABC;
	gs_cie_abc_proc3 DecodeABC;
	gs_matrix3 MatrixABC;
		/* Following are computed when structure is initialized. */
	struct {
		gx_cie_cache DecodeABC[3];
	} caches;
};

/* A CIEBasedA dictionary. */
struct gs_cie_a_s {
	gs_cie_common common;		/* must be first */
	rc_header rc;
	gs_range RangeA;
	gs_cie_a_proc DecodeA;
	gs_vector3 MatrixA;
		/* Following are computed when structure is initialized. */
	struct {
		gx_cie_cache DecodeA;
	} caches;
};

/* Default values for components */
extern const gs_range3 Range3_default;
extern const gs_cie_abc_proc3 DecodeABC_default;
extern const gs_cie_common_proc3 DecodeLMN_default;
extern const gs_matrix3 Matrix3_default;
extern const gs_range RangeA_default;
extern const gs_cie_a_proc DecodeA_default;
extern const gs_vector3 MatrixA_default;
extern const gs_vector3 BlackPoint_default;
extern const gs_cie_render_proc3 Encode_default;
extern const gs_cie_transform_proc3 TransformPQR_default;
extern const gs_cie_render_table_proc RenderTableT_default;

/* ------ Rendering dictionaries ------ */

struct gs_cie_wbsd_s {
	struct { gs_vector3 xyz, pqr; } ws, bs, wd, bd;
};
typedef struct gs_cie_render_table_s {
	int NA, NB, NC;			/* >1 */
	const byte **table;		/* [NA][m * NB * NC] */
					/* 0 means no table */
	int m;				/* 3 or 4 */
	gs_cie_render_table_proc T;	/* takes byte[m], returns float[m] */
} gs_cie_render_table;
/* The main dictionary */
struct gs_cie_render_s {
	rc_header rc;
	gs_matrix3 MatrixLMN;
	gs_cie_render_proc3 EncodeLMN;
	gs_range3 RangeLMN;
	gs_matrix3 MatrixABC;
	gs_cie_render_proc3 EncodeABC;
	gs_range3 RangeABC;
	gs_cie_wb points;
	gs_matrix3 MatrixPQR;
	gs_range3 RangePQR;
	gs_cie_transform_proc3 TransformPQR;
	gs_cie_render_table RenderTable;
		/* Following are computed when structure is initialized. */
	gs_matrix3 MatrixPQR_inverse_LMN;
	gs_vector3 wdpqr, bdpqr;
	struct {
		gx_cie_cache EncodeLMN[3];
		gx_cie_cache EncodeABC[3];
		gx_cie_cache RenderTableT[4];
	} caches;
};

/* ------ Joint caches ------ */

/* This cache depends on both the color space and the rendering */
/* dictionary -- see above. */

typedef struct gx_cie_joint_caches_s {
	rc_header rc;
	gs_cie_wbsd points_sd;
	gx_cie_cache TransformPQR[3];
} gx_cie_joint_caches;

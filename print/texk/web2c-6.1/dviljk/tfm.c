/* tfm.c -- read tfm widths resident font support in dvilj. Originally
   written by kb@cs.umb.edu in early 1994. Public domain. */

#include <stdio.h>

#include <kpathsea/config.h>
#include <kpathsea/c-fopen.h>
#include <kpathsea/lib.h>
#include <kpathsea/tex-file.h>

#include "config.h" /* for STRSIZE and tfm_info_type, at least */


/* Defined in dvi2xx.c. */
extern long NoSignExtend ();
#define TFM_GET_TWO()  NoSignExtend (tfm_fp, 2)
#define TFM_GET_FOUR() NoSignExtend (tfm_fp, 4)

/* Defined in xmalloc.c. */
extern void *xmalloc ();


/* Read N words (N * 4 bytes) from TFM_FP and return it in *OUTBUF, unless
   OUTBUF==NULL, in which case throw them away. */

static void
tfm_get_n (tfm_fp, nwords, outbuf)
    FILE *tfm_fp;
    unsigned nwords;
    void **outbuf;
{
  unsigned n = nwords * 4;
  void *buf = (void *) xmalloc (n);

  if (fread (buf, n, 1, tfm_fp) != 1) {
    fprintf (stderr, "dvi2xx: Could not read %u bytes from TFM file.\n", n);
    exit (1);
  }

  /* If OUTBUF is null, free what we just read, else return it. */
  if (outbuf) {
    *outbuf = buf;
  } else {
    free (buf);
  }
}


/* Read a string in BCPL format from DATA into STR, and terminate with a
   null byte. First byte of DATA says how many characters follow.
   Assume STR is long enough.  */

static void
get_bcpl (data, str)
    unsigned char *data;
    char str[];
{
  unsigned i;
  unsigned length = *data;
  
  for (i = 0; i < length; i++) {
    str[i] = data[i + 1];
  }
  str[length] = 0;
}

/* Header word 18:
     2 bytes: "KN" for Karl and Norm---this identifies our extensions
     1 byte : 1 if proportional, 0 otherwise
     1 byte : reserved (to extend the style, if necessary)
   Header word 19:
     2 bytes: PCL style selection number
     1 byte : reserved (to extend weight, if necessary)
     1 byte : weight (signed, 2's complement, valid -7 to +7)
   Header word 20:
     2 bytes: reserved (to extend typeface id, if necessary)
     2 bytes: PCL typeface selection number
   
   The first (BigEndian) byte of header word #18 is DATA[0].
   Assume DATA is long enough for everything we might try to read. */

static bool
get_pcl_info (data, spacing, style, weight, typeface_id)
    unsigned char *data;
    unsigned *spacing;
    unsigned *style;
    int *weight;
    unsigned *typeface_id;
{
  /* No magic number for our extensions => forget it. */
  if (data[0] != 'K' && data[1] != 'N')
    return FALSE;

  *spacing = data[(0* 4) + 2]; /* Third byte of first word. */
  
  /* First two bytes of second word. */
  *style = (data[(1 * 4)] << 8) + data[(1 * 4) + 1];
  
  /* Last byte of second word, signed two-complement. */
  *weight = data[(1 * 4) + 3];
  if (*weight >= 128) *weight -= 256;
  
  /* Effectively all four bytes of third word. */
  *typeface_id = (data[(2 * 4) + 0] << 24) + (data[(2 * 4) + 1] << 16)
               + (data[(2 * 4) + 2] << 8)  + (data[(2 * 4) + 3]);

  return TRUE;
}

/* If the TFM file NAME exists, set the elements of RET and return true.
   Otherwise, return false.  */

bool
tfm_read_info (name, ret)
    char *name;
    tfm_info_type *ret;
{
  /* Don't use a structure for this, since then it might occupy more
     than the exactly four bytes that we need. */
  unsigned char *char_info;
  FILE *tfm_fp;
  unsigned char *header_data;
  unsigned char *width_raw; /* array of 1-byte data */
  unsigned long *width_table; /* array of 4-byte fixes */
  unsigned i;
  unsigned lh, bc, ec, nw, nh, nd, ni, nl, nk, ne, np;
  char *full_name = kpse_find_tfm (name);
  
  if (full_name == NULL) {
    return FALSE;
  }
  tfm_fp = xfopen (full_name, FOPEN_RBIN_MODE);

  (void) TFM_GET_TWO ();     /* word length of file */
  lh = TFM_GET_TWO ();       /* words of header data */
  bc = TFM_GET_TWO ();       /* smallest character code */
  ec = TFM_GET_TWO ();       /* largest character code */
  nw = TFM_GET_TWO ();       /* words in width table */
  nh = TFM_GET_TWO ();       /* words in height table */
  nd = TFM_GET_TWO ();       /* words in depth table */
  ni = TFM_GET_TWO ();       /* words in italic correction table */
  nl = TFM_GET_TWO ();       /* words in lig/kern table */
  nk = TFM_GET_TWO ();       /* words in kern table */
  ne = TFM_GET_TWO ();       /* words in extensible char table */
  np = TFM_GET_TWO ();       /* words of font parameter data */

  tfm_get_n (tfm_fp, lh, &header_data);
  /* Only two headerbyte words are required by the TFM format, so don't
     insist on all this extra stuff. */
  if (lh > 2) {
    get_bcpl (header_data + (2 * 4), ret->coding_scheme);
  } else {
    ret->coding_scheme[0] = 0;
  }
  
  if (lh > 12) {
    get_bcpl (header_data + (12 * 4), ret->family);
  } else {
    ret->family[0] = 0;
  }

  /* Sorry for the convoluted logic. The idea is that if the family
     is HPAUTOTFM, we better have our extensions -- so if we don't
     have enough header words, or if we don't find what we need in
     the header words, something's seriously wrong, and we shouldn't
     claim to have succeeded at reading a good TFM file.  */
  if (strcmp (ret->family, "HPAUTOTFM") == 0
      && (lh < 20
          || !get_pcl_info (&(header_data[18 * 4]),
                            &ret->spacing, &ret->style, &ret->weight,
                            &ret->typeface_id))) {
    return FALSE;
  }
  
  /* Initialize our returned array of widths to zero, since the TFM file
     need not contain info for all character codes. */
  for (i = 0; i < bc; i++) {
    ret->widths[i] = 0;
  }
  for (i = ec + 1; i < 256; i++) {
    ret->widths[i] = 0;
  }
  
  /* The char_info is one word (four bytes) for each character in the font. */
  tfm_get_n (tfm_fp, ec - bc + 1, &char_info);
  
  /* The width table is just nw words. */
  tfm_get_n (tfm_fp, nw, &width_raw);
  width_table = (unsigned long *) xmalloc (nw * 4);

  /* But the width table contains four-byte numbers, so have to convert
     from BigEndian to host order. */
  for (i = 0; i < nw; i++) {
    unsigned byte_offset = i * 4;
    unsigned b1 = width_raw[byte_offset];
    unsigned b2 = width_raw[byte_offset + 1];
    unsigned b3 = width_raw[byte_offset + 2];
    unsigned b4 = width_raw[byte_offset + 3];
    width_table[i] = (b1 << 24) + (b2 << 16) + (b3 << 8) + b4;
  }

  /* For each character, retrieve and store the TFM width. */
  for (i = bc; i <= ec; i++) {
    unsigned char w_i = char_info[(i - bc) * 4];
    ret->widths[i] = width_table[w_i];
  }

  /* Throw away everything up to the second font parameter. (Could just
     seek, but I don't want to pull in the include files, etc.) */
  if (np >= 2) {
    tfm_get_n (tfm_fp, nh + nd + ni + nl + nk + ne + 1, NULL);
    ret->interword = TFM_GET_FOUR ();
  } else {
    ret->interword = 0;
  }

  free (header_data);
  free (char_info);
  free (width_raw);
  free (width_table);
  
  fclose (tfm_fp);
  return TRUE;
}

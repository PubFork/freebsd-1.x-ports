static char RCSId[] = "$Id: dump.c,v 1.1 1994/02/24 08:05:40 hsu Exp $";
static char Copyright[] = "Copyright  Robert J. Amstadt, 1993";

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#ifdef linux
#include <linux/unistd.h>
#include <linux/head.h>
#include <linux/ldt.h>
#endif
#include <errno.h>
#include "neexe.h"
#include "segmem.h"
#include "prototypes.h"

/**********************************************************************
 *					PrintFileHeader
 */
void
PrintFileHeader(struct ne_header_s *ne_header)
{
    printf("ne_header: %c%c\n",
	   ne_header->header_type[0], 
	   ne_header->header_type[1]);
    printf("linker version: %d.%d\n", ne_header->linker_version,
	   ne_header->linker_revision);
    printf("format flags: %04.4x\n", ne_header->format_flags);
    printf("automatic data segment: %04.4x\n", ne_header->auto_data_seg);
    printf("CS:IP  %04.4x:%04.4x\n", ne_header->cs, ne_header->ip);
    printf("SS:SP  %04.4x:%04.4x\n", ne_header->ss, ne_header->sp);
    printf("additional flags: %02.2x\n", ne_header->additional_flags);
    printf("operating system: %02.2x\n", ne_header->operating_system);
    printf("fast load offset: %04.4x\n", ne_header->fastload_offset);
    printf("fast load length: %04.4x\n", ne_header->fastload_length);
}

/**********************************************************************
 *					PrintSegmentTable
 */
void
PrintSegmentTable(struct ne_segment_table_entry_s *seg_table, int nentries)
{
    int i;

    for (i = 0; i < nentries; i++)
    {
	printf("  %2d: OFFSET %04.4x, LENGTH %04.4x, ",
	       i + 1, seg_table[i].seg_data_offset, 
	       seg_table[i].seg_data_length);
	printf("FLAGS %04.4x, MIN ALLOC %04.4x\n",
	       seg_table[i].seg_flags, seg_table[i].min_alloc);
    }
}

/**********************************************************************
 *					PrintRelocationTable
 */
void 
PrintRelocationTable(char *exe_ptr, 
		     struct ne_segment_table_entry_s *seg_entry_p,
		     int segment)
{
    struct relocation_entry_s *rep;
    int i;
    int offset;
    u_short n_entries, *sp;

    printf("RELOCATION TABLE %d:\n", segment + 1);
    
    if (seg_entry_p->seg_data_offset == 0)
	return;

    offset = seg_entry_p->seg_data_length;
    if (offset == 0)
	offset = 0x10000;

    sp = (u_short *) (exe_ptr + seg_entry_p->seg_data_offset * 512 + offset);
    n_entries = *sp;

    rep = (struct relocation_entry_s *) (sp + 1);
    for (i = 0; i < n_entries; i++, rep++)
    {
	printf("  ADDR TYPE %d,  TYPE %d,  OFFSET %04.4x,",
	       rep->address_type, rep->relocation_type, rep->offset);
	printf("TARGET %04.4x %04.4x\n", rep->target1, rep->target2);
    }
}

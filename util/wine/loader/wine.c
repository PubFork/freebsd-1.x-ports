static char RCSId[] = "$Id: wine.c,v 1.1 1994/02/24 08:05:42 hsu Exp $";
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
#include <linux/segment.h>
#endif
#include <string.h>
#include <errno.h>
#include "neexe.h"
#include "segmem.h"
#include "prototypes.h"
#include "dlls.h"
#include "wine.h"
#include "windows.h"
#include "wineopts.h"

/* #define DEBUG_FIXUP */

extern int CallToInit16(unsigned long csip, unsigned long sssp, 
			unsigned short ds);
extern void CallTo32();

char * GetModuleName(struct w_files * wpnt, int index, char *buffer);
extern unsigned char ran_out;
extern char WindowsPath[256];
unsigned short WIN_StackSize;
unsigned short WIN_HeapSize;

struct  w_files * wine_files = NULL;

int WineForceFail = 0;

char **Argv;
int Argc;
struct mz_header_s *CurrentMZHeader;
struct ne_header_s *CurrentNEHeader;
int CurrentNEFile;
HINSTANCE hSysRes;

static char *Extensions[] = { "dll", "exe", NULL };
static char *WinePath = NULL;

FILE *SpyFp = NULL;

/**********************************************************************
 *					myerror
 */
void
myerror(const char *s)
{
    if (s == NULL)
	perror("wine");
    else
	fprintf(stderr, "wine: %s\n", s);

    exit(1);
}

/**********************************************************************
 *					GetFilenameFromInstance
 */
char *
GetFilenameFromInstance(unsigned short instance)
{
    register struct w_files *w = wine_files;

    while (w && w->hinstance != instance)
	w = w->next;
    
    if (w)
	return w->filename;
    else
	return NULL;
}

struct w_files *
GetFileInfo(unsigned short instance)
{
    register struct w_files *w = wine_files;

    while (w && w->hinstance != instance)
	w = w->next;
    
    return w;
}

/**********************************************************************
 *					LoadImage
 * Load one NE format executable into memory
 */
HINSTANCE LoadImage(char *modulename)
{
    unsigned int read_size;
    int i;
    struct w_files * wpnt, *wpnt1;
    unsigned int status;
    char buffer[256];

    /*
     * search file
     */
    if (FindFile(buffer, sizeof(buffer), modulename, Extensions, WindowsPath)
    	==NULL)
    {
	

    	fprintf(stderr,"LoadImage: I can't find %s !\n",modulename);
	return (HINSTANCE) NULL;
    }
    fprintf(stderr,"LoadImage: loading %s (%s)\n", modulename, buffer);

    /* First allocate a spot to store the info we collect, and add it to
     * our linked list.
     */

    wpnt = (struct w_files *) malloc(sizeof(struct w_files));
    if(wine_files == NULL)
      wine_files = wpnt;
    else {
      wpnt1 = wine_files;
      while(wpnt1->next) wpnt1 =  wpnt1->next;
      wpnt1->next  = wpnt;
    };
    wpnt->next = NULL;

    /*
     * Open file for reading.
     */
    wpnt->fd = open(buffer, O_RDONLY);
    if (wpnt->fd < 0)
    {
	myerror(NULL);
    }
    /*
     * Establish header pointers.
     */
    wpnt->filename = strdup(buffer);
    wpnt->name = NULL;
    if(modulename)  wpnt->name = strdup(modulename);

    wpnt->mz_header = (struct mz_header_s *) malloc(sizeof(struct mz_header_s));;
    status = lseek(wpnt->fd, 0, SEEK_SET);
    if (read(wpnt->fd, wpnt->mz_header, sizeof(struct mz_header_s)) !=
	sizeof(struct mz_header_s))
    {
	myerror("Unable to read MZ header from file");
    }
    if (wpnt->mz_header->must_be_0x40 != 0x40)
	myerror("This is not a Windows program");
    
    wpnt->ne_header = (struct ne_header_s *) malloc(sizeof(struct ne_header_s));
    status = lseek(wpnt->fd, wpnt->mz_header->ne_offset, SEEK_SET);
    if (read(wpnt->fd, wpnt->ne_header, sizeof(struct ne_header_s)) 
	!= sizeof(struct ne_header_s))
    {
	myerror("Unable to read NE header from file");
    }
    if (wpnt->ne_header->header_type[0] != 'N' || 
	wpnt->ne_header->header_type[1] != 'E')
      myerror("This is not a Windows program");

    if(wine_files ==  wpnt){
      CurrentMZHeader = wpnt->mz_header;
      CurrentNEHeader = wpnt->ne_header;
      CurrentNEFile   = wpnt->fd;
      
      WIN_StackSize = wpnt->ne_header->stack_length;
      WIN_HeapSize = wpnt->ne_header->local_heap_length;
    };

    /*
     * Create segment selectors.
     */
    status = lseek(wpnt->fd, wpnt->mz_header->ne_offset + 
		   wpnt->ne_header->segment_tab_offset,
		   SEEK_SET);
    read_size  = wpnt->ne_header->n_segment_tab *
	         sizeof(struct ne_segment_table_entry_s);
    wpnt->seg_table = (struct ne_segment_table_entry_s *) malloc(read_size);
    if (read(wpnt->fd, wpnt->seg_table, read_size) != read_size)
	myerror("Unable to read segment table header from file");
    wpnt->selector_table = CreateSelectors(wpnt);
    wpnt->hinstance 
	= wpnt->
	    selector_table[wpnt->ne_header->auto_data_seg-1].selector;

    /* Get the lookup  table.  This is used for looking up the addresses
       of functions that are exported */

    read_size  = wpnt->ne_header->entry_tab_length;
    wpnt->lookup_table = (char *) malloc(read_size);
    lseek(wpnt->fd, wpnt->mz_header->ne_offset + 
	  wpnt->ne_header->entry_tab_offset, SEEK_SET);
    if (read(wpnt->fd, wpnt->lookup_table, read_size) != read_size)
	myerror("Unable to read lookup table header from file");

    /* Get the iname table.  This is used for looking up the names
       of functions that are exported */

    status = lseek(wpnt->fd, wpnt->ne_header->nrname_tab_offset,  SEEK_SET);
    read_size  = wpnt->ne_header->nrname_tab_length;
    wpnt->nrname_table = (char *) malloc(read_size);
    if (read(wpnt->fd, wpnt->nrname_table, read_size) != read_size)
	myerror("Unable to read nrname table header from file");

    status = lseek(wpnt->fd, wpnt->mz_header->ne_offset + 
		   wpnt->ne_header->rname_tab_offset,  SEEK_SET);
    read_size  = wpnt->ne_header->moduleref_tab_offset - 
	    wpnt->ne_header->rname_tab_offset;
    wpnt->rname_table = (char *) malloc(read_size);
    if (read(wpnt->fd, wpnt->rname_table, read_size) != read_size)
	myerror("Unable to read rname table header from file");

    /* Now get the module name */

    wpnt->name  = (char*) malloc(*wpnt->rname_table + 1);
    memcpy(wpnt->name, wpnt->rname_table+1, *wpnt->rname_table);
    wpnt->name[*wpnt->rname_table] =  0;

    /*
     * Now load any DLLs that  this module refers to.
     */
    for(i=0; i<wpnt->ne_header->n_mod_ref_tab; i++){
      char buff[14];
      char buff2[256];
      int  fd, j;
      GetModuleName(wpnt, i + 1, buff);
      
      if(FindDLLTable(buff)) continue;  /* This module already loaded */

      LoadImage(buff);
/*
      fprintf(stderr,"Unable to load:%s\n",  buff);
*/
    }
return(wpnt->hinstance);
}

/**********************************************************************
 *					ParseArgs
 */
void
ParseArgs(int argc, char **argv)
{
    if (argc < 2)
    {
	fprintf(stderr, "usage: %s [-spy FILENAME] FILENAME\n", argv[0]);
	exit(1);
    }

    Argc = argc - 1;
    
    for (Argv = argv + 1; **Argv == '-' && Argc > 0; Argv++)
    {
	if (strcmp(*Argv, "-spy") == 0)
	{
	    if (strcmp(*(++Argv), "-") == 0)
		SpyFp = stdout;
	    else
		SpyFp = fopen(*Argv, "a");
	}
    }
}

/**********************************************************************
 *					main
 */
_WinMain(int argc, char **argv)
{
	int segment;
	char *p;
	char *sysresname;
	char filename[100];
	char syspath[256];
	char exe_path[256];
#ifdef WINESTAT
	char * cp;
#endif
	struct w_files * wpnt;
	int cs_reg, ds_reg, ss_reg, ip_reg, sp_reg;
	int i;
	int rv;

	ParseArgs(argc, argv);

	WinePath = malloc(1024);
	
	getcwd(WinePath, 512);
	
	if ((p = getenv("WINEPATH")) != NULL) { 
		strcat(WinePath, ";");
		strcat(WinePath, p);
	}
	
	LoadImage(Argv[0]);

	GetPrivateProfileString("wine", "SystemResources", "sysres.dll", 
				filename, sizeof(filename), WINE_INI);
	hSysRes = LoadImage(filename);
	if (hSysRes == (HINSTANCE)NULL)
 	    printf("Error Loading System Resources !!!\n");
 	else
 	    printf("System Resources Loaded // hSysRes='%04X'\n", hSysRes);
	
    /*
     * Fixup references.
     */
    wpnt = wine_files;
    for(wpnt = wine_files; wpnt; wpnt = wpnt->next)
      for (segment = 0; segment < wpnt->ne_header->n_segment_tab; segment++)
	{
	  if (FixupSegment(wpnt, segment) < 0)
	    {
	      myerror("fixup failed.");
	    }
	}

    /*
     * Fixup stack and jump to start.
     */
    ds_reg = wine_files->selector_table[wine_files->ne_header->auto_data_seg-1].selector;
    cs_reg = wine_files->selector_table[wine_files->ne_header->cs-1].selector;
    ip_reg = wine_files->ne_header->ip;
    ss_reg = wine_files->selector_table[wine_files->ne_header->ss-1].selector;
    sp_reg = wine_files->ne_header->sp;

#ifdef WINESTAT
    cp = strrchr(argv[0], '/');
    if(!cp) cp = argv[0];
	else cp++;
    if(strcmp(cp,"winestat") == 0) {
	    winestat();
	    exit(0);
    };
#endif

    init_wine_signals();

    if (WineForceFail)
    {
	p = (char *) ((cs_reg << 16) | ip_reg);
	
	*p++ = 0xcd;
	*p++ = 0x20;
    }

    if (ss_reg == 0)
    {
	fprintf(stderr, "SS is 0\n");
    }

    LinearTest();

    rv = CallToInit16(cs_reg << 16 | ip_reg, ss_reg << 16 | sp_reg, ds_reg);
    printf ("rv = %x\n", rv);
}


/**********************************************************************
 *					GetImportedName
 */
char *
GetImportedName(int fd, struct mz_header_s *mz_header, 
		struct ne_header_s *ne_header, int name_offset, char *buffer)
{
    char *p;
    int length;
    int status;
    int i;
    
    status = lseek(fd, mz_header->ne_offset + ne_header->iname_tab_offset +
		   name_offset, SEEK_SET);
    length = 0;
    read(fd, &length, 1);  /* Get the length byte */
    read(fd, buffer, length);
    buffer[length] = 0;
    return buffer;
}

/**********************************************************************
 *					GetModuleName
 */
char *
GetModuleName(struct w_files * wpnt, int index, char *buffer)
{
    int fd = wpnt->fd;
    struct mz_header_s *mz_header = wpnt->mz_header; 
    struct ne_header_s *ne_header = wpnt->ne_header;
    char *p;
    int length;
    int name_offset, status;
    int i;
    
    status = lseek(fd, mz_header->ne_offset + ne_header->moduleref_tab_offset +
		   2*(index - 1), SEEK_SET);
    name_offset = 0;
    read(fd, &name_offset, 2);
    status = lseek(fd, mz_header->ne_offset + ne_header->iname_tab_offset +
		   name_offset, SEEK_SET);
    length = 0;
    read(fd, &length, 1);  /* Get the length byte */
    read(fd, buffer, length);
    buffer[length] = 0;

    /* Module names  are always upper case */
    for(i=0; i<length; i++)
	    if(buffer[i] >= 'a' && buffer[i] <= 'z')  buffer[i] &= ~0x20;

    return buffer;
}


/**********************************************************************
 *					FixupSegment
 */
int
FixupSegment(struct w_files * wpnt, int segment_num)
{
    int fd =  wpnt->fd;
    struct mz_header_s * mz_header = wpnt->mz_header;
    struct ne_header_s *ne_header =  wpnt->ne_header;
    struct ne_segment_table_entry_s *seg_table = wpnt->seg_table;
    struct segment_descriptor_s *selector_table = wpnt->selector_table;
    struct relocation_entry_s *rep, *rep1;
    struct ne_segment_table_entry_s *seg;
    struct segment_descriptor_s *sel;
    struct dll_table_entry_s *dll_table;
    int status;
    unsigned short *sp;
    unsigned int selector, address;
    unsigned int next_addr;
    int ordinal;
    char dll_name[257];
    char func_name[257];
    int i, n_entries;
    int additive;

    seg = &seg_table[segment_num];
    sel = &selector_table[segment_num];

#ifdef DEBUG_FIXUP
    printf("Segment fixups for %s, segment %d, selector %x\n", 
	   wpnt->name, segment_num, (int) sel->base_addr >> 16);
#endif

    if ((seg->seg_data_offset == 0) ||
	!(seg->seg_flags & NE_SEGFLAGS_RELOC_DATA))
	return 0;

    /*
     * Go through the relocation table on entry at a time.
     */
    i = seg->seg_data_length;
    if (i == 0)
	i = 0x10000;

    status = lseek(fd, seg->seg_data_offset * 
		       (1 << ne_header->align_shift_count) + i, SEEK_SET);
    n_entries = 0;
    read(fd, &n_entries, sizeof(short int));
    rep = (struct relocation_entry_s *)
	  malloc(n_entries * sizeof(struct relocation_entry_s));

    if (read(fd,rep, n_entries * sizeof(struct relocation_entry_s)) !=
        n_entries * sizeof(struct relocation_entry_s))
    {
	myerror("Unable to read relocation information");
    }
    
    rep1 = rep;

    for (i = 0; i < n_entries; i++, rep++)
    {
	/*
	 * Get the target address corresponding to this entry.
	 */
	additive = 0;
	
	switch (rep->relocation_type)
	{
	  case NE_RELTYPE_ORDINALADD:
	    additive = 1;
	    
	  case NE_RELTYPE_ORDINAL:
	    if (GetModuleName(wpnt, rep->target1,
			      dll_name) == NULL)
	    {
	      fprintf(stderr, "NE_RELTYPE_ORDINAL failed");
		return -1;
	    }
	    
	    ordinal = rep->target2;

  	    status = GetEntryDLLOrdinal(dll_name, ordinal, &selector,
					&address);
	    if (status)
	    {
		char s[80];
		
		sprintf(s, "Bad DLL name '%s.%d'", dll_name, ordinal);
		myerror(s);
		return -1;
	    }

#ifdef DEBUG_FIXUP
	    printf("%d: %s.%d: %04.4x:%04.4x\n", i + 1, dll_name, ordinal,
		   selector, address);
#endif
	    break;
	    
	  case NE_RELTYPE_NAMEADD:
	    additive = 1;
	    
	  case NE_RELTYPE_NAME:
	    if (GetModuleName(wpnt, rep->target1, dll_name)
		== NULL)
	    {
	      fprintf(stderr,"NE_RELTYPE_NAME failed");
		return -1;
	    }

	    if (GetImportedName(fd, mz_header, ne_header, 
				rep->target2, func_name) == NULL)
	    {
	      fprintf(stderr,"getimportedname failed");
		return -1;
	    }

  	    status = GetEntryDLLName(dll_name, func_name, &selector, 
					   &address);
	    if (status)
	    {
		char s[80];
		
		sprintf(s, "Bad DLL name '%s (%s)'", dll_name,func_name);
		myerror(s);
		return -1;
	    }

#ifdef DEBUG_FIXUP
	    printf("%d: %s %s.%d: %04.4x:%04.4x\n", i + 1, func_name,
		   dll_name, ordinal, selector, address);
#endif
	    break;
	    
	  case NE_RELTYPE_INTERNAL:
    	  case NE_RELTYPE_INT1:
	    if (rep->target1 == 0x00ff)
	    {
		address  = GetEntryPointFromOrdinal(wpnt, rep->target2);
		selector = (address >> 16) & 0xffff;
		address &= 0xffff;
	    }
	    else
	    {
		selector = selector_table[rep->target1-1].selector;
		address  = rep->target2;
	    }
	    
#ifdef DEBUG_FIXUP
	    printf("%d: %04.4x:%04.4x\n", i + 1, selector, address);
#endif
	    break;

	  case 7:
	    /* Relocation type 7:
	     *
	     *    These appear to be used as fixups for the Windows
	     * floating point emulator.  Let's just ignore them and
	     * try to use the hardware floating point.  Linux should
	     * successfully emulate the coprocessor if it doesn't
	     * exist.
	     */
#ifdef DEBUG_FIXUP
	    printf("%d: ADDR TYPE %d,  TYPE %d,  OFFSET %04.4x,  ",
		   i + 1, rep->address_type, rep->relocation_type, 
		   rep->offset);
	    printf("TARGET %04.4x %04.4x\n", rep->target1, rep->target2);
#endif
	    continue;
	    
	  default:
	    fprintf(stderr,"%d: ADDR TYPE %d,  TYPE %d,  OFFSET %04.4x,  ",
		   i + 1, rep->address_type, rep->relocation_type, 
		   rep->offset);
	    fprintf(stderr,"TARGET %04.4x %04.4x\n", 
		    rep->target1, rep->target2);
	    free(rep1);
	    return -1;
	}

	/*
	 * Stuff the right size result in.
	 */
	sp = (unsigned short *) ((char *) sel->base_addr + rep->offset);
	if (additive)
	{
	    if (FindDLLTable(dll_name) == NULL)
		additive = 2;

	    fprintf(stderr,"%d: ADDR TYPE %d,  TYPE %d,  OFFSET %04.4x,  ",
		   i + 1, rep->address_type, rep->relocation_type, 
		   rep->offset);
	    fprintf(stderr,"TARGET %04.4x %04.4x\n", 
		    rep->target1, rep->target2);
	    fprintf(stderr, "    Additive = %d\n", additive);
	}
	
	switch (rep->address_type)
	{
	  case NE_RADDR_OFFSET16:
	    do {
#ifdef DEBUG_FIXUP
		printf("    %04.4x:%04.4x:%04.4x OFFSET16\n",
		       (unsigned long) sp >> 16, (int) sp & 0xFFFF, *sp);
#endif
		next_addr = *sp;
		*sp = (unsigned short) address;
		if (additive == 2)
		    *sp += next_addr;
		sp = (unsigned short *) ((char *) sel->base_addr + next_addr);
	    } 
	    while (next_addr != 0xffff && !additive);

	    break;
	    
	  case NE_RADDR_POINTER32:
	    do {
#ifdef DEBUG_FIXUP
		printf("    %04.4x:%04.4x:%04.4x POINTER32\n",
		       (unsigned long) sp >> 16, (int) sp & 0xFFFF, *sp);
#endif
		next_addr = *sp;
		*sp     = (unsigned short) address;
		if (additive == 2)
		    *sp += next_addr;
		*(sp+1) = (unsigned short) selector;
		sp = (unsigned short *) ((char *) sel->base_addr + next_addr);
	    } 
	    while (next_addr != 0xffff && !additive);

	    break;
	    
	  case NE_RADDR_SELECTOR:
	    do {
#ifdef DEBUG_FIXUP
		printf("    %04.4x:%04.4x:%04.4x SELECTOR\n",
		       (unsigned long) sp >> 16, (int) sp & 0xFFFF, *sp);
#endif
		next_addr = *sp;
		*sp     = (unsigned short) selector;
		sp = (unsigned short *) ((char *) sel->base_addr + next_addr);
		if (rep->relocation_type == NE_RELTYPE_INT1) 
		    break;
	    } 
	    while (next_addr != 0xffff && !additive);

	    break;
	    
	  default:
	    printf("%d: ADDR TYPE %d,  TYPE %d,  OFFSET %04.4x,  ",
		   i + 1, rep->address_type, rep->relocation_type, 
		   rep->offset);
	    printf("TARGET %04.4x %04.4x\n", rep->target1, rep->target2);
	    free(rep1);
	    return -1;
	}
    }

    free(rep1);
    return 0;
}

/**********************************************************************
 *					GetProcAddress
 */
FARPROC GetProcAddress(HINSTANCE hinstance, char *proc_name)
{
    if ((int) proc_name & 0xffff0000)
	printf("GetProcAddress: %#04x, '%s'\n", hinstance, proc_name);
    else
	printf("GetProcAddress: %#04x, %d\n", hinstance, (int) proc_name);

    return NULL;
}

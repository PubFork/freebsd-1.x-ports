/* object100.c : Sather class: OBJECT_FILES_KEYOB, dbg=F, gc=T, chk=F */

#include "all_.h"
#include "c_names_.h"

static char __sather_compiled__;
extern void error_msg(ptr s__);
extern ptr str_ptr_(ptr s__);
extern void error_exit(ptr s__);

extern ptr OUT80_s_(ptr self__, ptr st__);
extern ptr STR20_create_(ptr self__);
extern ptr STR20_s_(ptr self__, ptr st__);
extern ptr LIS87_create_(ptr self__, int init_size__);
extern ptr LIS87_push_(ptr self__, ptr e__);
#include "macros_.h"



void OBJ100_error_msg_(ptr self__, ptr s__);
void OBJ100_error_exit_(ptr self__, ptr s__);
void OBJ100_start_(ptr self__, ptr nm__);
void OBJ100_terminate_(ptr self__);
ptr OBJ100_cmacro_def_(ptr self__, int i__);
void OBJ100_insert_(ptr self__, ptr nm__);
ptr OBJ100_create_(ptr self__);
int OBJ100_info_size_(ptr self__);
ptr OBJ100_ith_str_(ptr self__, int i__);
ptr OBJ100_ith_info_(ptr self__, int i__);
void OBJ100_cprint_self_(ptr self__, ptr outfile__);
ptr OBJ100_initialize_(ptr self__, ptr initarg__);
/*constant*/ int OBJ100_init_num_OBJECT_FILES_ = 5;
extern int attr_ent_OBJ100[];

void OBJ100_error_msg_(ptr self__, ptr s__)
{

   error_msg(str_ptr_(s__));

   ret0__:
   return;
}

void OBJ100_error_exit_(ptr self__, ptr s__)
{

   error_exit(str_ptr_(s__));

   ret0__:
   return;
}

void OBJ100_start_(ptr self__, ptr nm__)
{
   SATHER_STR_(20,38,ls1033_,"(COMPILE_KEYOB) : Unexpected string \"");
   SATHER_STR_(20,3,ls632_,"\"\n");

   OBJ100_error_msg_(self__,STR20_s_(STR20_s_(STR20_s_(STR20_create_(0),(ptr)(&ls1033_)),nm__),(ptr)(&ls632_)));

   ret0__:
   return;
}

void OBJ100_terminate_(ptr self__)
{


   ret0__:
   return;
}

ptr OBJ100_cmacro_def_(ptr self__, int i__)
{
   ptr res__ = 0;


   ret0__:
   return (res__);
}

void OBJ100_insert_(ptr self__, ptr nm__)
{

   PATT_(self__,4) = (ptr)LIS87_push_(PATT_(self__,4),nm__);

   ret0__:
   return;
}

ptr OBJ100_create_(ptr self__)
{
   ptr res__ = 0;

   res__ = (ptr)new_(100,0);
   PATT_(res__,4) = (ptr)LIS87_create_(PATT_(res__,4),(- 1));
   IATT_(res__,8) = (int)0;

   ret0__:
   return (res__);
}

int OBJ100_info_size_(ptr self__)
{
   int res__ = S_int_VOID_;

   res__ = (int)IATT_(PATT_(self__,4),4);

   ret0__:
   return (res__);
}

ptr OBJ100_ith_str_(ptr self__, int i__)
{
   ptr res__ = 0;

   res__ = (ptr)PATT_(PATT_(self__,4), 16 + ((i__) << 2));

   ret0__:
   return (res__);
}

ptr OBJ100_ith_info_(ptr self__, int i__)
{
   ptr res__ = 0;


   ret0__:
   return (res__);
}

void OBJ100_cprint_self_(ptr self__, ptr outfile__)
{
   SATHER_STR_(20,16,ls1045_,"(object_files) ");
   SATHER_STR_(20,2,ls780_,"\n");
   int    i__ = S_int_VOID_;
   int    sz__ = S_int_VOID_;

   (void)OUT80_s_(outfile__,(ptr)(&ls1045_));
   i__ = (int)0;
   sz__ = (int)IATT_(PATT_(self__,4),4);
   while (1) {
      if ((i__ >= sz__)) {
         goto goto_tag_1192_;
      }
      else {
      }
      (void)OUT80_s_(OUT80_s_(outfile__,PATT_(PATT_(self__,4), 16 + ((i__) << 2))),(ptr)(&ls780_));
      i__ = (int)(i__ + 1);
   }
goto_tag_1192_: ;
   (void)OUT80_s_(outfile__,(ptr)(&ls780_));

   ret0__:
   return;
}

ptr OBJ100_initialize_(ptr self__, ptr initarg__)
{
   ptr res__ = 0;

   res__ = (ptr)self__;

   ret0__:
   return (res__);
}


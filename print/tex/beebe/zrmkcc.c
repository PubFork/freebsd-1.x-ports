/* -*-C-*- zerom.c */
/*-->zerom*/
/**********************************************************************/
/******************************* zerom *******************************/
/**********************************************************************/

void
zerom(to, count)
int *to, count;
{
    while (count-- > 0)
	*to++ = 0;
}

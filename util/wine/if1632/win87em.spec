# $Id: win87em.spec,v 1.1 1994/02/24 08:05:33 hsu Exp $
#
name	win87em
id	5
length	10

1	register _fpMath(word word word word
			 word word word word) WIN87_fpmath()
3	pascal 	 __WinEm87Info(ptr word) WIN87_WinEm87Info(1 2)
4	pascal	 __WinEm87Restore(ptr word) WIN87_WinEm87Restore(1 2)
5	pascal	 __WinEm87Save(ptr word) WIN87_WinEm87Save(1 2)

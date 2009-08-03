#include "qsortp.h"
/*!  
** 
** Copyright (c) 2007 by John W. Ratcliff mailto:jratcliff@infiniplex.net
**
** Portions of this source has been released with the PhysXViewer application, as well as 
** Rocket, CreateDynamics, ODF, and as a number of sample code snippets.
**
** If you find this code useful or you are feeling particularily generous I would
** ask that you please go to http://www.amillionpixels.us and make a donation
** to Troy DeMolay.
**
** DeMolay is a youth group for young men between the ages of 12 and 21.  
** It teaches strong moral principles, as well as leadership skills and 
** public speaking.  The donations page uses the 'pay for pixels' paradigm
** where, in this case, a pixel is only a single penny.  Donations can be
** made for as small as $4 or as high as a $100 block.  Each person who donates
** will get a link to their own site as well as acknowledgement on the
** donations blog located here http://www.amillionpixels.blogspot.com/
**
** If you wish to contact me you can use the following methods:
**
** Skype Phone: 636-486-4040 (let it ring a long time while it goes through switches)
** Skype ID: jratcliff63367
** Yahoo: jratcliff63367
** AOL: jratcliff1961
** email: jratcliff@infiniplex.net
** Personal website: http://jratcliffscarab.blogspot.com
** Coding Website:   http://codesuppository.blogspot.com
** FundRaising Blog: http://amillionpixels.blogspot.com
** Fundraising site: http://www.amillionpixels.us
** New Temple Site:  http://newtemple.blogspot.com
**
**
** The MIT license:
**
** Permission is hereby granted, MEMALLOC_FREE of charge, to any person obtaining a copy 
** of this software and associated documentation files (the "Software"), to deal 
** in the Software without restriction, including without limitation the rights 
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
** copies of the Software, and to permit persons to whom the Software is furnished 
** to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all 
** copies or substantial portions of the Software.

** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
** WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
** CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/



void QuickSortPointers::swap(char **a,char **b)
{
	char *tmp;

	if ( a != b )
	{
		tmp = *a;
		*a++ = *b;
		*b++ = tmp;
	}
}


void QuickSortPointers::qsort(void **b,HeI32 num)
{
	char *lo,*hi;
	char *mid;
	char *bottom, *top;
	HeI32 size;
	char *lostk[30], *histk[30];
	HeI32 stkptr;
	char **base = (char **)b;

	if (num < 2 ) return;

	stkptr = 0;

	lo = (char *)base;
	hi = (char *)base + sizeof(char **) * (num-1);

nextone:

	size = (hi - lo) / sizeof(char**) + 1;

	mid = lo + (size / 2) * sizeof(char **);
	swap((char **)mid,(char **)lo);
	bottom = lo;
	top = hi + sizeof(char **);

	for (;;)
	{
		do
		{
			bottom += sizeof(char **);
		} while (bottom <= hi && compare((void **)bottom,(void **)lo) <= 0);

		do
		{
			top -= sizeof(char **);
		} while (top > lo && compare((void **)top,(void **)lo) >= 0);

		if (top < bottom) break;

		swap((char **)bottom,(char **)top);

	}

	swap((char **)lo,(char **)top);

	if ( top - 1 - lo >= hi - bottom )
	{
		if (lo + sizeof(char **) < top)
		{
			lostk[stkptr] = lo;
			histk[stkptr] = top - sizeof(char **);
			stkptr++;
		}
		if (bottom < hi)
		{
			lo = bottom;
			goto nextone;
		}
	}
	else
	{
		if ( bottom < hi )
		{
			lostk[stkptr] = bottom;
			histk[stkptr] = hi;
			stkptr++;
		}
		if (lo + sizeof(char **) < top)
		{
			hi = top - sizeof(char **);
			goto nextone; 					/* do small recursion */
		}
	}

	stkptr--;

	if (stkptr >= 0)
	{
		lo = lostk[stkptr];
		hi = histk[stkptr];
		goto nextone;
	}
	return;
}

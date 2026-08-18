/*		Case-independent string comparison		HTString.h
**		and allocations with copies
*/
#ifndef HTSTRING_H
#define HTSTRING_H

#include "HTUtils.h"

extern int strcasecomp (const char *a, const char *b);
extern int strncasecomp(const char *a, const char *b, int n);

extern char * HTSACopy(char **dest, const char *src);
extern char * HTSACat (char **dest, const char *src);

/* Enable the old macro-like calling methods */
#define StrAllocCopy(dest, src) HTSACopy (&(dest), src)
#define StrAllocCat(dest, src)  HTSACat  (&(dest), src)

#endif

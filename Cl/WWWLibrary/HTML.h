/*		The HTML Parser					HTML.h
**		---------------
*/

#ifndef HTML_H
#define HTML_H

#include "HTUtils.h"
#include "HTAnchor.h"
#include "SGML.h"

extern SGML_dtd HTML_dtd;	/* The DTD */
extern void HTML_begin(HTParentAnchor * anchor);
extern bool HTML_Parse(
	HTParentAnchor * anchor,
	char (*next_char)(void) );
#endif

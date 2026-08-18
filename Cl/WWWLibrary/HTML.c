/*		HTML Parser
**		===========
*/
#include <ctype.h>
#include <stdio.h>

#include "HTUtils.h"
#include "SGML.h"
#include "HTAtom.h"
#include "HTChunk.h"
#include "HText.h"
#include "HTStyle.h"
#include "HTML.h"


/*				SPECIAL HTML CODE
**				=================
*/

extern HTStyleSheet * styleSheet;	/* Application-wide */

PRIVATE HTParentAnchor * node_anchor;
PRIVATE HText * text;

PRIVATE HTStyle * glossary_style;
PRIVATE HTStyle * list_compact_style;
PRIVATE HTStyle * glossary_compact_style;

PRIVATE HTChunk title = { 0, 128, 0, 0 };	/* Grow by 128 */


/*	Forward declarations of routines for DTD
*/
PRIVATE void no_change(HTTag * t, HTElement * e);
PRIVATE void begin_litteral(HTTag * t, HTElement * e);
PRIVATE void begin_element(HTTag * t, HTElement * e);
PRIVATE void end_element(HTTag * t, HTElement * e);
PRIVATE void begin_document(HTTag * t, HTElement * e);
PRIVATE void end_document(HTTag * t, HTElement * e);
PRIVATE void begin_anchor(HTTag * t, HTElement * e);
PRIVATE void end_anchor(HTTag * t, HTElement * e);
PRIVATE void begin_list(HTTag * t, HTElement * e);
PRIVATE void list_element(HTTag * t, HTElement * e);
PRIVATE void end_list(HTTag * t, HTElement * e);
PRIVATE void begin_glossary(HTTag * t, HTElement * e);
PRIVATE void end_glossary(HTTag * t, HTElement * e);

PRIVATE int got_styles = 0;
PRIVATE void get_styles (void);

PRIVATE	bool		style_change;
PRIVATE HTStyle *	new_style;
PRIVATE HTStyle *	old_style;
PRIVATE bool		in_word;  /* Have just had a non-white character */

/*	Style buffering avoids dummy paragraph begin/ends.
*/
#define UPDATE_STYLE if (style_change) { \
	HText_setStyle(text, new_style); \
	old_style = new_style; \
	style_change = false; }

PRIVATE void change_style(HTStyle *style)
{
    if (new_style!=style) {
    	style_change = true /* was old_style == new_style */ ;
	new_style = style;
    }
}


/*		TITLE
*/

/*	Accumulate a character of title
*/
static void accumulate_string(char c)
{
    HTChunkPutc(&title, c);
}


/*		Clear the title
*/
PRIVATE  void clear_string(HTTag *t, HTElement *e)
{
    HTChunkClear(&title);
}

PRIVATE void set_title(HTTag *t, HTElement *e)
{
    HTChunkTerminate(&title);
    HTAnchor_setTitle(node_anchor, title.data);
}

/*		Character handling
*/
PRIVATE void set_index(HTTag *t, HTElement *e)
{
    HTAnchor_setIndex(node_anchor);
}

PRIVATE void pass_character(char c)
{
    if (style_change) {
        if ((c=='\n') || (c==' ')) return;	/* Ignore it */
        UPDATE_STYLE;
    }
    if (c=='\n') {
        if (in_word) {
	    HText_appendCharacter(text, ' ');
	    in_word = false;
	}
    } else {
        HText_appendCharacter(text, c);
	in_word = true;
    }
}

PRIVATE void litteral_text(char c)
{
/*	We guarrantee that the style is up-to-date in begin_litteral
*/
    HText_appendCharacter(text, c);		/* @@@@@ */
}

PRIVATE void ignore_text(char c)
{
    /* Do nothing */
}

PRIVATE void set_next_id(HTTag *t, HTElement *e)
{
    /* @@@@@  Bad SGML anyway */
}

PRIVATE void new_paragraph(HTTag *t, HTElement *e)
{
    UPDATE_STYLE;
    HText_appendParagraph(text);
    in_word = false;
}

PRIVATE void term(HTTag *t, HTElement *e)
{
    if (!style_change) {
        HText_appendParagraph(text);
	in_word = false;
    }
}

PRIVATE void definition(HTTag *t, HTElement *e)
{
    UPDATE_STYLE;
    pass_character('\t');	/* Just tab out one stop */
    in_word = false;
}

/*		Our Static DTD for HTML
**		-----------------------
*/

static entity entities[] = {
	{ "lt",	"<" },
	{ "gt", ">" },
	{ "amp", "&" },
	{ "bullet" , "\267" },			/* @@@ NeXT only */
	{ 0,	0 }  /* Terminate list */
};

static attr no_attr[] = {{ 0, 0 , 0}};

static attr a_attr[] = {				/* Anchor attributes */
#define A_ID 0
	{ "NAME", 0, 0 },				/* Should be ID */
#define A_TYPE 1
	{ "TYPE", 0, 0 },
#define A_HREF 2
	{ "HREF", 0, 0 },
	{ 0, 0 , 0}	/* Terminate list */
};	
static attr list_attr[] = {
#define LIST_COMPACT 0
	{ "COMPACT", 0, 0 },
	{ 0, 0, 0 }	/* Terminate list */
};

static attr glossary_attr[] = {
#define GLOSSARY_COMPACT 0
	{ "COMPACT", 0, 0 },
	{ 0, 0, 0 }	/* Terminate list */
};

static HTTag default_tag =
    { "DOCUMENT", no_attr , 0, 0, begin_document, pass_character, end_document };
/*	NAME ATTR  STYLE LITERAL?  ON_BEGIN   ON__CHARACTER     ON_END
*/
static HTTag tags[] = {
#define TITLE_TAG 0
    { "TITLE", no_attr, 0, 0, clear_string, accumulate_string, set_title },
#define ISINDEX_TAG 1
    { "ISINDEX", no_attr, 0, 0, set_index, 0 , 0 },
#define NEXTID_TAG 2
    { "NEXTID", no_attr, 0, 0, set_next_id, 0, 0 },
#define ADDRESS_TAG 3
    { "ADDRESS"	, no_attr, 0, 0, begin_element, pass_character, end_element },
#define H1_TAG 4
    { "H1"	, no_attr, 0, 0, begin_element, pass_character, end_element },
    { "H2"	, no_attr, 0, 0, begin_element, pass_character, end_element },
    { "H3"	, no_attr, 0, 0, begin_element, pass_character, end_element },
    { "H4"	, no_attr, 0, 0, begin_element, pass_character, end_element },
    { "H5"	, no_attr, 0, 0, begin_element, pass_character, end_element },
    { "H6"	, no_attr, 0, 0, begin_element, pass_character, end_element },
    { "H7"	, no_attr, 0, 0, begin_element, pass_character, end_element },
#define UL_TAG 11
    { "UL"	, list_attr, 0, 0, begin_list, pass_character, end_list },
#define OL_TAG 12
    { "OL"	, list_attr, 0, 0, begin_list, pass_character, end_list },
#define MENU_TAG 13
    { "MENU"	, list_attr, 0, 0, begin_list, pass_character, end_list },
#define DIR_TAG 14
    { "DIR"	, list_attr, 0, 0, begin_list, pass_character, end_list },
#define LI_TAG 15
    { "LI"	, no_attr, 0, 0, list_element, pass_character, 0 },
#define DL_TAG 16
    { "DL"	, list_attr, 0, 0, begin_glossary, pass_character, end_glossary },
    { "DT"	, no_attr, 0, 0, term, pass_character, 0 },
    { "DD"	, no_attr, 0, 0, definition, pass_character, 0 },
    { "A"	, a_attr,  0, 0, begin_anchor, pass_character, end_anchor },
#define P_TAG 20
    { "P"	, no_attr, 0, 0, new_paragraph, pass_character, 0 },
#define XMP_TAG 21
  { "XMP"	, no_attr, 0, true, begin_litteral, litteral_text, end_element },
#define LISTING_TAG 22
  { "LISTING"	, no_attr, 0, true,begin_litteral, litteral_text, end_element },
#define PLAINTEXT_TAG 23
  { "PLAINTEXT", no_attr, 0, true, begin_litteral, litteral_text, end_element },
#define COMMENT_TAG 24
    { "COMMENT", no_attr, 0, true, no_change, ignore_text, no_change },
    { 0, 0, 0, 0,  0, 0 , 0}	/* Terminate list */
};

SGML_dtd HTML_dtd = { tags, &default_tag, entities };


/*		Flattening the style structure
**		------------------------------
**
On the NeXT, and on any read-only browser, it is simpler for the text to have
a sequence of styles, rather than a nested tree of styles. In this
case we have to flatten the structure as it arrives from SGML tags into
a sequence of styles.
*/

/*	Anchor handling
**	---------------
*/
PRIVATE void begin_anchor(HTTag *t, HTElement *e)
{
    HTChildAnchor * source = HTAnchor_findChildAndLink(
    	node_anchor,						/* parent */
	a_attr[A_ID].present	? a_attr[A_ID].value : 0,	/* Tag */
	a_attr[A_HREF].present	? a_attr[A_HREF].value : 0,	/* Addresss */
	a_attr[A_TYPE].present	? 
		(HTLinkType*)HTAtom_for(a_attr[A_TYPE].value)
		 : 0);
    
    UPDATE_STYLE;
    HText_beginAnchor(text, source);
}

PRIVATE void end_anchor(HTTag *	 t,
			HTElement *	e)
{
    UPDATE_STYLE;
    HText_endAnchor(text);
}


/*	General SGML Element Handling
**	-----------------------------
*/
PRIVATE void begin_element(HTTag *t, HTElement *e)
{
    change_style((HTStyle*)(t->style));
}
PRIVATE void no_change(HTTag *t, HTElement *e)
{
    /* Do nothing */;
}
PRIVATE void begin_litteral(HTTag *t, HTElement *e)
{
    change_style(t->style);
    UPDATE_STYLE;
}
PRIVATE void end_element(HTTag *t, HTElement *e)
{
    if (e) change_style(e->tag->style);
}

/*			Lists
*/
PRIVATE void begin_list(HTTag *t, HTElement *e)
{
    change_style(list_attr[LIST_COMPACT].present
    		? list_compact_style
		: (HTStyle*)(t->style));
    in_word = false;
}

PRIVATE void end_list(HTTag *t, HTElement *e)
{
    change_style(e->tag->style);
    in_word = false;
}

PRIVATE void list_element(HTTag *t, HTElement *e)
{
    if (e->tag != &tags[DIR_TAG])
	HText_appendParagraph(text);
    else
        HText_appendCharacter(text, '\t');	/* Tab @@ nl for UL? */
    in_word = false;
}


PRIVATE void begin_glossary(HTTag *t, HTElement *e)
{
    change_style(glossary_attr[GLOSSARY_COMPACT].present
    		? glossary_compact_style
		: glossary_style);
    in_word = false;
}

PRIVATE void end_glossary(HTTag *t, HTElement *e)
{
    change_style(e->tag->style);
    in_word = false;
}


/*	Begin and End document
**	----------------------
*/
PUBLIC void HTML_begin(HTParentAnchor *anchor)
{
    node_anchor = anchor;
}

PRIVATE void begin_document(HTTag *t, HTElement *e)
{
    if (!got_styles) get_styles();
    text = HText_new(node_anchor);
    HText_beginAppend(text);
    HText_setStyle(text, default_tag.style);
    old_style = 0;
    style_change = false;
    in_word = false;
}

PRIVATE void end_document(HTTag *t, HTElement *e)
{
    HText_endAppend(text);

}

/*	Get Styles from style sheet
**	---------------------------
*/
PRIVATE void get_styles(void)
{
    got_styles = true;
    
    tags[P_TAG].style =
    default_tag.style =		HTStyleNamed(styleSheet, "Normal");
    tags[H1_TAG].style =	HTStyleNamed(styleSheet, "Heading1");
    tags[H1_TAG+1].style =	HTStyleNamed(styleSheet, "Heading2");
    tags[H1_TAG+2].style =	HTStyleNamed(styleSheet, "Heading3");
    tags[H1_TAG+3].style =	HTStyleNamed(styleSheet, "Heading4");
    tags[H1_TAG+4].style =	HTStyleNamed(styleSheet, "Heading5");
    tags[H1_TAG+5].style =	HTStyleNamed(styleSheet, "Heading6");
    tags[H1_TAG+6].style =	HTStyleNamed(styleSheet, "Heading7");
    tags[DL_TAG].style =	HTStyleNamed(styleSheet, "Glossary");
    tags[UL_TAG].style =	HTStyleNamed(styleSheet, "List");
    tags[OL_TAG].style =	HTStyleNamed(styleSheet, "List");
    tags[MENU_TAG].style =	HTStyleNamed(styleSheet, "Menu");
    list_compact_style =
    tags[DIR_TAG].style =	HTStyleNamed(styleSheet, "Dir");    
    glossary_style =		HTStyleNamed(styleSheet, "Glossary");
    glossary_compact_style =	HTStyleNamed(styleSheet, "GlossaryCompact");
    tags[ADDRESS_TAG].style=	HTStyleNamed(styleSheet, "Address");
    tags[PLAINTEXT_TAG].style =
    tags[XMP_TAG].style =	HTStyleNamed(styleSheet, "Example");
    tags[LISTING_TAG].style =	HTStyleNamed(styleSheet, "Listing");
}


/*	Parse an HTML file
**	------------------
**
**	This version takes a pointer to the routine to call
**	to get each character.
*/
bool HTML_Parse
  (HTParentAnchor * anchor, char (*next_char)(void) )
{
    HTML_begin(anchor);
    SGML_begin(&HTML_dtd);
    for(;;) {
	char character;
	character = (*next_char)();
	if (character == (char)EOF) break;
	
	SGML_character(&HTML_dtd, character);
    }
    SGML_end(&HTML_dtd);
    return true;
}

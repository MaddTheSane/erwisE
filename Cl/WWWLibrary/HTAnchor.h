/*	Hypertext "Anchor" Object				     HTAnchor.h
**	==========================
**
**	An anchor represents a region of a hypertext document which is linked
**	to another anchor in the same or a different document.
*/

#ifndef HTANCHOR_H
#define HTANCHOR_H

/* Version 0 (TBL) written in Objective-C for the NeXT browser */
/* Version 1 of 24-Oct-1991 (JFG), written in C, browser-independant */

#include "HTList.h"
#include "HTAtom.h"

#ifdef SHORT_NAMES
#define HTAnchor_findChild			HTAnFiCh
#define HTAnchor_findChildAndLink		HTAnFiLi
#define HTAnchor_findAddress			HTAnFiAd
#define HTAnchor_delete				HTAnDele
#define HTAnchor_makeLastChild			HTAnMaLa
#define HTAnchor_parent				HTAnPare
#define HTAnchor_setDocument			HTAnSeDo
#define HTAnchor_document			HTAnDocu
#define HTAnchor_setFormat			HTAnSeFo
#define HTAnchor_format				HTAnForm
#define HTAnchor_setIndex			HTAnSeIn
#define HTAnchor_isIndex			HTAnIsIn
#define HTAnchor_address			HTAnAddr
#define HTAnchor_hasChildren			HTAnHaCh
#define HTAnchor_title				HTAnTitl
#define HTAnchor_setTitle			HTAnSeTi
#define HTAnchor_appendTitle			HTAnApTi
#define HTAnchor_link				HTAnLink
#define HTAnchor_followMainLink			HTAnFoMa
#define HTAnchor_followTypedLink		HTAnFoTy
#define HTAnchor_makeMainLink			HTAnMaMa
#endif

/*			Main definition of anchor
**			=========================
*/

typedef struct _HyperDoc HyperDoc;  /* Ready for forward references */
typedef struct _HTAnchor HTAnchor;
typedef struct _HTParentAnchor HTParentAnchor;

#include "HTFormat.h"

typedef HTAtom HTLinkType;

typedef struct {
  HTAnchor *	dest;		/* The anchor to which this leads */
  HTLinkType *	type;		/* Semantics of this link */
} HTLink;

struct _HTAnchor {		/* Generic anchor : just links */
  HTLink	mainLink;	/* Main (or default) destination of this */
  HTList *	links;  	/* List of extra links from this, if any */
  /* We separate the first link from the others to avoid too many small mallocs
     involved by a list creation. Most anchors only point to one place. */
  HTParentAnchor * parent;	/* Parent of this anchor (self for adults) */
};

struct _HTParentAnchor {
  /* Common part from the generic anchor structure */
  HTLink	mainLink;	/* Main (or default) destination of this */
  HTList *	links;  	/* List of extra links from this, if any */
  HTParentAnchor * parent;	/* Parent of this anchor (self) */

  /* ParentAnchor-specific information */
  HTList *	children;	/* Subanchors of this, if any */
  HTList *	sources;	/* List of anchors pointing to this, if any */
  HyperDoc *	document;	/* The document within which this is an anchor */
  char * 	address;	/* Absolute address of this node */
  HTFormat *	format; 	/* Pointer to node format descriptor */
  bool		isIndex;	/* Acceptance of a keyword search */
  char *	title;		/* Title of document */
};

typedef struct HTChildAnchor {
  /* Common part from the generic anchor structure */
  HTLink	mainLink;	/*!< Main (or default) destination of this */
  HTList *	links;  	/*!< List of extra links from this, if any */
  HTParentAnchor * parent;	/*!< Parent of this anchor */

  /* ChildAnchor-specific information */
  char * 	tag;		/*!< Address of this anchor relative to parent */
} HTChildAnchor;


/*!
 * \brief Create new or find old sub-anchor
 *
 * This one is for a new anchor being edited into an existing
 * document. The parent anchor must already exist.
 */
extern HTChildAnchor * HTAnchor_findChild(HTParentAnchor *parent,
					  const char *tag);

/*	Create or find a child anchor with a possible link
**	--------------------------------------------------
**
**	Create new anchor with a given parent and possibly
**	a name, and possibly a link to a _relatively_ named anchor.
**	(Code originally in ParseHTML.h)
*/
extern HTChildAnchor * HTAnchor_findChildAndLink(
						 HTParentAnchor * parent,	/* May not be 0 */
       const char * tag,         /* May be "" or 0 */
       const char * href,	/* May be "" or 0 */
       HTLinkType * ltype	/* May be 0 */
);


/*	Create new or find old named anchor
**	-----------------------------------
**
**	This one is for a reference which is found in a document, and might
**	not be already loaded.
**	Note: You are not guaranteed a new anchor -- you might get an old one,
**	like with fonts.
*/

extern HTAnchor * HTAnchor_findAddress(const char * address);


/*	Delete an anchor and possibly related things (auto garbage collection)
**	--------------------------------------------
**
**	The anchor is only deleted if the corresponding document is not loaded.
**	All outgoing links from parent and children are deleted, and this anchor
**	is removed from the sources list of all its targets.
**	We also try to delete the targets whose documents are not loaded.
**	If this anchor's source list is empty, we delete it and its children.
*/

extern bool HTAnchor_delete(HTParentAnchor *this);


/*!
 * \brief Move an anchor to the head of the list of its siblings
 *
 *
 * This is to ensure that an anchor which might have already existed
 * is put in the correct order as we load the document.
 */
extern void HTAnchor_makeLastChild(HTChildAnchor *this);

#pragma mark - Data access functions

extern HTParentAnchor * HTAnchor_parent(HTAnchor *this);

extern void HTAnchor_setDocument(HTParentAnchor *this, HyperDoc *doc);

extern HyperDoc * HTAnchor_document(HTParentAnchor *this);
/* We don't want code to change an address after anchor creation... yet ?
extern void HTAnchor_setAddress
  PARAMS(
     (HTAnchor *this, char *addr)
     );
*/

extern char * HTAnchor_address(HTAnchor *this);

extern void HTAnchor_setFormat(HTParentAnchor *this, HTFormat *form);

extern HTFormat * HTAnchor_format(HTParentAnchor *this);

extern void HTAnchor_setIndex(HTParentAnchor *this);

extern bool HTAnchor_isIndex(HTParentAnchor *this);

extern bool HTAnchor_hasChildren(HTParentAnchor *this);

#pragma mark -Title handling
extern const char * HTAnchor_title(HTParentAnchor *this);

extern void HTAnchor_setTitle(HTParentAnchor *this, const char * title);

extern void HTAnchor_appendTitle(HTParentAnchor *this, const char * title);

/*!
 * Link this Anchor to another given one
 */
extern bool HTAnchor_link(HTAnchor *source, HTAnchor *destination, HTLinkType *type);

#pragma mark - Manipulation of links

extern HTAnchor * HTAnchor_followMainLink(HTAnchor *this);

extern HTAnchor * HTAnchor_followTypedLink(HTAnchor *this, HTLinkType *type);

extern bool HTAnchor_makeMainLink(HTAnchor *this, HTLink *movingLink);

#endif /* HTANCHOR_H */

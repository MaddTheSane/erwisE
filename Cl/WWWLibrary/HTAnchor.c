/*	Hypertext "Anchor" Object				HTAnchor.c
**	==========================
**
** An anchor represents a region of a hypertext document which is linked to
** another anchor in the same or a different document.
**
** History
**
**         Nov 1990  Written in Objective-C for the NeXT browser (TBL)
**	24-Oct-1991 (JFG), written in C, browser-independant 
**	21-Nov-1991 (JFG), first complete version
**
**	(c) Copyright CERN 1991 - See Copyright.html
*/

#define HASH_SIZE 101		/* Arbitrary prime. Memory/speed tradeoff */

#include <ctype.h>
#include "tcp.h"
#include "HTAnchor.h"
#include "HTUtils.h"
#include "HTParse.h"

typedef struct _HyperDoc Hyperdoc;
#ifdef vms
struct _HyperDoc {
	int junk;	/* VMS cannot handle pointers to undefined structs */
};
#endif

PRIVATE HTList **adult_table=0;  /* Point to table of lists of all parents */

/*				Creation Methods
**				================
**
**	Do not use "new" by itself outside this module. In order to enforce
**	consistency, we insist that you furnish more information about the
**	anchor you are creating : use newWithParent or newWithAddress.
*/

PRIVATE HTParentAnchor * HTParentAnchor_new(void)
{
  HTParentAnchor *newAnchor = 
    (HTParentAnchor *) calloc (1, sizeof (HTParentAnchor));  /* zero-filled */
  newAnchor->parent = newAnchor;
  return newAnchor;
}

PRIVATE HTChildAnchor * HTChildAnchor_new(void)
{
  return (HTChildAnchor *) calloc (1, sizeof (HTChildAnchor));  /* zero-filled */
}


/*	Case insensitive string comparison
**	----------------------------------
** On entry,
**	s	Points to one string, null terminated
**	t	points to the other.
** On exit,
**	returns	true if the strings are equivalent ignoring case
**		false if they differ in more than  their case.
*/

PRIVATE bool equivalent(const char *s, const char *t)
{
  if (s && t) {  /* Make sure they point to something */
    for ( ; *s && *t ; s++, t++) {
        if (TOUPPER(*s) != TOUPPER(*t))
	  return false;
    }
    return TOUPPER(*s) == TOUPPER(*t);
  } else
    return s == t;  /* Two NULLs are equivalent, aren't they ? */
}


/*	Create new or find old sub-anchor
**	---------------------------------
**
**	This one is for a new anchor being edited into an existing
**	document. The parent anchor must already exist.
*/

HTChildAnchor * HTAnchor_findChild(HTParentAnchor *parent, const char *tag)
{
  HTChildAnchor *child;
  HTList *kids;

  if (! parent) {
    if (TRACE) printf ("HTAnchor_findChild called with NULL parent.\n");
    return NULL;
  }
    if ((kids = parent->children)) {  /* parent has children : search them */
    if (tag && *tag) {		/* TBL */
	while ((child = HTList_nextObject (kids))) {
	    if (equivalent(child->tag, tag)) { /* Case sensitive 920226 */
		if (TRACE) printf (
	       "Child anchor %p of parent %p with name `%s' already exists.\n",
		    child, parent, tag);
		return child;
	    }
	}
     }  /*  end if tag is void */
  } else  /* parent doesn't have any children yet : create family */
    parent->children = HTList_new ();

  child = HTChildAnchor_new ();
  if (TRACE) printf("new Anchor %p named `%s' is child of %p\n",
                  child, tag != NULL ? tag : (const char *)"" , parent); /* int for apollo */
  HTList_addObject (parent->children, child);
  child->parent = parent;
  StrAllocCopy(child->tag, tag);
  return child;
}


/*	Create or find a child anchor with a possible link
**	--------------------------------------------------
**
**	Create new anchor with a given parent and possibly
**	a name, and possibly a link to a _relatively_ named anchor.
**	(Code originally in ParseHTML.h)
*/
PUBLIC HTChildAnchor * HTAnchor_findChildAndLink(
       HTParentAnchor *parent,	/* May not be 0 */
       const char *tag,	/* May be "" or 0 */
       const char *href,	/* May be "" or 0 */
       HTLinkType *ltype	/* May be 0 */
       )
{
  HTChildAnchor * child = HTAnchor_findChild(parent, tag);
  if (href && *href) {
    char * parsed_address;
    HTAnchor * dest;
    parsed_address = HTParse(href, HTAnchor_address((HTAnchor *) parent),
			     PARSE_ALL);
    dest = HTAnchor_findAddress(parsed_address);
    HTAnchor_link((HTAnchor *) child, dest, ltype);
    free(parsed_address);
  }
  return child;
}


/*	Create new or find old named anchor
**	-----------------------------------
**
**	This one is for a reference which is found in a document, and might
**	not be already loaded.
**	Note: You are not guaranteed a new anchor -- you might get an old one,
**	like with fonts.
*/

HTAnchor * HTAnchor_findAddress(const char *address)
{
  char *tag = HTParse (address, "", PARSE_ANCHOR);  /* Anchor tag specified ? */

  /* If the address represents a sub-anchor, we recursively load its parent,
     then we create a child anchor within that document. */
  if (*tag) {
    char *docAddress = HTParse(address, "", PARSE_ACCESS | PARSE_HOST |
			                    PARSE_PATH | PARSE_PUNCTUATION);
    HTParentAnchor * foundParent =
      (HTParentAnchor *) HTAnchor_findAddress (docAddress);
    HTChildAnchor * foundAnchor = HTAnchor_findChild (foundParent, tag);
    free (docAddress);
    free (tag);
    return (HTAnchor *) foundAnchor;
  } else { /* If the address has no anchor tag, check whether we have this node */
    int hash;
    const char *p;
    HTList * adults;
    HTList *grownups;
    HTParentAnchor * foundAnchor;

    free (tag);
    
    /* Select list from hash table */
    for(p=address, hash=0; *p; p++) hash = (hash * 3 + *p) % HASH_SIZE;
    if (!adult_table)
        adult_table = (HTList**) calloc(HASH_SIZE, sizeof(HTList*));
    if (!adult_table[hash]) adult_table[hash] = HTList_new();
    adults = adult_table[hash];

    /* Search list for anchor */
    grownups = adults;
    while ((foundAnchor = HTList_nextObject (grownups))) {
      if (equivalent(foundAnchor->address, address)) {
        if (TRACE) printf("Anchor %p with address `%s' already exists.\n",
                          foundAnchor, address);
        return (HTAnchor *) foundAnchor;
      }
    }
    
    /* Node not found : create new anchor */
    foundAnchor = HTParentAnchor_new ();
    if (TRACE) printf("New anchor %p has hash %d and address `%s'\n",
    	foundAnchor, hash, address);
    StrAllocCopy(foundAnchor->address, address);
    HTList_addObject (adults, foundAnchor);
    return (HTAnchor *) foundAnchor;
  }
}


/*	Delete an anchor and possibly related things (auto garbage collection)
**	--------------------------------------------
**
**	The anchor is only deleted if the corresponding document is not loaded.
**	All outgoing links from parent and children are deleted, and this anchor
**	is removed from the sources list of all its targets.
**	We also try to delete the targets whose documents are not loaded.
**	If this anchor's source list is empty, we delete it and its children.
*/

PRIVATE void deleteLinks(HTAnchor *this)
{
  if (! this)
    return;

  /* Recursively try to delete target anchors */
  if (this->mainLink.dest) {
    HTParentAnchor *parent = this->mainLink.dest->parent;
    HTList_removeObject (parent->sources, this);
    if (! parent->document)  /* Test here to avoid calling overhead */
      HTAnchor_delete (parent);
  }
  if (this->links) {  /* Extra destinations */
    HTLink *target;
    while ((target = HTList_removeLastObject (this->links))) {
      HTParentAnchor *parent = target->dest->parent;
      HTList_removeObject (parent->sources, this);
      if (! parent->document) { /* Test here to avoid calling overhead */
        HTAnchor_delete (parent);
      }
    }
  }
}

PUBLIC bool HTAnchor_delete(HTParentAnchor *this)
{
  HTChildAnchor *child;

  /* Don't delete if document is loaded */
  if (this->document) {
    return false;
  }

  /* Recursively try to delete target anchors */
  deleteLinks ((HTAnchor *) this);

  if (! HTList_isEmpty (this->sources)) {  /* There are still incoming links */
    /* Delete all outgoing links from children, if any */
    HTList *kids = this->children;
    while ((child = HTList_nextObject (kids))) {
      deleteLinks ((HTAnchor *) child);
    }
    return false;  /* Parent not deleted */
  }

  /* No more incoming links : kill everything */
  /* First, recursively delete children */
  while ((child = HTList_removeLastObject (this->children))) {
    deleteLinks ((HTAnchor *) child);
    free (child->tag);
    free (child);
  }

  /* Now kill myself */
  HTList_delete (this->children);
  HTList_delete (this->sources);
  free (this->address);
  /* Devise a way to clean out the HTFormat if no longer needed (ref count?) */
  free (this);
  return true;  /* Parent deleted */
}


/*!
 * \brief Move an anchor to the head of the list of its siblings
 *
 * This is to ensure that an anchor which might have already existed
 * is put in the correct order as we load the document.
 */
void HTAnchor_makeLastChild(HTChildAnchor *this)
{
  if (this->parent != (HTParentAnchor *) this) {  /* Make sure it's a child */
    HTList * siblings = this->parent->children;
    HTList_removeObject (siblings, this);
    HTList_addObject (siblings, this);
  }
}

/*	Data access functions
**	---------------------
*/

extern HTParentAnchor * HTAnchor_parent(HTAnchor *this)
{
  return this ? this->parent : NULL;
}

void HTAnchor_setDocument(HTParentAnchor *this, HyperDoc *doc)
{
  if (this)
    this->document = doc;
}

HyperDoc * HTAnchor_document(HTParentAnchor *this)
{
  return this ? this->document : NULL;
}


/* We don't want code to change an address after anchor creation... yet ?
void HTAnchor_setAddress
  ARGS2 (HTAnchor *,this, char *,addr)
{
  if (this)
    StrAllocCopy (this->parent->address, addr);
}
*/

char * HTAnchor_address(HTAnchor *this)
{
  char *addr = NULL;
  if (this) {
    if (((HTParentAnchor *) this == this->parent) ||
    	!((HTChildAnchor *) this)->tag) {  /* it's an adult or no tag */
      StrAllocCopy (addr, this->parent->address);
    }
    else {  /* it's a named child */
      addr = malloc (2 + strlen (this->parent->address)
		     + strlen (((HTChildAnchor *) this)->tag));
      if (addr == NULL) outofmem(__FILE__, "HTAnchor_address");
      sprintf (addr, "%s#%s", this->parent->address,
	       ((HTChildAnchor *) this)->tag);
    }
  }
  return addr;
}



void HTAnchor_setFormat(HTParentAnchor *this, HTFormat *form)
{
  if (this)
    this->format = form;
}

HTFormat * HTAnchor_format(HTParentAnchor *this)
{
  return this ? this->format : NULL;
}



void HTAnchor_setIndex(HTParentAnchor *this)
{
  if (this)
    this->isIndex = true;
}

bool HTAnchor_isIndex(HTParentAnchor *this)
{
  return this ? this->isIndex : false;
}



bool HTAnchor_hasChildren(HTParentAnchor *this)
{
  return this ? ! HTList_isEmpty(this->children) : false;
}

/*	Title handling
*/
const char * HTAnchor_title(HTParentAnchor *this)
{
  return this ? this->title : 0;
}

void HTAnchor_setTitle(HTParentAnchor *this, const char *title)
{
  StrAllocCopy(this->title, title);
}

void HTAnchor_appendTitle(HTParentAnchor *this, const char *title)
{
  StrAllocCat(this->title, title);
}

/*	Link this Anchor to another given one
**	-------------------------------------
*/

bool HTAnchor_link(HTAnchor *source, HTAnchor *destination, HTLinkType *type)
{
  if (! (source && destination))
    return false;  /* Can't link to/from non-existing anchor */
  if (TRACE) printf ("Linking anchor %p to anchor %p\n", source, destination);
  if (! source->mainLink.dest) {
    source->mainLink.dest = destination;
    source->mainLink.type = type;
  } else {
    HTLink * newLink = (HTLink *) malloc (sizeof (HTLink));
    if (newLink == NULL) outofmem(__FILE__, "HTAnchor_link");
    newLink->dest = destination;
    newLink->type = type;
    if (! source->links)
      source->links = HTList_new ();
    HTList_addObject (source->links, newLink);
  }
  if (!destination->parent->sources)
    destination->parent->sources = HTList_new ();
  HTList_addObject (destination->parent->sources, source);
  return true;  /* Success */
}


/*	Manipulation of links
**	---------------------
*/

HTAnchor * HTAnchor_followMainLink(HTAnchor *this)
{
  return this->mainLink.dest;
}

HTAnchor * HTAnchor_followTypedLink(HTAnchor *this, HTLinkType *type)
{
  if (this->mainLink.type == type)
    return this->mainLink.dest;
  if (this->links) {
    HTList *links = this->links;
    HTLink *link;
    while ((link = HTList_nextObject (links)))
      if (link->type == type)
	return link->dest;
  }
  return NULL;  /* No link of this type */
}

bool HTAnchor_makeMainLink(HTAnchor *this, HTLink *movingLink)
{
  /* Check that everything's OK */
  if (! (this && HTList_removeObject (this->links, movingLink)))
    return false;  /* link not found or NULL anchor */
  else {
    /* First push current main link onto top of links list */
    HTLink *newLink = (HTLink*) malloc (sizeof (HTLink));
    if (newLink == NULL) outofmem(__FILE__, "HTAnchor_makeMainLink");
    memcpy (newLink, & this->mainLink, sizeof (HTLink));
    HTList_addObject (this->links, newLink);

    /* Now make movingLink the new main link, and free it */
    memcpy (& this->mainLink, movingLink, sizeof (HTLink));
    free (movingLink);
    return true;
  }
}

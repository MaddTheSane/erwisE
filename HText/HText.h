/*
 * HText.h --
 *
 * Author: Teemu Rantanen <tvr@cs.hut.fi>
 * Copyright (c) 1992 Teemu Rantanen
 *                    All rights reserved
 *
 * Created: Wed Feb 26 15:57:03 1992 tvr
 * Last modified: Mon Apr 27 23:55:47 1992 tvr
 *
 */

#ifndef __HText_HText_h__
#define __HText_HText_h__

#include "HTAnchor.h"
#include "HTStyle.h"

/*!
 * Maximum Htext object data size
 */
#define HTEXT_MAX_OBJECT_SIZE	1024

/*!
 * Region changes on hypertext
 */
typedef enum HTextType: int {
    HTEXT_PARAGRAPH = 1,
    HTEXT_TAB,
    HTEXT_NEWLINE,
    HTEXT_CONTINUE
} HTextType;

/*!
 * Hypertext object structure.
 */
typedef struct HTextObject
{
  /*!
   * Pointers to next and previous objects
   */

  struct HTextObject *prev;
  struct HTextObject *next;

  /*
   * All Hypertext specific fields here
   */

  /*!
   * If object is an anchor, have anchordata here
   */
  HTChildAnchor *anchor;

  /*!
   * Style of this object.
   */
  HTStyle *style;

  /*!
   * what data on this object
   */
  char *data;

  /*!
   * How many bytes data
   */
  int length;

  /*!
   * Marks paragraph change at the end of this object
   */
    HTextType paragraph;

  /*
   * All Xl specific data here
   */

  /*!
   * Position of an object on a virtual screen. These will be set when
   * object is being positioned.
   */
  int x;
  int y;

  /*!
   * Size of an object. These are calculated once (as these does
   * not change).
   */
  int width;
  int height;

  /*!
   * Xl specific data of this object
   */
  struct XlObjectData *xl_data;
} HTextObject_t;


/*!
 * Hypertext structure of a page
 */
typedef struct HText
{
  /*!
   * First and last hypertext objects
   */
  struct HTextObject *first;
  struct HTextObject *last;

  /*!
   * Node anchor of this page
   */
  HTParentAnchor *node_anchor;

  /*!
   * Xl specific global data for a page
   */
  struct XlGlobalData *xl_global;

  /*!
   * HREF pointer list
   */
  struct HTextAnchor *anchorlist;

  /*!
   * Cursor object of this page
   */
  struct HTextObject *cursor;

} HText_t;


/*!
 * HREF pointer list on a page
 */
typedef struct HTextAnchor
{
  /*!
   * Pointer to next
   */
  struct HTextAnchor *next;

  /*!
   * Anchor data
   */
  HTChildAnchor *anchor;

  /*!
   * To which hypertextobject is this connected to
   */
  struct HTextObject *object;
} HTextAnchor_t;




#pragma mark Prototype


HText_t *HtDuplicate (HText_t * text);
extern char *HTSearchAddress(char *keywords);

#endif

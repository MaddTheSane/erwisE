#ifndef HTHISTORY_H
#define HTHISTORY_H

#include "HTAnchor.h"

#ifdef SHORT_NAMES
#define HTHistory_record		HTHiReco
#define HTHistory_backtrack		HTHiBack
#define HTHistory_canBacktrack		HTHiCaBa
#define HTHistory_moveBy		HTHiMoBy
#define HTHistory_canMoveBy		HTHiCaMo
#define HTHistory_read			HTHiRead
#define HTHistory_recall		HTHiReca
#define HTHistory_count			HTHiCoun
#define HTHistory_leavingFrom		HTHiLeFr
#endif

/*				Navigation
**				==========
*/

/*		Record the jump to an anchor
**		----------------------------
*/

extern void HTHistory_record(HTAnchor * destination);

/*		Go back in history (find the last visited node)
**		------------------
*/

extern HTAnchor * HTHistory_backtrack(void);
  /* FIXME: Should we add a `sticky' option ? */

extern bool HTHistory_canBacktrack(void);

/*		Browse through references in the same parent node
**		-------------------------------------------------
**
**	Take the n-th child's link after or before the one we took to get here.
**	Positive offset means go towards most recently added children.
*/

extern HTAnchor * HTHistory_moveBy(int offset);

extern bool HTHistory_canMoveBy(int offset);

#define HTHistory_next (HTHistory_moveBy (+1))
#define HTHistory_canNext (HTHistory_canMoveBy (+1))
#define HTHistory_previous (HTHistory_moveBy (-1))
#define HTHistory_canPrevious (HTHistory_canMoveBy (-1))


/*				Retrieval
**				=========
*/

/*		Read numbered visited anchor (1 is the oldest)
**		----------------------------
*/

extern HTAnchor * HTHistory_read(int number);

/*		Recall numbered visited anchor (1 is the oldest)
**		------------------------------
**	This reads the anchor and stores it again in the list, except if last.
*/

extern HTAnchor * HTHistory_recall(int number);

/*		Number of Anchors stored
**		------------------------
**
**	This is needed in order to check the validity of certain commands
**	for menus, etc.
(not needed for now. Use canBacktrack, etc.)
extern int HTHistory_count(void);
*/

/*		Change last history entry
**		-------------------------
**
**	Sometimes we load a node by one anchor but leave by a different
**	one, and it is the one we left from which we want to remember.
*/
extern void HTHistory_leavingFrom(HTAnchor * anchor);

#endif /* HTHISTORY_H */

/*			File Access				HTFile.h
**			===========
**
**	These are routines for file access used by WWW browsers.
**
*/

#include "HTFormat.h"

/*	Convert filenames between local and WWW formats
**	-----------------------------------------------
**	Make up a suitable name for saving the node in
**
**	E.g.	$(HOME)/WWW/news/1234@cernvax.cern.ch
**		$(HOME)/WWW/http/crnvmc/FIND/xx.xxx.xx
*/
extern char * HTLocalName(const char * name);

/*	Make a WWW name from a full local path name
**
*/
extern char * WWW_nameOfFile(const char * name);


/*	Generate the name of a cache file
*/

extern char * HTCacheFileName(const char * name);


/*	Determine file format from file name
**	------------------------------------
*/

extern int HTFileFormat(const char * filename);


/*	Determine write access to a file
//	--------------------------------
//
// On exit,
//	return value	true if file can be accessed and can be written to.
//
//	Isn't there a quicker way?
*/

extern bool HTEditable(const char * filename);


/*	Open a file descriptor for a document
**	-------------------------------------
**
** On entry,
**	addr		must point to the fully qualified hypertext reference.
**
** On exit,
**	returns		<0	Error has occured.
**			>=0	Value of file descriptor or socket to be used
**				 to read data.
**	*pFormat	Set to the format of the file, if known.
**			(See WWW.h)
**
*/
extern int HTOpenFile(
  const char * addr,
  HTFormat * pFormat,
  HTParentAnchor * anchor
);


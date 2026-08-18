/*		Access Manager					HTAccess.h
**		==============
*/

#ifndef HTACCESS_H
#define HTACCESS_H

/*	Flag to allow source to be loaded as text
*/
extern int HTDiag;

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
** No longer public -- only used internally.
*/
/* extern int HTOpen PARAMS((CONST char * addr, HTFormat * format)); */


/*	Close socket opened for reading a file
**	--------------------------------------
**
*/
extern int HTClose(int soc);


/*		Load a document
**		---------------
**
**    On Entry,
**	  anchor	    is the node_anchor for the document
**        full_address      The address of the file to be accessed.
**
**    On Exit,
**        returns    true     Success in opening file
**                   false      Failure 
**
*/

extern bool HTLoadDocument(HTParentAnchor * anchor,
			   const char * full_address,
			   bool	filter);



/*!
 * \brief Load a document from relative name
 *
 * \param relative_name     The relative address of the file to be accessed.
 * \returns
 * \c true Success in opening file,
 * \c false Failure
 */
extern  bool HTLoadRelative(const char * relative_name);

/*		Load a document from relative name
**		---------------
**
**    On Entry,
**        relative_name     The relative address of the file to be accessed.
**
**    On Exit,
**        returns    true     Success in opening file
**                   false      Failure 
**
**
*/

extern bool HTLoadAbsolute(const char * addr, bool filter);


/*		Load if necessary, and select an anchor
**		--------------------------------------
**
**    On Entry,
**        destination      	    The child or parenet anchor to be loaded.
**
**    On Exit,
**        returns    true     Success
**                   false      Failure 
**
*/

extern bool HTLoadAnchor(HTAnchor * destination);

/*		Search
**		------
**  Performs a keyword search on word given by the user. Adds the keyword to 
**  the end of the current address and attempts to open the new address.
**
**  On Entry,
**       *keywords  	space-separated keyword list or similar search list
**	HTMainAnchor	global must be valid.
*/

extern bool HTSearch(char * keywords);


#endif /* HTACCESS_H */

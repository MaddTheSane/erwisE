/*	HyperText Tranfer Protocol					HTTP.h
**	==========================
*/

#ifndef HTTP_H
#define HTTP_H

#include "HTAnchor.h"

/*		Open Socket for reading from HTTP Server	HTTP_get()
**		========================================
**
**	Given a hypertext address, this routine opens a socket to the server,
**	sends the "get" command to ask for the node, and then returns the
**	socket to the caller. The socket must later be closed by the caller.
**
** On entry,
**	arg	is the hypertext reference of the article to be loaded.
** On exit,
**	returns	>=0	If no error, a good socket number
**		<0	Error.
**
*/
extern int HTTP_Get(const char * arg);

/*	Load Document using HTTP
**	------------------------
*/

extern int HTLoadHTTP (const char *arg,
		       const char * gateway,
		       HTParentAnchor * anAnchor,
		       int diag);

#endif /* HTTP_H */

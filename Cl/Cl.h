/*
 * Cl.h --
 *
 * Author: Teemu Rantanen <tvr@cs.hut.fi>
 * Copyright (c) 1992 Teemu Rantanen
 *                    All rights reserved
 *
 * Created: Thu Apr 16 21:40:39 1992 tvr
 * Last modified: Wed May 13 00:11:13 1992 tvr
 *
 */

#include <sys/socket.h>
#include "HTAnchor.h"
#include "HTStyle.h"
#include "HText.h"

/*!
 * Data is store on linked list
 */
typedef struct cl_data_s
{
  struct cl_data_s *next;
  struct cl_data_s *prev;
  void *data;
  void *freeptr;
  int size;
}         cl_data_t;


/*!
 * This structure contains all information needed to accomplish this
 * connection.
 */
typedef struct ClConnection_s
{
  /*!
   * address of this connection
   */
  char *address;

  /*!
   * Fd of the connection. Also ftp connection data channel.
   */
  int fd;
  int secondary_fd;

  /*!
   * If this fd is not set connection has to be polled. If it is NOT
   * set, connection has to be selected (must not poll!)
   */
  int select_fd;

  /*!
   * Function of this state machine
   */
  void (**function) (void);

  /*!
   * Store junk here (address of data on connect() )
   */
  void *addr;
  int addr_size;

  /*!
   * Store command to write to the net here
   */
  char *command;

  /*!
   * Data Buffer
   */
  cl_data_t *buffer_first;
  cl_data_t *buffer_last;

  /*!
   * stuff given to HTLoadHTTP()
   */
  int diag;
  struct HTAnchor *anAnchor;

  /*!
   * How happened with load if on nonblocking mode
   */
  int status;

  /*!
   * Hostname on ftp connections
   */
  char *ftphost;

  /*
   * Data port on ftp connections
   */
  int port;


  /*!
   * Load to file ?
   */
  int load_to_file;
  int load_to_file_fd;

}              ClConnection_t;


/*!
 * More data is to be read
 */
#define CL_CONTINUES		0

/*!
 * Connection completed
 */
#define CL_COMPLETED		1

/*!
 * Connection failed
 */
#define CL_FAILED		2

/*!
 * Document already loaded (error)
 */
#define CL_ALREADY_LOADED	3



#pragma mark Prototypes -

ClConnection_t *ClOpenConnection (const char *address);

struct HText *ClReadData (ClConnection_t * connection, int *how_done, int *fd);

void ClSetOptions (ClConnection_t * connection, int argc, char *argv[]);

void ClCloseConnection (ClConnection_t * connection);

int ClCanLoadToFile (const char *address);

void ClSetFileNameForLoadingToFile (const char *filename);

int ClConnectionOnLoadToFileMode (ClConnection_t * connection);


#pragma mark Globals -


extern int WWWErwiseStatus;
extern ClConnection_t *WWWErwiseConnection;

void WWWErwiseConnect (void);
void WWWErwiseSendCommand (void);
void WWWErwiseReadData (void);
void WWWErwiseParse (void);



#pragma mark Internals -


void WWWErwiseFtpUser (void);
void WWWErwiseFtpPass (void);
void WWWErwiseFtpPassive (void);
void WWWErwiseFtpGetPassive (void);
void WWWErwiseFtpGetCommand (void);
void WWWErwiseFtpDataChannel (void);
void WWWErwiseFtpCheckForError (void);
void WWWErwiseFtpBinary (void);
void WWWErwiseCheckParse (void);
void WWWErwiseSetPoll (void);
void WWWErwiseSetSelect (void);
void WWWErwiseTerminateIfLoadToFile (void);

int cl_start_connection (char *host, ClConnection_t *connection, int port);
int erwise_connect (int fd, struct sockaddr *addr, int size);


#pragma mark Debug ... -


#define CL_DEBUG(a)    printf a

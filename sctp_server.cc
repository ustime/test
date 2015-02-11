#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>

#define MAX_BUFFER 1024
#define MY_PORT_NUM 10000
#define LOCALTIME_STREAM 0
#define GMT_STREAM 1

int main()
{
  int listenSock, connSock, ret;
  struct sockaddr_in servaddr;
  char buffer[MAX_BUFFER+1];
  time_t currentTime;

  /* Create SCTP TCP-Style Socket */
  listenSock = socket( AF_INET, SOCK_STREAM, IPPROTO_SCTP );

  /* Accept connections from any interface */
  bzero( (void *)&servaddr, sizeof(servaddr) );
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl( INADDR_ANY );
  servaddr.sin_port = htons(MY_PORT_NUM);

  /* Bind to the wildcard address (all) and MY_PORT_NUM */
  ret = bind( listenSock,
               (struct sockaddr *)&servaddr, sizeof(servaddr) );

  /* Place the server socket into the listening state */
  listen( listenSock, 5 );

  /* Server loop... */
  while( 1 ) {

    /* Await a new client connection */
    struct sockaddr_in sa;
    socklen_t socklen = sizeof (sockaddr_in);
    connSock = accept( listenSock,
                        (struct sockaddr *)&sa, (socklen_t *)&socklen );

    /* New client socket has connected */

    /* Grab the current time */
    currentTime = time(NULL);

    /* Send local time on stream 0 (local time stream) */
    snprintf( buffer, MAX_BUFFER, "%s\n", ctime(&currentTime) );

    ret = sctp_sendmsg( connSock,
                          (void *)buffer, (size_t)strlen(buffer),
                          NULL, 0, 0, 0, LOCALTIME_STREAM, 0, 0 );

    /* Send GMT on stream 1 (GMT stream) */
    snprintf( buffer, MAX_BUFFER, "%s\n",
               asctime( gmtime( &currentTime ) ) );

    ret = sctp_sendmsg( connSock,
                          (void *)buffer, (size_t)strlen(buffer),
                          NULL, 0, 0, 0, GMT_STREAM, 0, 0 );

    /* Close the client connection */
    close( connSock );

  }

  return 0;
}

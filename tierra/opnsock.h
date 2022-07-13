#ifndef OPNSOCK_H
#define OPNSOCK_H

#ifndef PEXTERN
#define PEXTERN extern
#endif /* PEXTERN */

#define TCPNAME        "tcp"
#define UDPMTU         1500
#define OPEN_MAX        256
#define RETRYSLEEP       10
#define STATUSINTERVAL    1

#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */

#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#ifdef HAVE_SYS_TIME_H
#include <time.h>
#endif /* HAVE_SYS_TIME_H */

#ifdef ANSI     /* ANSI prototyping */
#define P_(A) A
#else /* non-ANSI prototyping */
#define P_(A) ()
#endif

int main P_((int argc,char **argv));
PEXTERN int opnsock P_((int socktyp,int bindflg,int port));
PEXTERN int udp2tcp P_((int udpskt,fd_set *readfds,
    int *incudpcnt,int tcpskt,char *buf));
PEXTERN int tcp2udp P_((int tcpskt,fd_set *readfds,int *inctcpcnt,
    int udpskt,struct sockaddr_in *remudpadr,char *buf));
PEXTERN void statusdisplay P_((time_t *prvupdtme,
    int incudpcnt,int *prvincudpcnt,int inctcpcnt,
    int *prvinctcpcnt,int updcntmsg));

#endif /* OPNSOCK_H */

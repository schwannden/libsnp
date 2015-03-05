#ifndef __NP_LIB_H
#define __NP_LIB_H
#include "np_header.h"

#ifndef MAXSOCKADDRSIZE
#define MAXSOCKADDRSIZE 128
#endif

#ifndef MAXLINE
#define MAXLINE 1024
#endif

#ifndef SERV_PORT
#define SERV_PORT 9987
#endif

#ifndef BUFF_SIZE
#define BUFF_SIZE 1024
#endif

#ifndef LISTEN_Q
#define LISTEN_Q 1024
#endif

typedef struct sockaddr SA;

typedef enum BOOL {false=0, true=1} bool;

const char*
Inet_ntop( int family, const void* sa, char* str, socklen_t socklen );

void
Inet_pton(int family, const char *strptr, void *addrptr);

//returns a representation string for sa. The string is staticalled allocated in sock_ntop with length MAXSOCKADDRSIZE.
char*
sock_ntop( const SA* sa, socklen_t socklen );

ssize_t
readn( int fd, void* buf, size_t n );

ssize_t
Readn( int fd, void* buf, size_t n );

ssize_t
writen( int fd, const void* vptr, size_t n );

void
Writen( int fd, const void *vptr, size_t n );

int
Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);

void
Bind(int fd, const struct sockaddr *sa, socklen_t salen);

void
Connect(int fd, const struct sockaddr *sa, socklen_t salen);

void
Listen(int fd, int backlog);

void
Setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen);

int
Socket(int family, int type, int protocol);

ssize_t
readline( int fd, void* target_ptr, size_t maxlen );

ssize_t
readlinebuf( void** ptrptr );

ssize_t
Readline( int fd, void* target_ptr, size_t maxlen );

pid_t
Fork(void);

void
Fclose(FILE *fp);

FILE *
Fdopen(int fd, const char *type);

char *
Fgets(char *ptr, int n, FILE *stream);

FILE *
Fopen(const char *filename, const char *mode);

void
Fputs(const char *ptr, FILE *stream);

void
Close(int fd);

int
Sigaction( int flag, struct sigaction* newDisp, struct sigaction* oldDisp );

int
Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

void
Shutdown(int fd, int how);

ssize_t
Read( int fd, void* buf, size_t n );

#endif

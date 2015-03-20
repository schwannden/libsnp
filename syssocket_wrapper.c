#include "np_header.h"
#include "np_lib.h"
#include "error_functions.h"

const char*
Inet_ntop( int family, const void* sa, char* str, socklen_t socklen )
{
  if( str == NULL )
  fatal_sys_exit( "3rd argument can not ne NULL" );
  if( (inet_ntop(family, sa, str, socklen) == NULL) )
  fatal_sys_exit( "inet_ntop error" );
  return str;
}

void
Inet_pton(int family, const char *strptr, void *addrptr)
{
  if ( inet_pton(family, strptr, addrptr) <= 0)
    fatal_sys_exit("inet_pton error for %s", strptr);  /* errno set */
}

int
Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
  int    n;

again:
  if ( (n = accept(fd, sa, salenptr)) < 0) {
#ifdef  EPROTO
    if (errno == EPROTO || errno == ECONNABORTED)
#else
    if (errno == ECONNABORTED)
#endif
      goto again;
    else
      fatal_sys_exit("accept error");
  }
  return(n);
}

void
Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
  if (bind(fd, sa, salen) < 0)
    fatal_sys_exit("bind error");
}

void
Connect(int fd, const struct sockaddr *sa, socklen_t salen)
{
  if (connect(fd, sa, salen) < 0)
    fatal_sys_exit("connect error");
}

void
Getpeername(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
  if (getpeername(fd, sa, salenptr) < 0)
    fatal_sys_exit("getpeername error");
}

void
Getsockname(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
  if (getsockname(fd, sa, salenptr) < 0)
    fatal_sys_exit("getsockname error");
}

void
Getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlenptr)
{
  if (getsockopt(fd, level, optname, optval, optlenptr) < 0)
    fatal_sys_exit("getsockopt error");
}

void
Setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
{
	if (setsockopt(fd, level, optname, optval, optlen) < 0)
		err_sys("setsockopt error");
}

void
Shutdown(int fd, int how)
{
  if (shutdown(fd, how) < 0)
    fatal_sys_exit("shutdown error");
}

int
Sockatmark(int fd)
{
  int    n;

  if ( (n = sockatmark(fd)) < 0)
    fatal_sys_exit("sockatmark error");
  return(n);
}

/* include Socket */
int
Socket(int family, int type, int protocol)
{
  int    n;

  if ( (n = socket(family, type, protocol)) < 0)
    fatal_sys_exit("socket error");
  return(n);
}
/* end Socket */

void
Socketpair(int family, int type, int protocol, int *fd)
{
  int    n;

  if ( (n = socketpair(family, type, protocol, fd)) < 0)
    fatal_sys_exit("socketpair error");
}

void
Listen(int fd, int backlog)
{
  char  *ptr;

    /*4can override 2nd argument with environment variable */
  if ( (ptr = getenv("LISTENQ")) != NULL)
    backlog = atoi(ptr);

  if (listen(fd, backlog) < 0)
    fatal_sys_exit("listen error");
}
/* end Listen */


#include "np_header.h"
#include "np_lib.h"
#include "error_functions.h"

/* include Listen */
#ifdef  HAVE_POLL
int
Poll(struct pollfd *fdarray, unsigned long nfds, int timeout)
{
  int    n;

  if ( (n = poll(fdarray, nfds, timeout)) < 0)
    fatal_sys_exit("poll error");

  return(n);
}
#endif

ssize_t
Recv(int fd, void *ptr, size_t nbytes, int flags)
{
  ssize_t    n;

  if ( (n = recv(fd, ptr, nbytes, flags)) < 0)
    fatal_sys_exit("recv error");
  return(n);
}

ssize_t
Recvfrom(int fd, void *ptr, size_t nbytes, int flags,
     struct sockaddr *sa, socklen_t *salenptr)
{
  ssize_t    n;

  if ( (n = recvfrom(fd, ptr, nbytes, flags, sa, salenptr)) < 0)
    fatal_sys_exit("recvfrom error");
  return(n);
}

ssize_t
Recvmsg(int fd, struct msghdr *msg, int flags)
{
  ssize_t    n;

  if ( (n = recvmsg(fd, msg, flags)) < 0)
    fatal_sys_exit("recvmsg error");
  return(n);
}

int
Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
       struct timeval *timeout)
{
  int    n;

  if ( (n = select(nfds, readfds, writefds, exceptfds, timeout)) < 0)
    fatal_sys_exit("select error");
  return(n);    /* can return 0 on timeout */
}

void
Send(int fd, const void *ptr, size_t nbytes, int flags)
{
  if (send(fd, ptr, nbytes, flags) != (ssize_t)nbytes)
    fatal_sys_exit("send error");
}

void
Sendto(int fd, const void *ptr, size_t nbytes, int flags,
     const struct sockaddr *sa, socklen_t salen)
{
  if (sendto(fd, ptr, nbytes, flags, sa, salen) != (ssize_t)nbytes)
    fatal_sys_exit("sendto error");
}

void
Sendmsg(int fd, const struct msghdr *msg, int flags)
{
  unsigned int  i;
  ssize_t      nbytes;

  nbytes = 0;  /* must first figure out what return value should be */
  for (i = 0; i < msg->msg_iovlen; i++)
    nbytes += msg->msg_iov[i].iov_len;

  if (sendmsg(fd, msg, flags) != nbytes)
    fatal_sys_exit("sendmsg error");
}

void
Fclose(FILE *fp)
{
  if (fclose(fp) != 0)
    fatal_sys_exit("fclose error");
}

FILE *
Fdopen(int fd, const char *type)
{
  FILE  *fp;

  if ( (fp = fdopen(fd, type)) == NULL)
    fatal_sys_exit("fdopen error");

  return(fp);
}

char *
Fgets(char *ptr, int n, FILE *stream)
{
  char  *rptr;

  if ( (rptr = fgets(ptr, n, stream)) == NULL && ferror(stream))
    fatal_sys_exit("fgets error");

  return (rptr);
}

FILE *
Fopen(const char *filename, const char *mode)
{
  FILE  *fp;

  if ( (fp = fopen(filename, mode)) == NULL)
    fatal_sys_exit("fopen error");

  return(fp);
}

void
Fputs(const char *ptr, FILE *stream)
{
  if (fputs(ptr, stream) == EOF)
    fatal_sys_exit("fputs error");
}

ssize_t
Read( int fd, void* buf, size_t n )
{
  int bytes_read;
again:
  if( (bytes_read = read(fd, buf, n)) < 0 )
  if( errno == EINTR )
    goto again;
  else
    fatal_sys_exit( "read error" );

  return bytes_read;
}


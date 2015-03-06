#include "np_header.h"
#include "np_lib.h"
#include "error_functions.h"

const char*
Inet_ntop( int family, const void* sa, char* str, socklen_t socklen )
{
  if( str == NULL )
  err_sys( "3rd argument can not ne NULL" );
  if( (inet_ntop(family, sa, str, socklen) == NULL) )
  err_sys( "inet_ntop error" );
  return str;
}

void
Inet_pton(int family, const char *strptr, void *addrptr)
{
  if ( inet_pton(family, strptr, addrptr) <= 0)
    err_sys("inet_pton error for %s", strptr);  /* errno set */
}

//returns a representation string for sa. The string is staticalled allocated in sock_ntop with length MAXSOCKADDRSIZE.
char*
sock_ntop( const SA* sa_ptr, socklen_t socklen )
{
  in_port_t port;
  static char str[ MAXSOCKADDRSIZE ];
  switch( sa_ptr->sa_family ){
  case AF_INET:{
      struct sockaddr_in* sin_ptr = (struct sockaddr_in*)sa_ptr;
    Inet_ntop( AF_INET, &sin_ptr->sin_addr, str, MAXSOCKADDRSIZE );
    if( (port = ntohs( sin_ptr->sin_port)) > 0 )
      snprintf( str+strlen(str), MAXSOCKADDRSIZE-INET_ADDRSTRLEN, ":%d", port );
    return str;
  }
  case AF_INET6:{
    str[0] = '[';
      struct sockaddr_in6* sin6_ptr = (struct sockaddr_in6*)sa_ptr;
    Inet_ntop( AF_INET6, &sin6_ptr->sin6_addr, str+1, MAXSOCKADDRSIZE );
    if( (port = ntohs( sin6_ptr->sin6_port)) > 0 )
      snprintf( str+strlen(str), MAXSOCKADDRSIZE-INET_ADDRSTRLEN-1, "]:%d", port );
    return str;
  }
  }
}

ssize_t
readn( int fd, void* vptr, size_t n )
{
  size_t bytes_left = n;
  ssize_t bytes_read;
  char* ptr = vptr;
  bool finished = false;

  while( bytes_left > 0 && !finished ){
  if( (bytes_read = read( fd, ptr, bytes_left )) < 0 )
    if( errno == EINTR )
    bytes_read = 0;
    else
    return -1;
  else if( bytes_read == 0 )
    finished = true;

    bytes_left -= bytes_read;
    ptr += bytes_read;
  }
  return (n-bytes_left);
}

ssize_t
Readn( int fd, void* buf, size_t n )
{
  ssize_t number_read;
  if( (number_read = readn(fd, buf, n)) < 0 )
  err_sys( "readn error" );
  return number_read;
}

//return -1 if any error, and n if successful
ssize_t
writen( int fd, const void* vptr, size_t n )
{
  size_t bytes_left = n;
  ssize_t bytes_written;
  const char* ptr = vptr;

  while( bytes_left > 0 ) {
  if( (bytes_written = write(fd, ptr, bytes_left)) <= 0 )
    if( bytes_written < 0 && errno == EINTR )
      bytes_written = 0;
    else
    return -1;

  bytes_left -= bytes_written;
  ptr += bytes_written;
  }
  
  return n;
}

void
Writen( int fd, const void *ptr, size_t nbytes )
{
  if (writen(fd, ptr, nbytes) != nbytes)
    err_sys("writen error");
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
      err_sys("accept error");
  }
  return(n);
}

void
Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
  if (bind(fd, sa, salen) < 0)
    err_sys("bind error");
}

void
Connect(int fd, const struct sockaddr *sa, socklen_t salen)
{
  if (connect(fd, sa, salen) < 0)
    err_sys("connect error");
}

void
Getpeername(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
  if (getpeername(fd, sa, salenptr) < 0)
    err_sys("getpeername error");
}

void
Getsockname(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
  if (getsockname(fd, sa, salenptr) < 0)
    err_sys("getsockname error");
}

void
Getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlenptr)
{
  if (getsockopt(fd, level, optname, optval, optlenptr) < 0)
    err_sys("getsockopt error");
}

#ifdef  HAVE_INET6_RTH_INIT
int
Inet6_rth_space(int type, int segments)
{
  int ret;
  
  ret = inet6_rth_space(type, segments);
  if (ret < 0)
    err_quit("inet6_rth_space error");

  return ret;
}

void *
Inet6_rth_init(void *rthbuf, socklen_t rthlen, int type, int segments)
{
  void *ret;

  ret = inet6_rth_init(rthbuf, rthlen, type, segments);
  if (ret == NULL)
    err_quit("inet6_rth_init error");

  return ret;
}

void
Inet6_rth_add(void *rthbuf, const struct in6_addr *addr)
{
  if (inet6_rth_add(rthbuf, addr) < 0)
    err_quit("inet6_rth_add error");
}

void
Inet6_rth_reverse(const void *in, void *out)
{
  if (inet6_rth_reverse(in, out) < 0)
    err_quit("inet6_rth_reverse error");
}

int
Inet6_rth_segments(const void *rthbuf)
{
  int ret;

  ret = inet6_rth_segments(rthbuf);
  if (ret < 0)
    err_quit("inet6_rth_segments error");

  return ret;
}

struct in6_addr *
Inet6_rth_getaddr(const void *rthbuf, int idx)
{
  struct in6_addr *ret;

  ret = inet6_rth_getaddr(rthbuf, idx);
  if (ret == NULL)
    err_quit("inet6_rth_getaddr error");

  return ret;
}
#endif

#ifdef HAVE_KQUEUE
int
Kqueue(void)
{
  int ret;

  if ((ret = kqueue()) < 0)
    err_sys("kqueue error");
  return ret;
}

int
Kevent(int kq, const struct kevent *changelist, int nchanges,
       struct kevent *eventlist, int nevents, const struct timespec *timeout)
{
  int ret;

  if ((ret = kevent(kq, changelist, nchanges,
            eventlist, nevents, timeout)) < 0)
    err_sys("kevent error");
  return ret;
}
#endif


/* include Listen */
void
Listen(int fd, int backlog)
{
  char  *ptr;

    /*4can override 2nd argument with environment variable */
  if ( (ptr = getenv("LISTENQ")) != NULL)
    backlog = atoi(ptr);

  if (listen(fd, backlog) < 0)
    err_sys("listen error");
}
/* end Listen */

#ifdef  HAVE_POLL
int
Poll(struct pollfd *fdarray, unsigned long nfds, int timeout)
{
  int    n;

  if ( (n = poll(fdarray, nfds, timeout)) < 0)
    err_sys("poll error");

  return(n);
}
#endif

ssize_t
Recv(int fd, void *ptr, size_t nbytes, int flags)
{
  ssize_t    n;

  if ( (n = recv(fd, ptr, nbytes, flags)) < 0)
    err_sys("recv error");
  return(n);
}

ssize_t
Recvfrom(int fd, void *ptr, size_t nbytes, int flags,
     struct sockaddr *sa, socklen_t *salenptr)
{
  ssize_t    n;

  if ( (n = recvfrom(fd, ptr, nbytes, flags, sa, salenptr)) < 0)
    err_sys("recvfrom error");
  return(n);
}

ssize_t
Recvmsg(int fd, struct msghdr *msg, int flags)
{
  ssize_t    n;

  if ( (n = recvmsg(fd, msg, flags)) < 0)
    err_sys("recvmsg error");
  return(n);
}

int
Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
       struct timeval *timeout)
{
  int    n;

  if ( (n = select(nfds, readfds, writefds, exceptfds, timeout)) < 0)
    err_sys("select error");
  return(n);    /* can return 0 on timeout */
}

void
Send(int fd, const void *ptr, size_t nbytes, int flags)
{
  if (send(fd, ptr, nbytes, flags) != (ssize_t)nbytes)
    err_sys("send error");
}

void
Sendto(int fd, const void *ptr, size_t nbytes, int flags,
     const struct sockaddr *sa, socklen_t salen)
{
  if (sendto(fd, ptr, nbytes, flags, sa, salen) != (ssize_t)nbytes)
    err_sys("sendto error");
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
    err_sys("sendmsg error");
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
    err_sys("shutdown error");
}

int
Sockatmark(int fd)
{
  int    n;

  if ( (n = sockatmark(fd)) < 0)
    err_sys("sockatmark error");
  return(n);
}

/* include Socket */
int
Socket(int family, int type, int protocol)
{
  int    n;

  if ( (n = socket(family, type, protocol)) < 0)
    err_sys("socket error");
  return(n);
}
/* end Socket */

void
Socketpair(int family, int type, int protocol, int *fd)
{
  int    n;

  if ( (n = socketpair(family, type, protocol, fd)) < 0)
    err_sys("socketpair error");
}


//my_read reads 1 byte a time into *ptr
//my_read return 1 if successful, 0 if eof or FIN is read, -1 if error other than EINTR
static ssize_t readline_buff_size;  //the number of bytes still in the buffer
static ssize_t readline_buff_front; //like the front of a queue, where the next data will come out
static char readline_buff[MAXLINE];      //the buffer, implemented as a queue

ssize_t
my_read( int fd, char* ptr )
{
  if( readline_buff_size <= 0 ){
again:
  if( (readline_buff_size = read( fd, readline_buff, sizeof(readline_buff))) < 0 )
    if( errno = EINTR )
    goto again;
    else
    return -1;
  else if( readline_buff_size == 0 )
    return 0;
  else
    readline_buff_front = 0;
  }
  *ptr = readline_buff[ readline_buff_front++ ];
  readline_buff_size--;
  return 1;
}

//readline returns the string length read (excluding '\0'), or -1 if error
//readline reads a string until '\n' or eof, including '\n', then append the '\0' at the end.
ssize_t
readline( int fd, void* void_ptr, size_t maxline )
{
  ssize_t bytes_read, read_status;
  char* ptr = void_ptr;
  for( bytes_read = 1 ; bytes_read < maxline ; bytes_read++ )
    if( (read_status = my_read( fd, ptr )) == 1  ) {
    if( *ptr++ == '\n' ){
    *ptr = '\0';
    return bytes_read;
    }
    } else if( read_status == 0 ) {
    *ptr = '\0';
    return bytes_read-1;
  } else
    return -1;
}
//readlinebuf set *ptrptr to the location of the unread byte in the buffer and return buffer size
ssize_t
readlinebuf( void** ptrptr )
{
  if( readline_buff_size > 0 )
  *ptrptr = readline_buff+readline_buff_front;
  return readline_buff_size;
}

ssize_t
Readline( int fd, void* target_ptr, size_t maxline )
{
  ssize_t n;
  if( (n = readline( fd, target_ptr, maxline )) < 0 )
  err_sys( "readline error" );
  return n;
}


pid_t
Fork(void)
{
  pid_t  pid;

  if ( (pid = fork()) == -1)
    err_sys("fork error");
  return(pid);
}

void
Fclose(FILE *fp)
{
  if (fclose(fp) != 0)
    err_sys("fclose error");
}

FILE *
Fdopen(int fd, const char *type)
{
  FILE  *fp;

  if ( (fp = fdopen(fd, type)) == NULL)
    err_sys("fdopen error");

  return(fp);
}

char *
Fgets(char *ptr, int n, FILE *stream)
{
  char  *rptr;

  if ( (rptr = fgets(ptr, n, stream)) == NULL && ferror(stream))
    err_sys("fgets error");

  return (rptr);
}

FILE *
Fopen(const char *filename, const char *mode)
{
  FILE  *fp;

  if ( (fp = fopen(filename, mode)) == NULL)
    err_sys("fopen error");

  return(fp);
}

void
Fputs(const char *ptr, FILE *stream)
{
  if (fputs(ptr, stream) == EOF)
    err_sys("fputs error");
}

void
Close(int fd)
{
  if (close(fd) == -1)
    err_sys("close error");
}

typedef void sighandler_t( int);

//Sigaction autamatically set the SA_RESTART flag
int Sigaction( int sig, struct sigaction* ptrNewDisp, struct sigaction* ptrOldDisp )
{
  if( sig == SIGALRM ) {
#ifdef SA_INTERRUPT
  ptrNewDisp->sa_flags |= SA_INTERRUPT;
#endif
  } else {
#ifdef SA_RESTART
  ptrNewDisp->sa_flags |= SA_RESTART;
#endif
  }
  if( (sigaction( sig, ptrNewDisp, ptrOldDisp ) < 0) )
  err_sys( "sigaction error" );
  
  return 0;
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
    err_sys( "read error" );

  return bytes_read;
}

sctp_assoc_t
sctp_address_to_associd(int sock_fd, struct sockaddr *sa, socklen_t salen)
{
  struct sctp_paddrparams sp;
  int siz;

  siz = sizeof(struct sctp_paddrparams);
  bzero(&sp,siz);
  memcpy(&sp.spp_address,sa,salen);
  Sctp_opt_info(sock_fd,0,
       SCTP_PEER_ADDR_PARAMS, &sp, &siz);
  return(sp.spp_assoc_id);
}

int 
sctp_get_no_strms(int sock_fd,struct sockaddr *to, socklen_t tolen)
{
  int retsz;
  struct sctp_status status;
  retsz = sizeof (status);  
  bzero (&status, sizeof (status));

  status.sstat_assoc_id = sctp_address_to_associd (sock_fd, to, tolen);
  Sctp_opt_info (sock_fd, status.sstat_assoc_id, SCTP_STATUS, &status, &retsz);
  
  return (status.sstat_outstrms);
}

int
Sctp_opt_info(int sockfd, sctp_assoc_t assoc_id, int opt, void* arg, socklen_t *size)
{
  if (sctp_opt_info (sockfd, assoc_id, opt, arg, size) == 0)
    return 0;
  else
    err_sys( "scpt_opt_info error" );
}

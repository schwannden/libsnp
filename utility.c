#include "np_header.h"
#include "np_lib.h"
#include "error_functions.h"

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
  fatal_sys_exit( "readn error" );
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
    fatal_sys_exit("writen error");
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
  fatal_sys_exit( "readline error" );
  return n;
}

struct addrinfo*
host_serv(const char *host, const char *serv, int family, int socktype)
{
  int              n;
  struct addrinfo  hints, *res;

  bzero(&hints, sizeof(struct addrinfo));
  hints.ai_flags = AI_CANONNAME;  /* always return canonical name */
  hints.ai_family = family;       /* AF_UNSPEC, AF_INET, AF_INET6, etc. */
  hints.ai_socktype = socktype;   /* 0, SOCK_STREAM, SOCK_DGRAM, etc. */

  if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0)
    return(NULL);

  return(res);  /* return pointer to first on linked list */
}
/* end host_serv */

/*
 * There is no easy way to pass back the integer return code from
 * getaddrinfo() in the function above, short of adding another argument
 * that is a pointer, so the easiest way to provide the wrapper function
 * is just to duplicate the simple function as we do here.
 */

struct addrinfo *
Host_serv(const char *host, const char *serv, int family, int socktype)
{
  int             n;
  struct addrinfo hints, *res;

  bzero(&hints, sizeof(struct addrinfo));
  hints.ai_flags    = AI_CANONNAME;  /* always return canonical name */
  hints.ai_family   = family;        /* 0, AF_INET, AF_INET6, etc. */
  hints.ai_socktype = socktype;      /* 0, SOCK_STREAM, SOCK_DGRAM, etc. */

  if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0)
    err_quit("host_serv error for %s, %s: %s",
         (host == NULL) ? "(no hostname)" : host,
         (serv == NULL) ? "(no service name)" : serv,
         gai_strerror(n));

  return(res);  /* return pointer to first on linked list */
}

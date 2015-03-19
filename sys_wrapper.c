#include "np_header.h"
#include "np_lib.h"
#include "error_functions.h"
pid_t
Fork(void)
{
  pid_t  pid;

  if ( (pid = fork()) == -1)
    fatal_sys_exit("fork error");
  return(pid);
}

void*
Malloc(size_t size)
{
  void* buf = malloc (size);
  if (buf == NULL)
    fatal_sys_exit ("malloc error");
  return buf;
}

void
Close(int fd)
{
  if (close(fd) == -1)
    fatal_sys_exit("close error");
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
  fatal_sys_exit( "sigaction error" );
  
  return 0;
}


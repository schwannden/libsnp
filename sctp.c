#include "np_header.h"
#include "np_lib.h"
#include "error_functions.h"
#include "log.h"

int
Sctp_opt_info(int sockfd, sctp_assoc_t assoc_id, int opt, void* arg, socklen_t *size)
{
  if (sctp_opt_info (sockfd, assoc_id, opt, arg, size) == 0)
    return 0;
  else
    fatal_sys_exit( "scpt_opt_info error" );
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
Sctp_sendmsg (int sock_fd, void* data, size_t len, SA* to, 
              socklen_t tolen, uint32_t ppid, uint32_t flags, 
              uint16_t stream_no, uint32_t timetolive, uint32_t context)
{
  int ret;
  ret = sctp_sendmsg (sock_fd, data, len, to, tolen, ppid, flags, 
                      stream_no, timetolive, context);
  if (ret < 0)
    fatal_sys_exit ("sctp_sendmsg error");
  return ret;
}

int
Sctp_recvmsg (int sock_fd, void *msg, size_t len, SA* from,
              socklen_t *fromlen, struct sctp_sndrcvinfo *sinfo,
              int *msg_flags)
{
  int ret;
  ret = sctp_recvmsg (sock_fd, msg, len, from, fromlen, sinfo, msg_flags);
  if (ret < 0 && errno != EWOULDBLOCK){
    fatal_sys_exit ("sctp_recvmsg error");
  }
  return(ret);
}

static uint8_t* sctp_pdapi_readbuf = NULL;
static int sctp_pdapi_readbuf_size = 0;

uint8_t *
pdapi_recvmsg (int sock_fd, int* readlen, SA* from, int* fromlen, 
               struct sctp_sndrcvinfo* sri, int* msg_flags)
{
  int bytes_read, bytes_left;

  if (sctp_pdapi_readbuf == NULL)
    {
      sctp_pdapi_readbuf = (uint8_t*) Malloc (SCTP_PDAPI_INCR_SIZE);
      sctp_pdapi_readbuf_size = SCTP_PDAPI_INCR_SIZE;
    }

  bytes_read = Sctp_recvmsg (sock_fd, sctp_pdapi_readbuf, sctp_pdapi_readbuf_size, 
                             from, fromlen, sri, msg_flags);
  if (bytes_read < 0 && errno != EWOULDBLOCK)
    log_error ("pdapi_recvmsg error");
  while ((*msg_flags & MSG_EOR) == 0)
    {
      bytes_left = sctp_pdapi_readbuf_size - bytes_read;
      if (bytes_left < SCTP_NEED_MORE_THRESHOLD)
        {
          sctp_pdapi_readbuf = 
            realloc (sctp_pdapi_readbuf, sctp_pdapi_readbuf_size + SCTP_PDAPI_INCR_SIZE);
          if (sctp_pdapi_readbuf == NULL)
            fatal_sys_exit ("realloc error, sctp partial delivery api ran out of memory");
          sctp_pdapi_readbuf_size += SCTP_PDAPI_INCR_SIZE;
          bytes_left = sctp_pdapi_readbuf_size - bytes_read;
        }
      bytes_read += Sctp_recvmsg (sock_fd, &sctp_pdapi_readbuf[bytes_read], 
                                  sctp_pdapi_readbuf_size, from, fromlen, 
                                  sri, msg_flags);
    }
  *readlen = bytes_read;
  return sctp_pdapi_readbuf;
}

static char ip_addr_presentation[INET_ADDRSTRLEN];
void print_notification (int sock_fd, char* notify_buf)
{
  union sctp_notification* notification;
  struct sctp_assoc_change*     sac;
  struct sctp_paddr_change*     spc;
  struct sctp_remote_error*     sre;
  struct sctp_send_failed*      ssf;
  struct sctp_shutdown_event*   sse;
  struct sctp_adaptation_event* sae;
  struct sctp_pdapi_event*      pdapi;
  const char* str; 
  struct sockaddr_storage *address_list;
  int address_number = 0;

  notification = (union sctp_notification*) notify_buf;
  switch (notification->sn_header.sn_type)
    {
      case SCTP_ASSOC_CHANGE:
        sac = &notification->sn_assoc_change;
        switch (sac->sac_state)
          {
            case SCTP_COMM_UP:
              str = "COMMUNICATION UP";
              address_number = 1;
              break;
            case SCTP_COMM_LOST:
              str = "COMMUNICATION LOST";
              break;
            case SCTP_RESTART:
              str = "RESTART";
              address_number = 1;
              break;
            case SCTP_SHUTDOWN_COMP:
              str = "SHUTDOWN COMPLETE";
              break;
            case SCTP_CANT_STR_ASSOC:
              str = "CAN'T START ASSOCIATION";
              break;
            default:
              str = "UNKNOWN";
              break;
          }
        log_info ("#  SCTP notification: SCTP_ASSOC_CHANGE: %s, assoc=0x%x",
                 str, (uint32_t) sac->sac_assoc_id);
        if (address_number == 1) //only print address on SCTP_COMM_UP and SCTP_RESTART
          {
            address_number = sctp_getpaddrs (sock_fd, sac->sac_assoc_id, (SA**)&address_list);
            log_info ("There are %d remote address:", address_number);
            sctp_list_addresses (address_list, address_number);
            sctp_freepaddrs ((SA*) address_list);
            address_number = sctp_getladdrs (sock_fd, sac->sac_assoc_id, (SA**)&address_list);
            log_info ("There are %d local address:", address_number);
            sctp_list_addresses (address_list, address_number);
            sctp_freeladdrs ((SA*) address_list);
          }
        break;
      case SCTP_PEER_ADDR_CHANGE:
        spc = &notification->sn_paddr_change;
        switch ( spc->spc_state)
          {
            case SCTP_ADDR_AVAILABLE:
              str = "ADDRESS AVAILABLE";
              break;
            case SCTP_ADDR_UNREACHABLE:
              str = "ADDRESS UNREACHABLE";
              break;
            case SCTP_ADDR_REMOVED:
              str = "ADDRESS REMOVED";
              break;
            case SCTP_ADDR_ADDED:
              str = "ADDRESS ADDED";
              break;
            case SCTP_ADDR_MADE_PRIM:
              str = "ADDRESS MADE PRIMARY";
              break;
            case SCTP_ADDR_CONFIRMED:
              str = "ADDRESS CONFIRMED";
              break;
            default:
              str = "UNKNOWN";
              break;
          }
        log_info ("#  SCTP notification: SCTP_PEER_ADDR_CHANGE: %s, addr=%s, assoc=0x%x",
                 str, Sock_ntop ( (SA*) &spc->spc_aaddr, sizeof (spc->spc_aaddr) ),
                 (uint32_t) spc->spc_assoc_id);
        break;
      case SCTP_REMOTE_ERROR:
        sre = &notification->sn_remote_error;
        log_info ("#  SCTP notification: SCTP_REMOTE_ERROR: asoc=0x%x, error=%d",
                 (uint32_t) sre->sre_assoc_id, sre->sre_error);
        break;
      case SCTP_SEND_FAILED:
        ssf = &notification->sn_send_failed;
        log_info ("#  SCTP notification: SCTP_SEND_FAILED: asoc=0x%x, error=%d",
                 (uint32_t) ssf->ssf_assoc_id, ssf->ssf_error);
        break;
      case SCTP_ADAPTATION_INDICATION:
        sae = &notification->sn_adaptation_event;
        log_info ("#  SCTP notification: SCTP_ADAPTATION_INDICATION: 0x%x", (u_int) sae->sai_adaptation_ind);
        break;
      case SCTP_PARTIAL_DELIVERY_EVENT:
        pdapi = &notification->sn_pdapi_event;
        if (pdapi->pdapi_indication == SCTP_PARTIAL_DELIVERY_ABORTED)
          log_info ("#  SCTP notification: SCTP_PARTIAL_DELIVERY_ABORTED");
        else
          log_info ("#  SCTP notification: SUnknown SCTP_PARTIAL_DELIVERY_EVENT 0x%x",
                  pdapi->pdapi_indication);
        break;
      case SCTP_SHUTDOWN_EVENT:
        sse = &notification->sn_shutdown_event;
        log_info ("#  SCTP notification: SCTP_SHUTDOWN_EVENT: assoc_id=0x%x", (uint32_t)sse->sse_assoc_id);
        break;
      default:
        nonfatal_user_ret ("#  SCTP notification: unknown notification type 0x%x", notification->sn_header.sn_type);
    }
}

void
sctp_list_addresses (struct sockaddr_storage* addrs, int n)
{
  if (n<1)
    fatal_user_exit ("error sctp_list_addresses: cannot list %n addresses", n);

  struct sockaddr_storage* current;
  int                      i, socklen;

  current = addrs;
  for (i = 0 ; i < n ; i++)
    {
      log_info ("    %d. %s", i+1, Sock_ntop ((SA*)current, socklen));
#ifdef HAVE_SOCKADDR_SA_LEN
      socklen = current->ss_len;
#else
      switch (current->ss_family)
        {
          case AF_INET: socklen = sizeof (struct sockaddr_in); break;
#ifdef IPV6
          case AF_INET6: socklen = sizeof (struct sockaddr_in6); break;
#endif
          default: fatal_user_exit ("sctp_list_addresses unknown AF");
        }
#endif
      current = (struct sockaddr_storage*) ((char*) current + socklen);
    }
}

int
heartbeat_action (int sock_fd, struct sockaddr* sa, socklen_t salen, int interval)
{
  struct sctp_paddrparams paddrparams;
  int    size = sizeof(paddrparams);
  bzero (&paddrparams, size);
  memcpy ((void*) &paddrparams.spp_address, sa, salen);
  Setsockopt (sock_fd, IPPROTO_SCTP, SCTP_PEER_ADDR_PARAMS, &paddrparams, size);
  return 0;
}

char *
sock_ntop(const struct sockaddr *sa, socklen_t salen)
{
    char  portstr[8];
    static char str[128];    /* Unix domain is largest */

  switch (sa->sa_family) {
  case AF_INET: {
    struct sockaddr_in  *sin = (struct sockaddr_in *) sa;

    if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
      return(NULL);
    if (ntohs(sin->sin_port) != 0) {
      snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin_port));
      strcat(str, portstr);
    }
    return(str);
  }
/* end sock_ntop */

#ifdef  IPV6
  case AF_INET6: {
    struct sockaddr_in6  *sin6 = (struct sockaddr_in6 *) sa;

    str[0] = '[';
    if (inet_ntop(AF_INET6, &sin6->sin6_addr, str + 1, sizeof(str) - 1) == NULL)
      return(NULL);
    if (ntohs(sin6->sin6_port) != 0) {
      snprintf(portstr, sizeof(portstr), "]:%d", ntohs(sin6->sin6_port));
      strcat(str, portstr);
      return(str);
    }
    return (str + 1);
  }
#endif

#ifdef  AF_UNIX
  case AF_UNIX: {
    struct sockaddr_un  *unp = (struct sockaddr_un *) sa;

      /* OK to have no pathname bound to the socket: happens on
         every connect() unless client calls bind() first. */
    if (unp->sun_path[0] == 0)
      strcpy(str, "(no pathname bound)");
    else
      snprintf(str, sizeof(str), "%s", unp->sun_path);
    return(str);
  }
#endif

#ifdef  HAVE_SOCKADDR_DL_STRUCT
  case AF_LINK: {
    struct sockaddr_dl  *sdl = (struct sockaddr_dl *) sa;

    if (sdl->sdl_nlen > 0)
      snprintf(str, sizeof(str), "%*s (index %d)",
           sdl->sdl_nlen, &sdl->sdl_data[0], sdl->sdl_index);
    else
      snprintf(str, sizeof(str), "AF_LINK, index=%d", sdl->sdl_index);
    return(str);
  }
#endif
  default:
    snprintf(str, sizeof(str), "sock_ntop: unknown AF_xxx: %d, len %d",
         sa->sa_family, salen);
    return(str);
  }
    return (NULL);
}

char *
Sock_ntop(const struct sockaddr *sa, socklen_t salen)
{
  char  *ptr;

  if ( (ptr = sock_ntop(sa, salen)) == NULL)
    err_sys("sock_ntop error");  /* inet_ntop() sets errno */
  return(ptr);
}

int
Sctp_bindx(int sock_fd,struct sockaddr_storage *at,int num,int op)
{
  int ret;
  ret = sctp_bindx (sock_fd, (struct sockaddr*) at, num, op);
  if (ret < 0)
    err_sys("sctp_bindx error");
  return ret;
}

int
sctp_bind_arg_list(int sock_fd, char **argv, int argc)
{
  struct addrinfo *addr;
  char *bindbuf, *p, portbuf[10];
  int addrcnt=0;
  int i;

  bindbuf = (char *) calloc (argc, sizeof (struct sockaddr_storage));
  p = bindbuf;
  sprintf(portbuf, "%d", SERV_PORT);
  for( i=0; i<argc; i++ )
    {
      addr = Host_serv(argv[i], portbuf, AF_UNSPEC, SOCK_SEQPACKET);
      memcpy(p, addr->ai_addr, addr->ai_addrlen);
      freeaddrinfo(addr);
      addrcnt++;
      p += addr->ai_addrlen;
    }
  Sctp_bindx ( sock_fd, (struct sockaddr_storage*) bindbuf, addrcnt, SCTP_BINDX_ADD_ADDR);
  free (bindbuf);
  return 0;
}

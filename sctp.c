#include "np_header.h"
#include "np_lib.h"
#include "error_functions.h"

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
  if (ret < 0){
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
void print_notification (char* notify_buf)
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

  notification = (union sctp_notification*) notify_buf;
  switch (notification->sn_header.sn_type)
    {
      case SCTP_ASSOC_CHANGE:
        sac = &notification->sn_assoc_change;
        switch (sac->sac_state)
          {
            case SCTP_COMM_UP:
              str = "COMMUNICATION UP";
              break;
            case SCTP_COMM_LOST:
              str = "COMMUNICATION LOST";
              break;
            case SCTP_RESTART:
              str = "RESTART";
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
        printf ("#  SCTP notification: SCTP_ASSOC_CHANGE: %s, assoc=0x%x\n",
                 str, (uint32_t) sac->sac_assoc_id);
        break;
      case SCTP_PEER_ADDR_CHANGE:
        spc = &notification->sn_paddr_change;
        switch (spc->spc_state)
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
            default:
              str = "UNKNOWN";
              break;
          }
        Inet_ntop (AF_INET, (SA*) &spc->spc_aaddr, ip_addr_presentation, sizeof (spc->spc_aaddr)); 
        printf ("#  SCTP notification: SCTP_PEER_ADDR_CHANGE: %s, addr=%s, assoc=0x%x\n",
                 str, ip_addr_presentation, (uint32_t) spc->spc_assoc_id);
        break;
      case SCTP_REMOTE_ERROR:
        sre = &notification->sn_remote_error;
        printf ("#  SCTP notification: SCTP_REMOTE_ERROR: asoc=0x%x, error=%d\n",
                 (uint32_t) sre->sre_assoc_id, sre->sre_error);
        break;
      case SCTP_SEND_FAILED:
        ssf = &notification->sn_send_failed;
        printf ("#  SCTP notification: SCTP_SEND_FAILED: asoc=0x%x, error=%d\n",
                 (uint32_t) ssf->ssf_assoc_id, ssf->ssf_error);
        break;
      case SCTP_ADAPTATION_INDICATION:
        sae = &notification->sn_adaptation_event;
        printf ("#  SCTP notification: SCTP_ADAPTATION_INDICATION: 0x%x\n", (u_int) sae->sai_adaptation_ind);
        break;
      case SCTP_PARTIAL_DELIVERY_EVENT:
        pdapi = &notification->sn_pdapi_event;
        if (pdapi->pdapi_indication == SCTP_PARTIAL_DELIVERY_ABORTED)
          printf ("#  SCTP notification: SCTP_PARTIAL_DELIVERY_ABORTED\n");
        else
          printf ("#  SCTP notification: SUnknown SCTP_PARTIAL_DELIVERY_EVENT 0x%x\n",
                  pdapi->pdapi_indication);
        break;
      case SCTP_SHUTDOWN_EVENT:
        sse = &notification->sn_shutdown_event;
        printf ("#  SCTP notification: SCTP_SHUTDOWN_EVENT: assoc_id=0x%x\n", (uint32_t)sse->sse_assoc_id);
        break;
      default:
        nonfatal_user_ret ("#  SCTP notification: unknown notification type 0x%x\n", notification->sn_header.sn_type);
    }
}


#ifndef __NP_HEADER 
#define __NP_HEADER 

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
/*unistd.h:
ssize_t write(int fd, const void *buf, size_t count);
ssize_t read(int fd, void *buf, size_t count);
int close(int fd);
*/
#include<fcntl.h>
/*
int  creat(const char *, mode_t);
int  fcntl(int, int, ...);
int  open(const char *, int, ...);
and associate flags
*/
#include<errno.h>
/* errno.h
errno variable and the errno valies
*/
#include<netinet/in.h>
/* netinet/in.h
struct in_addr
struct sockaddr
struct sockaddr_in
IPPROTO_...
INET_ADDRSTRLEN
*/
#include<sys/socket.h>
/* sys/socket.h
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int socket(int domain, int type, int protocol);
int listen(int socket, int backlog);
int accept(int socket, struct sockaddr *restrict address, socklen_t *restrict address_len);
*/
#include<sys/types.h>
#include<arpa/inet.h>
/*arpa/inet.h
const char *inet_ntop(int af, const void *restrict src, char *restrict dst, socklen_t size);
int inet_pton(int af, const char *restrict src, void *restrict dst);
*/
#include<signal.h>
/*signal.h
for signal, sigaction
*/

#endif

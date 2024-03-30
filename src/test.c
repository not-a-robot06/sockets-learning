#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define MYPORT "3582"
#define BUF_LEN 256

int
main(void){
  struct sockaddr_storage in_addr;
  socklen_t addr_size;
  struct addrinfo hints, *res;
  int sockfd, new_fd;
  char buf[BUF_LEN];

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  getaddrinfo(NULL, MYPORT, &hints, &res);

  sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  bind(sockfd, res->ai_addr, res->ai_addrlen);
  listen(sockfd, 10);

  addr_size = sizeof in_addr;
  new_fd = accept(sockfd, (struct sockaddr *)&in_addr, &addr_size);
  close(sockfd);

  if (recv(new_fd, buf, BUF_LEN, 0) < 0)
    return 1;
  printf("recv msg: %s", buf);
  close(new_fd);

  return 0;
}

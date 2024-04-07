#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_LEN 256
#define PATH_LEN 128
#define PORT "3456"
#define BACKLOG 10

int
main(void){
  struct sockaddr_storage client_addr;
  socklen_t addr_size = sizeof client_addr;
  struct addrinfo hints, *serv_info;
  int sockfd, acpt_fd, yes = 1, err, recv_len, file_len;
  char buf[BUF_LEN], method[16], path[PATH_LEN], *file_buf;
  FILE *file;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_socktype = SOCK_STREAM;

  if ((err = getaddrinfo(NULL, PORT, &hints, &serv_info)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
    return 1;
  }
  if ((sockfd = socket(serv_info->ai_family, serv_info->ai_socktype,
        serv_info->ai_protocol)) < 0) {
    perror("socket");
    exit(1);
  }
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0) {
    perror("setsockopt");
    exit(1);
  }
  if (bind(sockfd, serv_info->ai_addr, serv_info->ai_addrlen) < 0) {
    perror("bind");
    exit(1);
  }
  freeaddrinfo(serv_info);

  if (listen(sockfd, BACKLOG) < 0) {
    perror("listen");
    exit(1);
  }

  if ((acpt_fd =
      accept(sockfd, (struct sockaddr *)&client_addr, &addr_size)) < 0) {
    perror("accept");
    exit(1);
  }
  chdir("./resources");

  while (1) {
    if ((recv_len = recv(acpt_fd, buf, BUF_LEN, 0)) < 0) {
      perror("accept");
      exit(1);
    }
    buf[recv_len] = '\0';

    printf("got request: %s\n", buf);
    sscanf(buf, "%s %s", method, path);
    printf("method: %s\n", method);
    if (strcmp(method, "GET") == 0) {
      printf("requested: %s\n", path);
      memmove(&path[1], path, PATH_LEN-1);
      path[0] = '.';

      file = fopen(path, "r");
      fseek(file, 0, SEEK_END);
      file_len = ftell(file);
      rewind(file);

      file_buf = malloc(file_len+1);
      fread(file_buf, file_len, 1, file);
      fclose(file);
      file_buf[file_len] = '\0';

      if (send(acpt_fd, file_buf, file_len+1, 0) < 0) {
        perror("send");
        exit(1);
      }
      free(file_buf);
    }
    else
      printf("invalid method\n");
  }
  close(acpt_fd);
  close(sockfd);

  return 0;
}

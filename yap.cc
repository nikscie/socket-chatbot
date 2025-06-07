#include <arpa/inet.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
int server(int fd);
int client(int fd);
int yappage(int fd);

int main(int argc, char **argv) {
  printf("%d\n", argc);
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (argc == 1) {
    server(fd);
  } else {
    client(fd);
  }
}

int server(int fd) {
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(7777);

int opt = 1;
if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) <
 0){
perror("sockopt wahwah");
return EXIT_FAILURE;
}

  if (bind(fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    return EXIT_FAILURE;
  }

  if (listen(fd, 5) < 0) {
    perror("listen fail");
    return EXIT_FAILURE;
 }

  socklen_t addrlen = sizeof(address);
  int new_socket = accept(fd, (struct sockaddr *)&address, &addrlen);
  if (new_socket < 0) {
    perror("accept failed");
    return EXIT_FAILURE;
  }

  yappage(new_socket);
  return 0;
}

int client(int fd) {
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(7777);
  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
    printf("\ninvalid waah \n");
    return EXIT_FAILURE;
  }

  int status = connect(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (status < 0) {
    perror("\nconnection failed. go cry \n");
    return EXIT_FAILURE;
  }
  yappage(fd);
  return 0;
}

int yappage(int fd) {
  while (1) {
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
    FD_SET(0, &rfds);
    int ready = select(fd + 1, &rfds, nullptr, nullptr, nullptr);
    char buf[1024];
    if (FD_ISSET(fd, &rfds)) {
      int count = read(fd, buf, 1024);
      if (count < 1) {
        break;
      }
      write(1, "::", 2);
      write(1, buf, count);
    } else {
      int count = read(0, buf, 1024);
      if (count < 1) {
        break;
      }
      write(fd, buf, count);
    }
  }
  return 0;
}








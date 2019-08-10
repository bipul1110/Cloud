#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fstream>
#include <sstream>

#define MAX_SIZE 5 * 1024 * 1024
#define END_OF_FILE "^^&^&^^"

typedef struct {
  std::string fname;
  std::string buffer;
  int buffLen;
  char eof[sizeof(END_OF_FILE)] = END_OF_FILE;
}file_t;

void error(const char *msg)
{
  perror(msg);
  exit(0);
}

void read_file(file_t* file_struct)
{
  std::ifstream in(file_struct->fname);
  if(!in) 
  {
    std::cout << "Cannot read file " << file_struct->fname << std::endl;
    return;
  }
  std::stringstream buf;
  buf << in.rdbuf();
  in.seekg(0, std::ios::end);
  file_struct->buffLen = in.tellg();
  file_struct->buffer = buf.str();
  in.close();
}

void write_to_socket(int sockfd, char* buffer, int buff_len)
{
  int n = write(sockfd, buffer, buff_len);
  if (n < 0)
  {
    error("ERROR writing to socket");
  }
}

void send_file(int sockfd, const file_t *file)
{
  std::cout << sizeof(*file) << std::endl;
  write_to_socket(sockfd, (char*) file, sizeof(file));
}

int main(int argc, char *argv[])
{
  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;

  char buffer[MAX_SIZE];
  if (argc < 3)
  {
    fprintf(stderr, "usage %s hostname port\n", argv[0]);
    exit(0);
  }

  portno = atoi(argv[2]);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    error("ERROR opening socket");
  }

  server = gethostbyname(argv[1]);
  if (server == NULL)
  {
    fprintf(stderr, "ERROR, no such host\n");
    exit(0);
  }

  bzero((char *)&serv_addr, sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;

  bcopy((char *)server->h_addr,
        (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);

  serv_addr.sin_port = htons(portno);
  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    error("ERROR connecting");
  }
  printf("Please enter the message: ");

  bzero(buffer, MAX_SIZE);
  // fgets(buffer, MAX_SIZE, stdin);
  file_t to_send = {"hello.txt", buffer};

  read_file(&to_send);
  send_file(sockfd, &to_send);

  bzero(buffer, 256);
  n = read(sockfd, buffer, 255);
  if (n < 0)
  {
    error("ERROR reading from socket");
  }
  
  printf("%s\n", buffer);
  close(sockfd);
  
  return 0;
}
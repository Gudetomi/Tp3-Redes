#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <time.h>

void error(const char *msg);

float time_diff(struct timeval *start, struct timeval *end);

int enviar_arquivo_uni(int newsockfd, char buffer[], FILE *fp, int buffer_tam);

char* receber_ack(int newsockfd, char *buffer, int buffer_tam);

int enviar_arquivo_rr(int newsockfd, char buffer[], FILE *fp, int buffer_tam);

int open_sock();

int open_connect(int portno, int sockfd);

int server_rr(int newsockfd, int sockfd, int portno, char *buffer, int tam, int buffer_tam, FILE *fp);

int server_uni(int newsockfd, int sockfd, int portno, char *buffer, int tam, int buffer_tam, FILE *fp);
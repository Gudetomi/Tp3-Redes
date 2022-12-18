#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <time.h>

void error(const char *msg);

float time_diff(struct timeval *start, struct timeval *end);

int open_sock();

void open_connect(int portno, int sockfd, char *hostname);

void send_filename(int sockfd, char *buffer, char *filename, int buffer_tam);

void client_uni(int sockfd, int buffer_tam, char *buffer, FILE *fp, int *bytes_recebidos);

void enviar_ack(int sockfd, int byte_atual);

int recebe_blocos(int sockfd, int bytes_recebidos, char *buffer, int BUFFER_TAM, int tam, FILE *fp);

void client_rr(int sockfd, int buffer_tam, char *buffer, FILE *fp, int *bytes_recebidos);

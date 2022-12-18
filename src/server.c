#include "server.h"

void error(const char *msg){
    perror(msg);
    exit(0);
}

float time_diff(struct timeval *start, struct timeval *end){ // Função para calcular tempo de execução
    return (end->tv_sec - start->tv_sec) + 1e-6*(end->tv_usec - start->tv_usec);
}

int enviar_arquivo_uni(int newsockfd, char buffer[], FILE *fp, int buffer_tam){
    int tam = 0, read_return, bytes_enviados = 0;
    while(!feof(fp)){
        read_return = fread(&buffer[tam], sizeof(char), 1, fp); // Lê um byte por vez do arquivo
        tam++;
        if (read_return == 0 || tam == buffer_tam){ // Se terminou de ler o arquivo, ou o buffer está cheio
            bytes_enviados += tam;
            write(newsockfd, buffer, tam); // Manda todo o buffer pro cliente
            bzero(buffer, buffer_tam);
            tam = 0;
        }
    }
    return bytes_enviados;
}

char* receber_ack(int newsockfd, char *buffer, int buffer_tam){
    read(newsockfd, buffer, 20);
    return buffer;
}

int enviar_arquivo_rr(int newsockfd, char buffer[], FILE *fp, int buffer_tam){
    int tam = 0, read_return = 0, bytes_enviados = 0;
    long int intervalo = 0;
    char valor[20];

    bzero(buffer, buffer_tam);
    while(!feof(fp)){
        read_return = fread(&buffer[tam], sizeof(char), 1, fp); // Lê um byte por vez do arquivo
        tam++;
        if (read_return == 0 || tam == buffer_tam){ // Se terminou de ler o arquivo, ou o buffer está cheio
            bytes_enviados += tam;
            sprintf(valor, "%i", tam);
            write(newsockfd, valor, 20); // manda quantos bytes o cliente precisa esperar
            write(newsockfd, buffer, tam); // Manda todo o buffer pro cliente
            bzero(buffer, buffer_tam);
            buffer = receber_ack(newsockfd, buffer, buffer_tam); // Recebe qual o próximo byte que o cliente está esperando
            intervalo = atoi(buffer);
            if(intervalo != bytes_enviados){ // Se o cliente recebeu menos bytes do que era esperado
                fseek(fp, intervalo, SEEK_SET); // Move o ponteiro do arquivo pro byte que o cliente está esperando
            }
            tam = 0;
            bzero(buffer, buffer_tam);
        }
    }
    return bytes_enviados;
}

int open_sock(){
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        error("ERROR opening socket");
    }
    return sockfd;
}

int open_connect(int portno, int sockfd){
    int newsockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        error("ERROR on binding");
    }

    listen(sockfd,5);
    clilen = sizeof(cli_addr);

    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); // Aceita a primeira conexao

    if (newsockfd < 0) {
        error("ERROR on accept");
    }
    return newsockfd;
}

int server_rr(int newsockfd, int sockfd, int portno, char *buffer, int tam, int buffer_tam, FILE *fp){
    char *nome_arquivo;
    int n, bytes_enviados;
    struct timeval start;
    struct timeval end;
    struct sockaddr_in cli_addr;
    socklen_t clilen;
    n = read(newsockfd, &buffer[tam], 1);
    if (n < 0){
        error("ERROR reading from socket");
    }
    if(buffer[tam] == '0'){ // cliente enviou o nome do arquivo inteiro
        buffer[tam] = '\0'; // Terminador de string: sumir com o 0 do final do arquivo
        nome_arquivo = (char*)calloc(strlen(buffer), sizeof(char));
        strcpy(nome_arquivo, buffer);
        fp = fopen(buffer, "r");
        tam = -1;
        bzero(buffer, buffer_tam);
        if (fp == NULL){ // Se nao foi possivel abrir o arquivo
            close(newsockfd);
            newsockfd = open_connect(portno, sockfd);
        }else{ // Se conseguiu abrir o arquivo
            bytes_enviados = enviar_arquivo_rr(newsockfd, buffer, fp, buffer_tam); // Envia o arquivo pro cliente

            close(newsockfd); // Fecha a conexao
            gettimeofday(&end, NULL); // Finaliza o cronometro
            printf("Quantidade de bytes enviados: %u byte(s). Tempo gasto de envio do arquivo \"%s\": %.2fs\n", bytes_enviados, nome_arquivo, time_diff(&start, &end));
            free(nome_arquivo);
            printf("\nAguardando novas conexões...\n\n");
            bzero(buffer, buffer_tam);
            newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); // Aceita novas conexoes
            gettimeofday(&start, NULL); // Inicia o cronometro para proxima conexao
        }
    }
    tam++;
    return tam;
}

int server_uni(int newsockfd, int sockfd, int portno, char *buffer, int tam, int buffer_tam, FILE *fp){
    char *nome_arquivo;
    int n, bytes_enviados;
    struct timeval start;
    struct timeval end;
    struct sockaddr_in cli_addr;
    socklen_t clilen;
    n = read(newsockfd, &buffer[tam], 1);
    if (n < 0){
        error("ERROR reading from socket");
    }
    if(buffer[tam] == '0'){ // cliente enviou o nome do arquivo inteiro
        buffer[tam] = '\0'; // Terminador de string: sumir com o 0 do final do arquivo
        nome_arquivo = (char*)calloc(strlen(buffer), sizeof(char));
        strcpy(nome_arquivo, buffer);
        fp = fopen(buffer, "r");
        tam = -1;
        bzero(buffer, buffer_tam);
        if (fp == NULL){ // Se nao foi possivel abrir o arquivo
            close(newsockfd);
            newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        }else{ // Se conseguiu abrir o arquivo
            bytes_enviados = enviar_arquivo_uni(newsockfd, buffer, fp, buffer_tam); // Envia o arquivo pro cliente
            close(newsockfd); // Fecha a conexao
            gettimeofday(&end, NULL); // Finaliza o cronometro

            printf("Quantidade de bytes enviados: %5u byte(s). Tempo gasto de envio do arquivo \"%s\": %.2fs\n", bytes_enviados, nome_arquivo, time_diff(&start, &end));
            free(nome_arquivo);
            printf("\n\nAguardando novas conexões...\n\n");
            newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); // Aceita novas conexoes
            gettimeofday(&start, NULL); // Inicia o cronometro para proxima conexao
        }
    }
    tam++;
    return tam;
}
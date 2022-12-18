#include "client.h"

void error(const char *msg){
    perror(msg);
    exit(0);
}

float time_diff(struct timeval *start, struct timeval *end){ // Função para calcular tempo de execução
    return (end->tv_sec - start->tv_sec) + 1e-6*(end->tv_usec - start->tv_usec);
}

int open_sock(){
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        error("ERROR opening socket");
    }
    return sockfd;
}

void open_connect(int portno, int sockfd, char *hostname){
    struct sockaddr_in serv_addr;
    struct hostent *server;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    server = gethostbyname(hostname);
    if (server == NULL){
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
        error("ERROR connecting\n");
    }
    printf("connection created\n");
}

void send_filename(int sockfd, char *buffer, char *filename, int buffer_tam){
    int i;
    bzero(buffer, buffer_tam);
    strcpy(buffer, filename); // Pega o nome do arquivo digitado na linha de compilacao

    for(i = 0; i < strlen(buffer); i++){ // Envia o nome do arquivo pro servidor
        write(sockfd, &buffer[i], 1);
    }
    buffer[i] = '0'; // Coloca o terminador do nome do arquivo (especificacao do trabalho)
    write(sockfd, &buffer[i], 1);
    bzero(buffer, buffer_tam);
    printf("filename sended to server\n");
}

void client_uni(int sockfd, int buffer_tam, char *buffer, FILE *fp, int *bytes_recebidos){
    bzero(buffer, buffer_tam);
    recv(sockfd, buffer, buffer_tam, 0); // Recebe um buffer de BUFFER_TAM bytes do servidor
    for(int i = 0; i < strlen(buffer); i++){
        fwrite(&buffer[i], 1, sizeof(char), fp); // Escreve cada valor recebido
        (*bytes_recebidos)++;
    }
}

void enviar_ack(int sockfd, int byte_atual){
    char valor[20]; // considerando que o número nao terá mais que 20 números
    sprintf(valor, "%i", byte_atual); // Transforma int em string
    write(sockfd, valor, 20);
}

int recebe_blocos(int sockfd, int bytes_recebidos, char *buffer, int buffer_tam, int tam, FILE *fp){ // Recebe o buffer do servidor em blocos de buffer do cliente
    int i = 0, j;
    while(i < tam){ // ler todos os bytes recebidos
        bzero(buffer,buffer_tam);
        if((i + buffer_tam) > tam){ // O buffer do cliente é maior que a quantidade de dados que faltam pra ler
            for(j = 0; i < tam ; j++){ // Pega o restante dos dados que faltam dentro de cada envio
                read(sockfd, &buffer[j], 1); // Recebe um buffer de BUFFER_TAM bytes do cliente
                i++;
            }
            fwrite(buffer, strlen(buffer), sizeof(char), fp); // Escreve cada valor recebido
            bytes_recebidos+= j;
        }else{ // O buffer do cliente pode ser usado inteiro
            recv(sockfd, buffer, buffer_tam, 0); // Recebe um buffer de BUFFER_TAM bytes do cliente
            fwrite(buffer, buffer_tam, sizeof(char), fp); // Escreve cada valor recebido
            i += buffer_tam;
            bytes_recebidos += buffer_tam;
        }
    }
    return bytes_recebidos;
}

void client_rr(int sockfd, int buffer_tam, char *buffer, FILE *fp, int *bytes_recebidos){
    int tam;
    bzero(buffer, buffer_tam);
    read(sockfd, buffer, 20); // Descobrir quantos bytes deverão ser lidos
    tam = atoi(buffer); // Armazena esse valor
    *bytes_recebidos = recebe_blocos(sockfd, *bytes_recebidos, buffer, buffer_tam, tam, fp);
    enviar_ack(sockfd, *bytes_recebidos); // Envia qual byte o cliente está esperando
}

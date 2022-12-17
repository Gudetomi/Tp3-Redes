/* Servidor TCP Unidirecional - Envio de arquivos com tamanhos diferentes, bem como buffer de tamanhos diferentes
    Antônio Pereira de Souza Júnior - 2022103670
    Laís Fagundes Moraes - 2022103732
    Wagner Lancetti - 2022103705
    Gustavo Henrique Alves Detomi - 172050107 
*/
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

void error(const char *msg){
    perror(msg);
    exit(0);
}

float time_diff(struct timeval *start, struct timeval *end){ // Função para calcular tempo de execução
    return (end->tv_sec - start->tv_sec) + 1e-6*(end->tv_usec - start->tv_usec);
}

char* Receber_ACK(int newsockfd, char *buffer, int BUFFER_TAM){
    read(newsockfd, buffer, 20);
    return buffer;
}


int Enviar_Arquivo(int newsockfd, char buffer[], FILE *fp, int BUFFER_TAM){
    int tam = 0, read_return = 0, bytes_enviados = 0;
    long int intervalo = 0;
    char valor[20];

    bzero(buffer, BUFFER_TAM);
    while(!feof(fp)){
        read_return = fread(&buffer[tam], sizeof(char), 1, fp); // Lê um byte por vez do arquivo
        tam++;
        if (read_return == 0 || tam == BUFFER_TAM){ // Se terminou de ler o arquivo, ou o buffer está cheio
            bytes_enviados += tam;
            sprintf(valor, "%i", tam);
            write(newsockfd, valor, 20); // manda quantos bytes o cliente precisa esperar
            write(newsockfd, buffer, tam); // Manda todo o buffer pro cliente
            bzero(buffer, BUFFER_TAM);
            buffer = Receber_ACK(newsockfd, buffer, BUFFER_TAM); // Recebe qual o próximo byte que o cliente está esperando
            intervalo = atoi(buffer);
            if(intervalo != bytes_enviados){ // Se o cliente recebeu menos bytes do que era esperado
                fseek(fp, intervalo, SEEK_SET); // Move o ponteiro do arquivo pro byte que o cliente está esperando
            }
            tam = 0;
            bzero(buffer, BUFFER_TAM);
        }
    }
    return bytes_enviados;
}


int main(int argc, char *argv[]){
    int sockfd, newsockfd, portno, tam = 0, bytes_enviados = 0;
    socklen_t clilen;
    FILE *fp;
    char *buffer;
    char *nome_arquivo;
    struct sockaddr_in serv_addr, cli_addr;
    int n, BUFFER_TAM;
    struct timeval start;
    struct timeval end;
    
    if (argc < 3){
        fprintf(stderr,"Modo de uso: %s Porta_Servidor Tam_Buffer\n", argv[0]);
        exit(1);
    }

    BUFFER_TAM = atoi(argv[2]);
    buffer = (char*) calloc(BUFFER_TAM, sizeof(char));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        error("ERROR opening socket");
    } 
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
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
    bzero(buffer,BUFFER_TAM);

    gettimeofday(&start, NULL); // Inicia o cronometro primeira conexao

    while(1){
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
            bzero(buffer, BUFFER_TAM);
            if (fp == NULL){ // Se nao foi possivel abrir o arquivo
                close(newsockfd);
                newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
            }else{ // Se conseguiu abrir o arquivo
                bytes_enviados = Enviar_Arquivo(newsockfd, buffer, fp, BUFFER_TAM); // Envia o arquivo pro cliente
                
                close(newsockfd); // Fecha a conexao
                gettimeofday(&end, NULL); // Finaliza o cronometro
                printf("Quantidade de bytes enviados: %u byte(s). Tempo gasto de envio do arquivo \"%s\": %.2fs\n", bytes_enviados, nome_arquivo, time_diff(&start, &end));
                free(nome_arquivo);
                printf("\nAguardando novas conexões...\n\n");
                bzero(buffer, BUFFER_TAM);
                newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); // Aceita novas conexoes
                gettimeofday(&start, NULL); // Inicia o cronometro para proxima conexao
            }
        }
        tam++;
    }
    close(newsockfd);
    close(sockfd);
    return 0; 
}
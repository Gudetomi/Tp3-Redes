/* Cliente TCP Requisição-Resposta - Envio de arquivos com tamanhos diferentes, bem como buffer de tamanhos diferentes
    Antônio Pereira de Souza Júnior - 2022103670
    Laís Fagundes Moraes - 2022103732
    Wagner Lancetti - 2022103705
    Gustavo Henrique Alves Detomi - 172050107 
*/

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

void error(const char *msg){
    perror(msg);
    exit(0);
}

float time_diff(struct timeval *start, struct timeval *end){ // Função para calcular tempo de execução
    return (end->tv_sec - start->tv_sec) + 1e-6*(end->tv_usec - start->tv_usec);
}

void Enviar_ACK(int sockfd, int byte_atual){
    char valor[20]; // considerando que o número nao terá mais que 20 números
    sprintf(valor, "%i", byte_atual); // Transforma int em string
    write(sockfd, valor, 20);
}

int Recebe_Blocos(int sockfd, int bytes_recebidos, char *buffer, int BUFFER_TAM, int tam, FILE *fp){ // Recebe o buffer do servidor em blocos de buffer do cliente
    int i = 0, j;
    while(i < tam){ // ler todos os bytes recebidos
        bzero(buffer,BUFFER_TAM);
        if((i + BUFFER_TAM) > tam){ // O buffer do cliente é maior que a quantidade de dados que faltam pra ler
            for(j = 0; i < tam ; j++){ // Pega o restante dos dados que faltam dentro de cada envio
                read(sockfd, &buffer[j], 1); // Recebe um buffer de BUFFER_TAM bytes do cliente
                i++;
            }
            fwrite(buffer, strlen(buffer), sizeof(char), fp); // Escreve cada valor recebido 
            bytes_recebidos+= j;
        }else{ // O buffer do cliente pode ser usado inteiro
            recv(sockfd, buffer, BUFFER_TAM, 0); // Recebe um buffer de BUFFER_TAM bytes do cliente
            fwrite(buffer, BUFFER_TAM, sizeof(char), fp); // Escreve cada valor recebido 
            i += BUFFER_TAM;
            bytes_recebidos += BUFFER_TAM;
        }
    }
    return bytes_recebidos;
}

int main(int argc, char *argv[]){
    int sockfd, portno, bytes_recebidos = 0;
    int BUFFER_TAM, tam;
    int i;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    FILE *fp;
    char *buffer;
    struct timeval start;
    struct timeval end;

    if (argc < 5){
       fprintf(stderr,"Modo de uso: %s Host_do_Servidor Porta_Servidor Nome_Arquivo Tam_Buffer\n", argv[0]);
       exit(0);
    }
    BUFFER_TAM = atoi(argv[4]); // Buffer com tamanho digitado pelo usuário
    buffer = (char*)calloc(BUFFER_TAM, sizeof(char));

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        error("ERROR opening socket");
    }
    server = gethostbyname(argv[1]);
    if (server == NULL){
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
        error("ERROR connecting");
    }

    fp = fopen("saida.txt", "wa"); // Arquivo que será guardado os dados lidos

    gettimeofday(&start, NULL); // Inicia o cronometro

    bzero(buffer, BUFFER_TAM);
    strcpy(buffer, argv[3]); // Pega o nome do arquivo digitado na linha de compilacao
    
    for(i = 0; i < strlen(buffer); i++){ // Envia o nome do arquivo pro servidor
        write(sockfd, &buffer[i], 1);
    }
    buffer[i] = '0'; // Coloca o terminador do nome do arquivo (especificacao do trabalho)
    write(sockfd, &buffer[i], 1);

    while(1){ // Receber todo o arquivo
        bzero(buffer,BUFFER_TAM);
        read(sockfd, buffer, 20); // Descobrir quantos bytes deverão ser lidos
        tam = atoi(buffer); // Armazena esse valor
        bytes_recebidos = Recebe_Blocos(sockfd, bytes_recebidos, buffer, BUFFER_TAM, tam, fp);
        Enviar_ACK(sockfd, bytes_recebidos); // Envia qual byte o cliente está esperando
        if (strlen(buffer) == 0){ // Terminou a leitura
            break;
        }
    }
    close(sockfd); // Fecha conexao
    fclose(fp); // Fecha arquivo

    gettimeofday(&end, NULL); // Finaliza o cronometro
    printf("Buffer = %u byte(s). Tempo gasto de envio %.2fs\n", bytes_recebidos, time_diff(&start, &end));
    
    return 0;
}
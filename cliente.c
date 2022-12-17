/* Cliente TCP Unidirecional - Envio de arquivos com tamanhos diferentes, bem como buffer de tamanhos diferentes
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

int main(int argc, char *argv[]){
    int sockfd, portno, i, bytes_recebidos = 0;
    // double kbps = 0;
    int BUFFER_TAM;
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
    BUFFER_TAM = atoi(argv[4]); // Tamanho definido no terminal
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
    bzero(buffer, BUFFER_TAM);

    while(1){ // Receber todo o arquivo
        recv(sockfd, buffer, BUFFER_TAM, 0); // Recebe um buffer de BUFFER_TAM bytes do servidor
        for(i = 0; i < strlen(buffer); i++){
            fwrite(&buffer[i], 1, sizeof(char), fp); // Escreve cada valor recebido 
            bytes_recebidos++;
        }
        if (strlen(buffer) == 0){
            break;
        }
        bzero(buffer,BUFFER_TAM);
    }
    close(sockfd); // Fecha conexao
    fclose(fp); // Fecha arquivo

    gettimeofday(&end, NULL); // Finaliza o cronometro
    // kbps = bytes_recebidos/1000;
    // printf("Buffer = %5u byte(s), %10.2f kbps (u bytes em %3u.%06u s)",bytes_recebidos, kbps, bytes_recebidos, time_diff(&start, &end));
    printf("Buffer = %5u byte(s). Tempo gasto de envio %.2fs\n", bytes_recebidos+1, time_diff(&start, &end));
    
    return 0;
}

/* Servidor TCP Unidirecional - Envio de arquivos com tamanhos diferentes, bem como buffer de tamanhos diferentes
    Antônio Pereira de Souza Júnior - 2022103670
    Laís Fagundes Moraes - 2022103732
    Wagner Lancetti - 2022103705
    Gustavo Henrique Alves Detomi - 172050107 
*/
#include "server.h"

int main(int argc, char *argv[]){
    int sockfd, newsockfd, portno, tam = 0;
    struct timeval start;

    FILE *fp=NULL;
    char *buffer;
    int BUFFER_TAM;

    if (argc < 3){
        fprintf(stderr,"Modo de uso: %s Porta_Servidor Tam_Buffer\n", argv[0]);
        exit(1);
    }

    BUFFER_TAM = atoi(argv[2]);
    buffer = (char*) calloc(BUFFER_TAM, sizeof(char));

    sockfd = open_sock();

    portno = atoi(argv[1]);
    newsockfd = open_connect(portno, sockfd);

    bzero(buffer,BUFFER_TAM);

    gettimeofday(&start, NULL); // Inicia o cronometro primeira conexao

    while(1){
        tam = server_uni(newsockfd, sockfd, portno, buffer, tam, BUFFER_TAM, fp);
    }
    close(newsockfd);
    close(sockfd);
    return 0;
}

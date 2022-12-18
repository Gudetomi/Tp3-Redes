/* Cliente TCP Unidirecional - Envio de arquivos com tamanhos diferentes, bem como buffer de tamanhos diferentes
    Antônio Pereira de Souza Júnior - 2022103670
    Laís Fagundes Moraes - 2022103732
    Wagner Lancetti - 2022103705
    Gustavo Henrique Alves Detomi - 172050107 
*/

#include "client.h"

int main(int argc, char *argv[]){
    int sockfd, portno, bytes_recebidos=0;
    int BUFFER_TAM;

    FILE *fp;
    char *buffer;
    struct timeval start;
    struct timeval end;

    if (argc < 5){
        fprintf(stderr, "Modo de uso: %s Host_do_Servidor Porta_Servidor Nome_Arquivo Tam_Buffer\n", argv[0]);
        exit(0);
    }

    BUFFER_TAM = atoi(argv[4]); // Tamanho definido no terminal
    buffer = (char*)calloc(BUFFER_TAM, sizeof(char));

    portno = atoi(argv[2]);
    sockfd = open_sock();
    printf("%d\n", sockfd);

    open_connect(portno, sockfd, argv[1]);

    fp = fopen("saida.txt", "wa"); // Arquivo que será guardado os dados lidos

    gettimeofday(&start, NULL); // Inicia o cronometro

    send_filename(sockfd, buffer, argv[3], BUFFER_TAM);

    while(1){ // Receber todo o arquivo
        client_uni(sockfd, BUFFER_TAM, buffer, fp, &bytes_recebidos);
        if (strlen(buffer) == 0){
            break;
        }
    }
    close(sockfd); // Fecha conexao
    fclose(fp); // Fecha arquivo
    free(buffer);

    gettimeofday(&end, NULL); // Finaliza o cronometro
    printf("Buffer = %5u byte(s). Tempo gasto de envio %.2fs\n", bytes_recebidos+1, time_diff(&start, &end));
    return 0;
}

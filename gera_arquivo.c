#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(){
    FILE *fp = fopen("arquivo_teste.txt","w");
    long int cont = 0, cont2 = 0;
    char valor[1];
    char *quebra = "\n";
    char *espaco = " ";
    srand(time(NULL));
    while(cont < 10000000){
        valor[0] = rand() % 52;
        if (valor[0] < 26){ // Maiscula
            valor[0] += 65;
        }else{ // Minuscula
            valor[0] += 71;
        }
        fprintf(fp, valor, "%s");
        if (cont2 % 8 == 0 && cont2 != 0){
            fprintf(fp, espaco, "%s");
            cont++;
            cont2++;
        }
        if (cont2 == 30 && cont + 1 != 1000000){
            fprintf(fp, quebra, "%s");
            cont++;
            cont2 = -1;
        }
        cont++;
        cont2++;
    }
    fclose(fp);
}
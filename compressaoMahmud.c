#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


//RANDOM NUMBER
int serieRandomica(int min, int max){
    return(rand() % (max - min)) + min;
}

//IMPRIME O VETOR
void imprimeVetor(int *vetor, int inicio, int fim){
    int i;
    
    printf("\t\t\t");
    for(i = inicio; i <= fim; i++){
        printf("%d ", vetor[i]);
    }
}

/*
* Tabela verdade da técnica proposta
*
* 0 and 0 -> 0 = 0
* 1 and 1 -> 1 = 1
* 0 and 1 -> \0 = 2
* 1 and 0 -> \1 = 3
*/

int comprimeDados(int val1, int val2){
    if(val1 == 0 && val2 == 0)
        return 0;
    else if(val1 == 1 && val2 == 1)
        return 1;
    else if(val1 == 0 && val2 == 1)
        return 2;
    else if(val1 == 1 && val2 == 0)
        return 3;
    else{
        printf("Error"); exit(0);
    }
}

int descomprimeDados(int val1){
    if(val1 == 0)
       	return 00;
    else if(val1 == 1)
        return 11;
    else if(val1 == 2)
        return 01;
    else if(val1 == 3)
        return 10;
    else{
        printf("Error"); exit(0);
    }
}

//TAMANHO DO VETOR PRECISA SER SEMPRE MULTIPLO DE 2
bool multiploDeDois(int valor){
    if(valor%2 == 0){
        return true;
    }else{
        return false;
    }
}

// GERA ARRAY ALEATORIO
// O vetor aleatório é gerado a partir de um random de 0 ou 1
int arrayAleatorio(int *array, int tamVetor){
     int i;
     
     for(i = 0; i < tamVetor; i++){
         int random = serieRandomica(0,2);
         array[i] = random;
     }
     return *array;
}

//GERA O "Y" A PARTIR DO ARRAY
int geraYRandom(int *y, int tamVetor, int *array){
    int i;
    
    for(i = 1; i < tamVetor; i++){
        int random = serieRandomica(0, tamVetor);
        y[i] = array[random];
    }
    return *y;
}

//GERA O VETOR "Y" NAO ALEATORIO
// Vetor Y é gerado a partir do vetor ARRAY não aleatório
int geraY(int *y, int tamVetor, int *array){
    int i;
    
    for(i = 0; i < tamVetor; i++){
        y[i] = array[i];
        printf("%d", y[i]);
    }
    return *y;
}

// PREEECHER VETOR CASO O USUARIO NAO QUEIRA GERAR ALEATORIO
int preencheArray(int *array, int tamVetor){
    int i, val;
    
    for(i = 0; i < tamVetor; i++){
        printf("\t\tVALOR %d:", i+1);
        scanf("%d", &val);
        
		array[i] = val;
    }
    return *array;
}

// GERA O VETOR Z
// Vetor Z é resultado do (Xor X and Y) 
int vetorZ(int *z, int tamVetor, int *x, int *y){
    int i;
    
    for(i = 0; i < tamVetor; i++){
        int or = x[i]|x[i];
        int and = (or != y[i]) ? 1 : 0;
        z[i] = and;
    }
    
    printf("\n\t\t\t>>Vetor Z<<\n");
    imprimeVetor(z, 0, tamVetor-1);
    
	return *z;
}

//RESULTADO DA COMPACTACAO
int compactacao(int *compacta, int tamVetor, int *z){
    int i, j = 0;

    for(i = 0; i < tamVetor; i+=2){
        compacta[j] = comprimeDados(z[i], z[i+1]);
        j++;
    }

    printf("\n\n\t\t\t>>Vetor Compactado<<\n");
    imprimeVetor(compacta, 0, ((tamVetor-1)/2));
    return *compacta;
}

//RESULTADO DA DESCOMPACTACAO
int descompactacao(int *descompacta, int tamVetor, int *compacta){
    int i, j = 0;

    for(i = 0; i < tamVetor; i++){
        descompacta[j] = descomprimeDados(compacta[i]);
        j++;
    }

    printf("\n\t\t\t>>Vetor Descompactado<<\n");
    imprimeVetor(descompacta, 0, ((tamVetor-1)/2));
    return *descompacta;
}

// Imprime os vetores finais, array, x, y
void imprimeValorVetores(int *array, int *x, int *y, int tamVetor){

    printf("\t\t\t>>Vetor RANDOM<<\n");
    imprimeVetor(array, 0, tamVetor-1);

    printf("\n\t\t\t>>Vetor X<<\n");
    imprimeVetor(x, 0, tamVetor-1);
    
    printf("\n\t\t\t>>Vetor Y<<\n");
    imprimeVetor(y, 0, tamVetor-1);
}


// PRINCIPAL
int main(){
	setlocale(LC_ALL, "Portuguese");
	
    int tamVetor, gerar;
    int array[tamVetor];
    int x[20] = {1,0,1,1,0,1,0,1,0,0,1,1,0,0,1,1,0,0,1,0};
    int y[tamVetor]; //y gerado apartir do array
    int z[tamVetor]; // vetor criptografado
    int compacta[tamVetor/2]; // vetor comprimido
    int descompacta[tamVetor]; // vetor descomprimido

    //int array[20] = {0,1,0,1,0,0,1,1,0,1,1,0,1,0,1,0,1,0,1,0}; // para teste igual ao artigo
    //int y[20] = {0,1,0,1,0,0,1,1,0,1,1,0,1,0,1,0,1,0,1,0};  // para teste igual ao artigo
    
    //int array[];

    srand(time(NULL));

    //ENTRADA TAMANHO DO VETOR
    printf("\t\tINFORME O TAMANHO DO VETOR (com valor máximo de 20 e múltiplo de 2): ");
    scanf("%d", &tamVetor);
	printf("\n");
	
    if(!multiploDeDois(tamVetor)){
		printf("\n\n\n\t\tO VALOR DIGITADO NAO É MULTIPLO DE 2\n\n\n");
    }else{
    	printf("\t\tDESEJA GERAR VALORES ALEATÓRIOS? (1-sim / 2-não) ");
        scanf("%d", &gerar);
        printf("\n");

        if(gerar == 1){
			//random array
        	arrayAleatorio(array, tamVetor);
        	geraYRandom(y, tamVetor, array);
        	imprimeValorVetores(array, x, y, tamVetor);
        	vetorZ(z, tamVetor, x, y);
        	compactacao(compacta, tamVetor, z);
        	descompactacao(descompacta, tamVetor, compacta);
	    }else if(gerar == 2){
            preencheArray(array, tamVetor);
            geraY(y, tamVetor, array);
        	imprimeValorVetores(array, x, y, tamVetor);
            vetorZ(z, tamVetor, x, y);
            compactacao(compacta, tamVetor, z);
            descompactacao(descompacta, tamVetor, compacta);
        }
    }
}

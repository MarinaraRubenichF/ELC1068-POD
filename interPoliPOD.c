#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TAM 100

unsigned int contaLeituras = 0;
unsigned int contaEscritas = 0;
unsigned int contaFases = 0;
unsigned int nRegistros = 10;

struct diaMesAno{
	int dia;
	int mes;
	int ano;
};

struct diaMesAno getRandDate(){
	struct tm * estTempo;
	time_t tempo;
	struct diaMesAno date;
	tempo = rand();
	tempo *= 60 * 60 * 24;
	estTempo = gmtime(&tempo);
	date.dia = estTempo->tm_mday;
	date.mes = estTempo->tm_mon + 1;
	date.ano = estTempo->tm_year + 1900;
	return date;
}

int validaData(int dd, int mm, int aaaa){
	int dias = 31;
	
	if(mm < 1 || mm > 12){
		return 0;
	}
	if(dd < 1){
		return 0;
	}
	if(mm == 2){
		dias = 28;
		if(aaaa % 400 == 0 || (aaaa % 4 == 0 && aaaa % 100 != 0)){
			dias = 29;
		}
	}
	else if(mm == 4 || mm == 6 || mm == 9 || mm == 11){
		dias = 30;
	}
	if(dd > dias){
		return 0;
	}
	return 1;
}

int comparaDatas(struct diaMesAno * comp1, struct diaMesAno * comp2){
	if(comp1->ano > comp2->ano)
		return 1;
	else if(comp1->ano < comp2->ano)
		return -1;
	else if(comp1->mes > comp2->mes)
		return 1;
	else if(comp1->mes < comp2->mes)
		return -1;
	else if(comp1->dia > comp2->dia)
		return 1;
	else if(comp1->dia < comp2->dia)
		return -1;
	else
		return 0;
}

struct arquivo{
	char *nome;	//Nome do Arquivo
	int blocos, posBuffer, simulados, numRegistros, posicao;	//Blocos existentes em cada fita, Posição do registro no início do Buffer (região da RAM, armazena temp dados em movimento),  
	//Número de Registros no Arquivo, Posicao do registro a ser lido, 
	struct diaMesAno buffer[TAM];
	struct diaMesAno ultimo;
	fpos_t fpost; //Especifica uma posição dentro do arquivo e grava-o na fpost (ponteiro para fpost_t)
};

int carregaArquivo(struct arquivo * meuArquivo){
	FILE * fileStream;
	
	fileStream = fopen(meuArquivo->nome, "rb");
	
	if(fileStream){
		contaLeituras++;
		fsetpos(fileStream, &(meuArquivo->fpost));
		if (meuArquivo->numRegistros < meuArquivo->posicao + TAM) {
			fread(meuArquivo->buffer, sizeof(struct diaMesAno), meuArquivo->numRegistros % TAM, fileStream);
		}
		else {
			fread(meuArquivo->buffer,sizeof(struct diaMesAno), TAM, fileStream);
		}
		fgetpos(fileStream, &(meuArquivo->fpost));
		fclose(fileStream);
	}
	else {
		printf("\tO arquivo não pode ser aberto!\n");
		return -1;
	}
}

void escreveRegistro(struct arquivo * meuArquivo, struct diaMesAno registro){
	FILE * fileStream;
	
	if(meuArquivo->posBuffer == -1)
		meuArquivo->posBuffer = 0;
	
	meuArquivo->numRegistros++;
	meuArquivo->ultimo = registro;
	meuArquivo->buffer[meuArquivo->posBuffer++] = registro;
	
	if(meuArquivo->posBuffer >= TAM) {
		meuArquivo->posBuffer %= TAM;
		meuArquivo->posicao += TAM;
		if(meuArquivo->posicao == TAM)
			fileStream = fopen(meuArquivo->nome, "wb");
		else
			fileStream = fopen(meuArquivo->nome, "ab");
		if(fileStream){
			fsetpos(fileStream, &(meuArquivo->fpost));
			fwrite(meuArquivo->buffer, sizeof(struct diaMesAno), TAM, fileStream);
			fgetpos(fileStream, &(meuArquivo->fpost));
			fclose(fileStream);
			
			contaEscritas++;
		}
		else {
			printf("O arquivo de fita não pode ser aberto!\n");
			return;
		}
	}
	return;
}

void imprimeArquivo(struct arquivo * meuArquivo){
	int i, numRegistros = TAM, roda = 1, rodou = 0;
	fpos_t posicao = 0;
	struct diaMesAno * data;
	
	FILE * fileStream;
	fileStream = fopen(meuArquivo->nome, "rb");	
	
	if(fileStream){
		struct diaMesAno anterior;
		
		printf("\n");
		printf("\t\t######################\n");
		if(meuArquivo->nome != "INTER_POLIFASICA")	
			printf("\t\t\t%s\n", meuArquivo->nome);
		else
			printf("\t\t   %s\n", meuArquivo->nome);
		printf("\n");
		
		data = (struct diaMesAno *)malloc(meuArquivo->numRegistros * sizeof(struct diaMesAno));		
		
		while(posicao < meuArquivo->numRegistros * sizeof(struct diaMesAno)) {
			if(posicao/sizeof(struct diaMesAno) + TAM >= meuArquivo->numRegistros) {
				numRegistros = meuArquivo->numRegistros - posicao/sizeof(struct diaMesAno);
			}
			fsetpos(fileStream, &(posicao));
			fread(data, sizeof(struct diaMesAno), numRegistros, fileStream);
			
			for(i = 0; i < numRegistros; i++) {
				if(roda > 1 && comparaDatas(&anterior, &(data[i])) > 0)
					printf("\t         .%d-----------------\n", ++rodou);
				printf("\t\t    _%d - %2d %2d %4d\n", roda++, data[i].dia, data[i].mes, data[i].ano);
				anterior = data[i];
			}
			posicao += sizeof(struct diaMesAno) * TAM;
		}
		
		if(roda)
			printf("\t         .%d-----------------\n", ++rodou);
		//else
			//printf("Nenhum registro nessa fase...");
		printf("\n");
		
		free(data);
		fclose(fileStream);
	}
}

void limpaArquivo(struct arquivo *meuArquivo) {
	FILE * fileStream;
	
	meuArquivo->posBuffer = -1;
	meuArquivo->simulados = 0;
	meuArquivo->fpost = 0;
	meuArquivo->blocos = 0;
	meuArquivo->posicao = 0;
	meuArquivo->numRegistros = 0;
	
	fileStream = fopen(meuArquivo->nome, "wb");
	if (fileStream)
		fclose(fileStream);
}

char proxRegistro(struct arquivo * meuArquivo) {
	FILE * fileStream;

	if(meuArquivo->numRegistros == 0)
		return 0;
	if(meuArquivo->posicao + meuArquivo->posBuffer < meuArquivo->numRegistros)
		return 1;
		
	limpaArquivo(meuArquivo);
	
	return 0;
}

struct diaMesAno * pegaRegistro(struct arquivo * meuArquivo) {
	if(proxRegistro(meuArquivo) == 0)
		return NULL;
	if(meuArquivo->posBuffer >= TAM) {
		meuArquivo->posBuffer -= TAM;
		meuArquivo->posicao += TAM;
		carregaArquivo(meuArquivo);
	}
	if(meuArquivo->posBuffer == -1){
		meuArquivo->posBuffer = 0;
		carregaArquivo(meuArquivo);
	}
	return &(meuArquivo->buffer[meuArquivo->posBuffer]);
}

void acessaPonteiro(struct arquivo * meuArquivo){
	FILE * fileStream;
	
	fileStream = fopen(meuArquivo->nome, "ab");
	
	if(meuArquivo->numRegistros == 0 || meuArquivo->posBuffer == -1)
		return;
	if(fileStream){
		fsetpos(fileStream, &(meuArquivo->fpost));
		fwrite(meuArquivo->buffer, sizeof(struct diaMesAno), meuArquivo->posBuffer, fileStream);
		rewind(fileStream);
		fgetpos(fileStream, &(meuArquivo->fpost));
		fclose(fileStream);
	
		contaEscritas++;
	
		meuArquivo->posBuffer = 0;
	}
	else {
		printf("\tO arquivo de fita não pode ser aberto! %s\n", meuArquivo->nome);
		return;
	}
}

void trocaPonteiro(struct arquivo * meuArquivo){
	meuArquivo->posBuffer++;
}

void reiniciaPonteiro(struct arquivo * meuArquivo){
	meuArquivo->posBuffer = -1;
	meuArquivo->fpost = 0;
	meuArquivo->posicao = 0;
}

void distribuiRegistros(struct arquivo * origem, struct arquivo * arq1, struct arquivo * arq2){
	char p = 1;	int n = 1;
	struct diaMesAno temp;
	
	while(proxRegistro(origem)){
		temp = *(pegaRegistro(origem));
		trocaPonteiro(origem);
		
		if(arq1->numRegistros == 0 || p == 1 && !(n == 0 && (comparaDatas(&(arq1->ultimo), &temp) > 0)) 
			|| p == 2 && n == 0 && (arq2->numRegistros == 0 || comparaDatas(&(arq2->ultimo), &temp) > 0)){
			if(p == 2){
				p = 1;
				n = arq2->blocos;
			}
			if(arq1->numRegistros == 0 || comparaDatas(&(arq1->ultimo), &temp) > 0){
				n--;
				arq1->blocos++;
			}
			escreveRegistro(arq1, temp);
		}
		else{
			if(p == 1){
				p = 2;
				n = arq1->blocos;
			}
			if(arq2->numRegistros == 0 || comparaDatas(&(arq2->ultimo), &temp) > 0){
				n--;
				arq2->blocos++;
			}
			escreveRegistro(arq2, temp);
		}
	}
	acessaPonteiro(arq1);
	reiniciaPonteiro(arq1);
	acessaPonteiro(arq2);
	reiniciaPonteiro(arq2);
	
	if (p == 1)
		arq1->simulados = n;
	else
		arq2->simulados = n;
	return;
}

void uneRegistros(struct arquivo * entrada1, struct arquivo * entrada2, struct arquivo * saida){
	saida->blocos++;
	entrada1->blocos--;
	entrada2->blocos--;
	
	struct diaMesAno ultimo1 = {0, 0, 0}, ultimo2 = {0, 0, 0}, atual1 = *(pegaRegistro(entrada1)), atual2 = *(pegaRegistro(entrada2));
	char recente1 = 0, recente2 = 0;

	while(proxRegistro(entrada1) && proxRegistro(entrada2) && (recente1 == 0 || comparaDatas(&ultimo1, &atual1) <= 0) &&
	(recente2 == 0 || comparaDatas(&ultimo2, &atual2) <= 0)){
		if(comparaDatas(&atual1, &atual2) <= 0){
			escreveRegistro(saida, atual1);
			trocaPonteiro(entrada1);
			recente1 = 1;
			ultimo1 = atual1;
			
			if(!proxRegistro(entrada1)) {
				atual1.mes--;
				break;
			}
			atual1 = *(pegaRegistro(entrada1));
		}
		else{
			escreveRegistro(saida, atual2);
			trocaPonteiro(entrada2);
			recente2 = 1;
			ultimo2 = atual2;
			
			if (!proxRegistro(entrada2)) {
				atual2.mes--;
				break;
			}
			atual2 = *(pegaRegistro(entrada2));
		}
	}
	while(proxRegistro(entrada1) && (recente1 == 0 || comparaDatas(&ultimo1, &atual1) <= 0)){
		escreveRegistro(saida, atual1);
		trocaPonteiro(entrada1);
		
		recente1 = 1;
		ultimo1 = atual1;
		 
		if(!proxRegistro(entrada1))
			break;
		atual1 = *(pegaRegistro(entrada1));
	}
	while(proxRegistro(entrada2) && (recente2 == NULL || comparaDatas(&ultimo2, &atual2) <= 0)){
		escreveRegistro(saida, atual2);
		trocaPonteiro(entrada2);
		recente2 = 1;
		ultimo2 = atual2;
		
		if (!proxRegistro(entrada2))
			break;
		atual2 = *(pegaRegistro(entrada2));
	}
}

void interPolifasica(struct arquivo * meuArquivo){
	struct arquivo * a1, *a2, *a3;
	struct diaMesAno registro;
	int i;
	
	a1 = calloc(1, sizeof(struct arquivo));
	a1->nome = "FITA-1";
	a1->posBuffer = -1;
	a2 = calloc(1, sizeof(struct arquivo));
	a2->nome = "FITA-2";
	a2->posBuffer = -1;
	a3 = calloc(1, sizeof(struct arquivo));
	a3->nome = "FITA-3";
	a3->posBuffer = -1;

	distribuiRegistros(meuArquivo, a1, a2);
	
	printf("\n");
	printf("\t%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
	printf("\t\t\tFASE-%d:\n", contaFases+1);
	imprimeArquivo(a1);
	imprimeArquivo(a2);
	imprimeArquivo(a3);
	printf("\t%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
	
	if(a2->simulados + a2->blocos > a1->blocos + a1->simulados){
		struct arquivo *temp;
		temp = a2;
		a2 = a1;
		a1 = temp;
	}

	while(a1->simulados || a1->simulados == 0 && a3->numRegistros && comparaDatas(&(a3->ultimo), pegaRegistro(a2)) <= 0){
		if(a3->numRegistros == 0 || comparaDatas(&(a3->ultimo), pegaRegistro(a2)) > 0){
			a1->simulados--;
			a3->blocos++;
			a2->blocos--;
		}
		escreveRegistro(a3, *(pegaRegistro(a2)));
		trocaPonteiro(a2);
	}
	
	while(a1->blocos > 1){
		int k = a2->blocos;
		
		for(i = 0; i < k; i++){
			uneRegistros(a2, a1, a3);
		}		
		acessaPonteiro(a3);
		reiniciaPonteiro(a3);
		contaFases++;
		
		printf("\n");
		printf("\t%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
		printf("\t\t\tFASE-%d:\n", contaFases+1);
		imprimeArquivo(a1);
		imprimeArquivo(a2);
		imprimeArquivo(a3);
		printf("\t%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
		
		struct  arquivo * temp = a1;
		a1 = a3;
		a3 = a2;
		a2 = temp;
	}
	//final uneRegistros
	contaFases++;
	
	uneRegistros(a2, a1, meuArquivo);
	acessaPonteiro(meuArquivo);
	limpaArquivo(a1);
	limpaArquivo(a2);
	limpaArquivo(a3);
	return;
}

int main(){
	setlocale(LC_ALL, "Portuguese");
	struct arquivo meuArquivo;	int i, resposta;
	
	meuArquivo.nome = "INTER_POLIFASICA";
	meuArquivo.posicao = 0;
	meuArquivo.numRegistros = 0;
	meuArquivo.posBuffer = -1;

	printf("\t\tESCOLHA:\n\n\t1: Digitar os registros \n\t2: Gerar registros aleatoriamente\n\t");
	scanf("%d", &resposta);
	printf("\n");
	
	if(resposta == 1 || resposta == 2){
		printf("\tDigite o número de Registros que o arquivo vai conter:\n\tREGISTROS => ");
		scanf("%d", &nRegistros);
		printf("\n");
	}
	
	switch(resposta){
	case 1: {															//Dane z klawiatury
		struct diaMesAno * data;
		FILE * fileStream;
		data = (struct diaMesAno *)malloc(nRegistros * sizeof(struct diaMesAno));
		
		printf("\tOPÇÃO 1\n\tInsira as datas no formato: DD MM AAAA;\n");
		for (i = 0; i < nRegistros; i++) {
			int d, m, a;
			scanf("%u%u%u", &d, &m, &a);
			if(validaData(d, m, a) == 0){
				printf("\tData fornecida inválida!\n");
				i--;
			}
			else {
				data[i].dia = d;
				data[i].mes = m;
				data[i].ano = a;
			}
		}
		fileStream = fopen(meuArquivo.nome, "wb");
		if (fileStream) {
			fgetpos(fileStream, &(meuArquivo.fpost));
			meuArquivo.numRegistros = nRegistros;
			meuArquivo.posicao = 0;

			fwrite(data, sizeof(struct diaMesAno), nRegistros, fileStream);
			free(data);
			fclose(fileStream);
		}
		else {
			printf("\tO arquivo não pode ser criado!\n");
			
			free(data);
			return 0;
		}
		break;
	}
	case 2: {		//Ggera data randomicamente
		FILE * fileStream;
		srand(time(NULL));
		struct diaMesAno * data; int i;
		data = (struct diaMesAno *)malloc(nRegistros * sizeof(struct diaMesAno));
		
		for(i = 0; i < nRegistros; i++){
			data[i] = getRandDate();
		}
		fileStream = fopen(meuArquivo.nome, "wb");
		if(fileStream){
			fgetpos(fileStream, &(meuArquivo.fpost));
			meuArquivo.numRegistros = nRegistros;
			meuArquivo.posicao = 0;

			fwrite(data, sizeof(struct diaMesAno), nRegistros, fileStream);
			free(data);
			fclose(fileStream);
		}
		else {
			printf("\tO arquivo não pode ser criado!\n");
			
			free(data);
			return 0;
		}
		break;
	}
	default:
		return 0;
	}
	imprimeArquivo(&meuArquivo);
	interPolifasica(&meuArquivo);
	imprimeArquivo(&meuArquivo);
	
	printf("\n\t...................................\n");
	printf("\t\tDADOS:\n");
	printf("\t\tNúmero de leituras: %d\n", contaLeituras);
	printf("\t\tNúmero de entradas: %d\n", contaEscritas);
	printf("\t\tNúmero de fases: %d\n", contaFases);
	printf("\t...................................");

	getch();
	return 0;
}

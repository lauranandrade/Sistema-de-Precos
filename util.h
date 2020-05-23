#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int char2num(char letra)
{
	if (letra < 58)
		return letra-48;
	else if (letra < 91)
		return letra-65;
	else
		return letra-97;
	return num;
}

int str2num(char *str, int t)
{
	int res = 0;
	for (int i=0; i< t; ++i) {
		res += char2num(str[i]) * pow(10,t-i-1);
	}
	return res;
}

char leLetra(void)
{
	char linha[3];
	int c;
	int i=0;
	while((c=getchar()) != '\n' && c != EOF && i<3) {
		linha[i] = c;
		++i;
	}
	return linha[0];
}

int leInt(void)
{
	char linha[10];
	int c;
	int i=0;
	while((c=getchar()) != '\n' && c != EOF) {
		linha[i++] = c;
	}
	return str2num(linha,i);
}

void add_dados(char *mensagem, char *arquivo)
{
	//escreve no arquivo a informação passada pelo cliente
	FILE *arq = fopen(arquivo,"a");
	fprintf(arq,"%s\n",mensagem);
	fclose(arq);
}

int parse_string(char *linha, int tipo, int raio, int lat, int lon)
{
	int longitude;
	int latitude;
	int resp=-1;
	int preco;
	int tp;
	char *token;
	int i = 0;

	while((token = strsep(&linha," "))) {
		switch(i) {
			case 2: tp=str2num(token,strlen(token));
					break;
			case 3: preco=str2num(token,strlen(token));
					break;
			case 4: latitude=str2num(token,strlen(token));
					break;
			case 5: token[strlen(token)-1] = '\0';
					longitude = str2num(token,strlen(token));
					break;
			default: ;
		}
		++i;
	}
	printf("tp %d | preco %d | latitude %d | longitude %d\n",tp,preco,latitude,longitude);
	if (tp == tipo) {
		double dist = sqrt( (lat-latitude)*(lat-latitude) + (lon-longitude)*(lon-longitude) );
		if (dist <= raio) {
			resp = preco;
		}
	}
	return resp;
}

int pesquisar_dados(char *mensagem, char *arquivo)
{
	FILE *arq = fopen(arquivo,"r");
	char *linha;
	char *token;
	int menor = -1;
	int tipo;
	int raio;
	int lat;
	int lon;
	int aux;
	int i = 0;

	while((token = strsep(&mensagem," "))) {
		switch(i) {
			case 2: tipo=str2num(token,strlen(token));
					break;
			case 3: raio=str2num(token,strlen(token));
					break;
			case 4: lat=str2num(token,strlen(token));
					break;
			case 5: lon=str2num(token,strlen(token));
					break;
			default: ;
		}
		++i;
	}

	while(fgets(linha,256,arq)) {
		//verifica se as informações dessa
		//linha valem para a consulta, se valerem e o preço
		//for menor que o menor, salva o novo menor
		aux = parse_string(linha, tipo, raio, lat, lon);
		if (aux != -1 && menor == -1 || aux < menor)
			menor = aux;
	}
	return menor;
}
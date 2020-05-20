int main(int argc, char const *argv[])
{
	printf("Digite \"D\" para Dados e \"P\" para Pesquisa: ");
	//le do cliente
	if (== "D" || == 'd'){
		printf("\nDigite o tipo do combustível (0 - Díesel, 1 - Álcool e 2 - Gasolina): ");
		//le do cliente
		printf("\nDigite o preço sem vírgulas (Ex: r$ 3,299 vira 3299): ");
		//le do cliente
		printf("\nDigite as coordenadas do posto de gasolina (latitude longitude): ");
		//le do cliente
		//junta tudo em uma mensagem só
		//confirmar recepção da mensagem
		printf("\nMensagem recebida: ");
		//imprime a mensagem
		//add a informação em um arquivo

	} else if (== "P" || == 'p'){
		printf("\nDigite o tipo do combustível (0 - Díesel, 1 - Álcool e 2 - Gasolina): ");
		//le do cliente
		printf("\nDigite o raio de busca: ");
		//le do cliente
		printf("\nDigite as coordenadas do centro de busca (latitude longitude): ");
		//le do cliente
		//junta tudo em uma mensagem só
		//confirmar recepção da mensagem
		printf("\nMensagem recebida: ");
		//buscar o menor preço no arquivo
		printf("\nO menor preço para este combustível para postos de combustível que estejam no centro de busca mais raio de busca é: ");
	} else {
		//ERRO
	}
	return 0;
}




#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#include "util.h"

#define MAXLINE 1024 

int sockfd; 
int len, n; 
int PORT = 8000;
char buffer[MAXLINE]; 
struct sockaddr_in servaddr, cliaddr; 

void init_socket()
{
	// Criando o socket
	puts("Criando Socket");
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	memset(&servaddr, 0, sizeof(servaddr)); 
	memset(&cliaddr, 0, sizeof(cliaddr)); 
	
	// Informacoes do servidor
	servaddr.sin_family = AF_INET; // IPv4 
	servaddr.sin_addr.s_addr = INADDR_ANY; 
	servaddr.sin_port = htons(PORT); 
	
	// Coloca o socket no endereco
	puts("Vinculando socket no endereco");
	if ( bind(sockfd, (const struct sockaddr *)&servaddr, 
			sizeof(servaddr)) < 0 ) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	len = sizeof(cliaddr); 
	puts("Conectado, aguardando");
}

int main(int argc, char *argv[]) 
{ 
	double preco;
	char buffer[1024] = {0}; 
	char resposta[256];
	char *mensagem = "Recebido"; 
	char *arquivo = "precos.txt";
	char log[256];
	int c;


	while((c = getopt(argc,argv,"p:")) != -1) {
		switch(c) {
			case 'p':
				PORT = str2int(optarg,strlen(optarg));
				break;
			case '?':
				if(optopt == 'p') {
					puts("Porta não específicada\n Uso: servidor -p <porta>\n");
					return 1;
				}
				break;
			default:
					puts("Parametros não específicados\n Usando porta padrão.\nUso: servidor -p <porta>\n");
		}
	}

	init_socket();

	//recebe a mensagem so cliente
	n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
				MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
				&len); 
	buffer[n] = '\0'; 
	snprintf(log,256,"Cliente : %s",buffer);
	puts(log); 

	//envia a confirmacao
	sendto(sockfd, (const char *)mensagem, strlen(mensagem), 
		MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
			len); 
	puts("Confirmada a recepção.\nMensagem: "); 
	puts(buffer);

	while(1) {

		if (buffer[0] == 'D' || buffer[0] == 'd') {
			adicionar_dados(buffer, arquivo);
			//cria uma string a ser enviada pelo socket
			snprintf(resposta,256,"Dados adicionados");
		}
		else {
			preco = pesquisar_dados(buffer, arquivo);
			//cria uma string a ser enviada pelo socket
			if (preco == -1)
				snprintf(resposta,256,"Nenhum preço encontrado para esse combustível nesse raio.\n");
			else
				snprintf(resposta,256,"Menor preço encontrado: R$%.2f\n",preco/1000);
		}

		//envia a resposta
		sendto(sockfd, (const char *)resposta, strlen(resposta), 
				MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
				len); 
		puts("Resposta enviada, aguardando requisição.");
	
		//recebe a mensagem so cliente
		n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
				MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
				&len); 
		buffer[n] = '\0'; 
		snprintf(log,256,"Cliente : %s",buffer);
		puts(log); 

		//envia a confirmacao
		sendto(sockfd, (const char *)mensagem, strlen(mensagem), 
				MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
				len); 
		puts("Confirmada a recepção.\n"); 

	}
	return 0; 
} 
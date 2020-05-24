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

char *ip = "127.0.0.1";
int PORT = 8000;
int sockfd; 
char buffer[MAXLINE]; 
char *mensagem = "Hello from client"; 
struct sockaddr_in	 servaddr; 

void init_socket()
{ 
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("Socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 
	memset(&servaddr, 0, sizeof(servaddr)); 
	
	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(PORT); 
	servaddr.sin_addr.s_addr = INADDR_ANY; 
}

int main(int argc, char *argv[]) 
{ 
	char opcao;
	char tipo;
	int preco_ou_raio;
	int latitude, longitude;
	int id = 0;
	int c;
	int n, len; 
	char mensagem[256];

	while((c = getopt(argc,argv,"i:p:")) != -1) {
		switch(c) {
			case 'i': 
				ip = optarg;
				break;
			case 'p':
				break;
			case '?':
				if (optopt == 'i') {
					perror("IP não especificado\n uso: cliente -i <ip> -p <porta> \n");
					return 1;
				}
				else if(optopt == 'p') {
					perror("Porta não especificada\n uso: cliente -i <ip> -p <porta> \n");
					return 1;
				}
			default :
				printf("Parametros não especificados, usando padrão\nuso: cliente -i <ip> -p <porta>\n");
		}
	}
	init_socket();
	
	do {
		printf("\nDigite 'D' para inserir dados, 'P' para pesquisar dados ou '0' para encerrar: ");
		opcao = getchar();
		limparBuffer();
		if (opcao != 'D' && opcao != 'P' && opcao != 'd' && opcao != 'p' && opcao !='0'){
			printf("\nEntrada inválida!");
		}
	} while (opcao != 'D' && opcao != 'P' && opcao != 'd' && opcao != 'p' && opcao !='0');
	if (opcao == '0') {
		close(sockfd); 
		exit(0);
	}

	do {
		printf("\nTipo de combustível (0-diesel, 1-alcool, 2-gasolina): ");
		tipo = getchar();
		limparBuffer();
		if (tipo != '0' && tipo != '1' && tipo != '2'){
			printf("\nEntrada inválida!");
		}
	} while(tipo != '0' && tipo != '1' && tipo != '2');
	if(opcao == 'P' || opcao == 'p'){
		printf("\nDigite o raio da busca: ");
	}
	else{
		printf("\nDigite o preço do combustível (Ex: R$4,529 -> 4529): ");
	}
	preco_ou_raio = leInt();

	printf("\nDigite a latitude(0-90): ");
	latitude = leInt();

	printf("\nDigite a longitude(0-180): ");
	longitude = leInt();

	//cria uma string a ser enviada pelo socket
	snprintf(mensagem, 256, "%c %d %d %d %d %d", opcao, id, char2num(tipo), preco_ou_raio, latitude, longitude);
	//cliente envia mensagem
	sendto(sockfd, (const char *)mensagem, strlen(mensagem), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 
	printf("\nEnviado: %s\n", mensagem); 
	id++;
	//recebe a confirmacao
	n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len); 
	buffer[n] = '\0'; 
	//se nao recebeu a confirmacao, envia de novo
	if(n < 2) {
		printf("\nResposta não recebida, reenviando"); 
		//cliente envia mensagem
		sendto(sockfd, (const char *)mensagem, strlen(mensagem), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 
		printf("\nEnviado: %s\n", mensagem); 
		id++;
	}

	while(1) {
		//recebe a resposta do sistema
		n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len); 
		buffer[n] = '\0'; 
		//printa a resposta do sistema
		printf("\nServidor : %s\n",buffer);
		
		//cliente digita informacoes
		do {
			printf("\nDigite 'D' para inserir dados, 'P' para pesquisar dados ou '0' para encerrar: ");
			opcao = getchar();
			limparBuffer();
			if (opcao != 'D' && opcao != 'P' && opcao != 'd' && opcao != 'p' && opcao !='0'){
				printf("\nEntrada inválida!");
			}
		} while (opcao != 'D' && opcao != 'P' && opcao != 'd' && opcao != 'p' && opcao !='0');
		if (opcao == '0') {
			close(sockfd); 
			exit(0);
		}

		do {
			printf("\nTipo de combustível (0-diesel, 1-alcool, 2-gasolina): ");
			tipo = getchar();
			limparBuffer();
			if (tipo != '0' && tipo != '1' && tipo != '2'){
				printf("\nEntrada inválida!");
			}
		} while(tipo != '0' && tipo != '1' && tipo != '2');
		if(opcao == 'P' || opcao == 'p'){
			printf("\nDigite o raio da busca: ");
		}
		else{
			printf("\nDigite o preço do combustível (Ex: R$4,529 -> 4529): ");
		}
		preco_ou_raio = leInt();

		printf("\nDigite a latitude(0-90): ");
		latitude = leInt();

		printf("\nDigite a longitude(0-180): ");
		longitude = leInt();

		//cria uma string a ser enviada pelo socket
		snprintf(mensagem, 256, "%c %d %d %d %d %d", opcao, id, char2num(tipo), preco_ou_raio, latitude, longitude);

		//cliente envia mensagem
		sendto(sockfd, (const char *)mensagem, strlen(mensagem), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 
		printf("\nEnviado: %s\n", mensagem); 
		id++;
		//recebe a confirmacao
		n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len); 
		buffer[n] = '\0'; 
		//se nao recebeu a confirmacao, envia de novo
		if(n < 2) {
			printf("\nResposta não recebida, reenviando"); 
			//cliente envia mensagem
			sendto(sockfd, (const char *)mensagem, strlen(mensagem), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 
			printf("\nEnviado: %s\n", mensagem); 
			id++;
		}
	}
	close(sockfd); 
	return 0; 
}
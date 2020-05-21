#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#include "utils.h"

#define MAXLINE 1024 

char *ip = "127.0.0.1";
int PORT=8000;

int sockfd; 
char buffer[MAXLINE]; 
char *mensagem = "Hello from client"; 
struct sockaddr_in	 servaddr; 


void init_socket()
{ 
	// Criando o descritor do socket
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 

	memset(&servaddr, 0, sizeof(servaddr)); 
	
	// informacao do servidor
	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(PORT); 
	servaddr.sin_addr.s_addr = INADDR_ANY; 
}

int main(int argc, char *argv[]) 
{ 

	int DouP;
	int tipo;
	int preco_ou_raio;
	int lat, lon;
	int id = 0;
	int c;
	char mensagem[256];
	while((c = getopt(argc,argv,"i:p:")) != -1) {
		switch(c) {
			case 'i': 
				ip = optarg;
				break;
			case 'p':
				//PORT = str2int(optarg);
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

	int n, len; 
	
	//cliente digita informacoes
	printf("Digite as informações:\nDados ou Pesquisa (D/P) (0 para encerrar): ");
	DouP = leLetra();
	while (DouP != 68 && DouP != 80 && DouP !=100 && DouP != 112 && DouP !='0') {
		printf("Invalido. Dados ou Pesquisa (D/P) (0 para encerrar): ");
		DouP = leLetra();
	}
	if (DouP == '0') {
		close(sockfd); 
		exit(0);
	}
	printf("Tipo de combustível (0-diesel, 1-alcool, 2-gasolina): ");
	tipo = leInt();
	while(tipo != 0 && tipo != 1 && tipo != 2) {
		printf("Invalido. Tipo de combustível (0-diesel, 1-alcool, 2-gasolina): ");
		tipo = leInt();
	}
	if(DouP == 'P' || DouP == 'p')
		printf("Raio de busca: ");
	else
		printf("Preço do combustível (x1000): ");
	preco_ou_raio = leInt();
	printf("Coordenadas (latitude):");
	lat = leInt();
	printf("Coordenadas (longitude):");
	lon = leInt();

	//cria uma string a ser enviada pelo socket
	snprintf(mensagem,256,"%c %d %d %d %d %d",DouP,id,tipo,preco_ou_raio,lat,lon);

	//cliente envia mensagem
	sendto(sockfd, (const char *)mensagem, strlen(mensagem), 
		MSG_CONFIRM, (const struct sockaddr *) &servaddr, 
			sizeof(servaddr)); 
	printf("Enviado: %s\n", mensagem); 
	id++;
	
	//recebe a confirmacao
	n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
				MSG_WAITALL, (struct sockaddr *) &servaddr, 
				&len); 
	buffer[n] = '\0'; 

	//se nao recebeu a confirmacao, envia de novo
	if(n < 2) {
		printf("Resposta não recebida, reenviando"); 
		//cliente envia mensagem
		sendto(sockfd, (const char *)mensagem, strlen(mensagem), 
				MSG_CONFIRM, (const struct sockaddr *) &servaddr, 
				sizeof(servaddr)); 
		printf("Enviado: %s\n", mensagem); 
		id++;
	}

	while(1) {

		//recebe a resposta do sistema
		n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
					MSG_WAITALL, (struct sockaddr *) &servaddr, 
					&len); 
		buffer[n] = '\0'; 
		//printa a resposta do sistema
		printf("Servidor : %s\n",buffer);

		//cliente digita informacoes
		printf("Digite as informações:\nDados ou Pesquisa (D/P) (0 para encerrar):");
		DouP = leLetra();
		while (DouP != 68 && DouP != 80 && DouP !=100 && DouP != 112 && DouP != '0') {
			printf("Invalido. Dados ou Pesquisa (D/P) (0 para encerrar):");
			DouP = leLetra();
		}
		if (DouP == '0') {
			close(sockfd); 
			exit(0);
		}
		printf("Tipo de combustível (0-diesel, 1-alcool, 2-gasolina):");
		tipo = leInt();
		while(tipo != 0 && tipo != 1 && tipo != 2) {
			printf("Invalido. Tipo de combustível (0-diesel, 1-alcool, 2-gasolina):");
			tipo = leInt();
		}
		if(DouP == 'P' || DouP == 'p')
			printf("Raio de busca: ");
		else
			printf("Preço do combustível (x1000): ");
		preco_ou_raio = leInt();
		printf("Coordenadas (latitude):");
		lat = leInt();
		printf("Coordenadas (longitude):");
		lon = leInt();

		//cria uma string a ser enviada pelo socket
		snprintf(mensagem,256,"%c %d %d %d %d %d",DouP,id,tipo,preco_ou_raio,lat,lon);

		//cliente envia mensagem
		id++;

		//se nao recebeu a mensagem, envia de novo

	}
	close(sockfd); 
	return 0; 
} 


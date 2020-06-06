#include "UDP.h"
#include "geral.h"

/*******************************************************************************************************
* Função: RS_clientUDP()
*
*  Implementação do envio e recebimento de informações de parte do Servidor de Raizes.
*
* Retorna: strings com info das mensagens recebidas 
********************************************************************************************************/

int RS_clientUDP_send(char streamID[], char rsaddr[], char rsport[], int request, char ipaddr[], char uport[]){

  memset(&hints,0,sizeof hints); //inicializa hints
  hints.ai_family=AF_INET;
  hints.ai_socktype=SOCK_DGRAM;
  hints.ai_flags= AI_NUMERICSERV|AI_NUMERICHOST;// se o IP for em numero
  
  char buffer[1024] = {'\0'};
  char * message =NULL;
  char msg[1024] = {'\0'};

  int i = 0,x = 0,k = 0;
  
  n=getaddrinfo(rsaddr,rsport,&hints,&res);
  if(n!=0)
  {
  	fprintf(stderr,"error: getaddrinfo: %s\n", gai_strerror(n));
        /*error*/ exit(1);
  }

  fd=socket(res->ai_family,res->ai_socktype,res->ai_protocol);
  check_error(fd,-1, "socket in RS_clientUDP");


  if(request==1){ //answer to who is root
  	strcpy(buffer, (whoisroot(streamID,ipaddr,uport)));

  }
  else if(request==2){ //show all streams
  	strcpy(buffer,"DUMP\n\0");

  }
   else if(request==3){
  	strcpy(buffer,"REMOVE ");
  	strcat(buffer,streamID);
  	strcat(buffer,"\n");
  }

  n=sendto(fd,buffer,strlen(buffer),0,res->ai_addr,res->ai_addrlen);
  check_error(n,-1, "sendto in RS_clientUDP");

  if (flag_debug == 1)
  {
   	printf("Sent:\n%s\n", buffer);
  }

  freeaddrinfo(res);
  return (fd);
}


char * RS_clientUDP_receive(int fd, int request){


	struct sockaddr_in addr_RS;
	char buffer[MAX_STR] = {'\0'};
	static char message[MAX_STR] = {'\0'};
	char msg[MAX_STR] = {'\0'};

	int i = 0,x = 0,k = 0;

	addrlen=sizeof(addr_RS);
	n=recvfrom(fd,buffer,(MAX_STR-1),0,(struct sockaddr*)&addr_RS,&addrlen);
	check_error(n,-1, "recvfrom in RS_clientUDP_receive");
	buffer[n] = '\0';

	if (flag_debug == 1)
	{
		printf("Received:\n%s\n", buffer);
	}

 	if (request==1){   // formula a resposta à mensagem WhoIsRoot
 		for(i=0; i<strlen(buffer) && buffer[i] != ' ' ; i++ );

 			for (k=i; buffer[k] != '\n'; k++ );

 				for (; k<strlen(buffer); k++)
 					buffer[k]='\0';

 				x=0;

 				for(i=i+1; i<strlen(buffer) && buffer[i]!='\n'; i++ ){
 					msg[x]=buffer[i];
 					x++;
 				}
 				for(i; i<strlen(buffer) ; i++ ){
 					msg[x]='\0';
 					x++;
 				} 
 				strcpy(message,msg);
 			}
	else if(request == 2) // Manda as Streams da APP
	{
		if (flag_debug == 0)
		{
			printf("%s\n",buffer );
		}
	}

	close(fd);
	return (message);
}


/***************************************************************************************
* Função: whoisroot()
*
*   Formula o buffer que vai ser enviado ao servidor de raizes para perguntar quem é
*   a Raiz.
*
****************************************************************************************/
char * whoisroot(char streamID[], char rsaddr[], char rsport[]){
	static char buffer[1024]={'\0'};

	strcpy(buffer,"WHOISROOT ");
	strcat(buffer,streamID);
	strcat(buffer," ");
	strcat(buffer,rsaddr);
	strcat(buffer,":");
	strcat(buffer,rsport);
	strcat(buffer,"\n");

	return buffer;
}

/****************************************************************************************
*Função: getfd_servUDP_access()
* 
*    Determina o valor do fd associado ao Servidor de Acesso, este fd é usado no run_app()
*   em que o servidor recebe o comando "POPREQ\n" e vai buscar ao vetor bestpops[] o 
*   primeiro iamroot com ligação TCP disponível, em seguida responde com "POPRESQ ..." 
*   com o porto e ip do ponto de acesso ao qual este iamroot se pode conectar.
*
* retorna: fd do servidor de Acesso
****************************************************************************************/
int getfd_servUDP_access(char uport[]){

	struct addrinfo hints_acesso, *res_acesso;

	memset(&hints_acesso,0, sizeof hints_acesso);
	hints_acesso.ai_family=AF_INET;
	hints_acesso.ai_socktype=SOCK_DGRAM;
	hints_acesso.ai_flags=AI_PASSIVE|AI_NUMERICSERV;


	n=getaddrinfo(NULL,uport,&hints_acesso,&res_acesso);
	if(n!=0){
		fprintf(stderr,"error: ?? getaddrinfo: %s\n", gai_strerror(n));
		/*error*/ exit(1);
	}

	fd=socket(res_acesso->ai_family,res_acesso->ai_socktype,res_acesso->ai_protocol);
	check_error(fd,-1, "socket in getfd_servUDP_access");

	n=bind(fd,res_acesso->ai_addr,res_acesso->ai_addrlen);
	check_error(n,-1, "bind in getfd_servUDP_access");

	freeaddrinfo(res_acesso);
	return fd;
}

/****************************************************************************************
*Função: PopReq()
*
*    Faz a ligação com o Servidor de Raízes e pede um ponto de acesso a este servidor 
* pelo comando "POPREQ\n", a isto o servidor responde com "POPRESQ ..." com o porto e
* ip do ponto de acesso ao qual este iamroot se pode conectar.
*
* retorna: str com os dados do ponto de acesso
****************************************************************************************/
int PopReq_send(char ipaddr[], char uport[]){

  memset(&hints,0,sizeof hints); //inicializa hints
  hints.ai_family=AF_INET;
  hints.ai_socktype=SOCK_DGRAM;
  hints.ai_flags= AI_NUMERICSERV|AI_NUMERICHOST;// se o IP for em numero
  
  char buffer[1024] = {'\0'};
  static char message[1024]  = {'\0'};
  char msg[1024] = {'\0'};

  n=getaddrinfo(ipaddr,uport,&hints,&res);
  if(n!=0)
  {
  	fprintf(stderr,"error: getaddrinfo: %s\n", gai_strerror(n));
        /*error*/ exit(1);
  }

  fd=socket(res->ai_family,res->ai_socktype,res->ai_protocol);
  check_error(fd,-1, "socket in PopReq");

  strcpy(buffer, "POPREQ\n");

  n=sendto(fd,buffer,strlen(buffer),0,res->ai_addr,res->ai_addrlen);
  check_error(n,-1, "sendto in PopReq");

  if (flag_debug == 1)
	{
		printf("Sent:\n%s\n", buffer);
	}

  freeaddrinfo(res);
  return(fd);
}


char * PopReq_recv(int fd){

  static char buffer[MAX_STR] = {'\0'}; 

  addrlen=sizeof(addr);
  n=recvfrom(fd,buffer,(MAX_STR-1),0,(struct sockaddr*)&addr,&addrlen);
  check_error(n,-1, "recvfrom in PopReq_recv");
  buffer[n] = '\0';

  if (flag_debug == 1)
	{
		printf("Received:\n%s\n", buffer);
	}
 
  close(fd);

  return (buffer);
}


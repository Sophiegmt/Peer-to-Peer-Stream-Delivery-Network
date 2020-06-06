#include "TCP.h"
#include "geral.h"

/*****************************************************************************************
*Função:getfd_TCP_fonte()
*
*	Determina o valor do FD correspondente ao servidor da Fonte que vai enviar mensagens 
* para baixo.
*
*****************************************************************************************/
int getfd_TCP_fonte(char ip[], char port[]){
	int fd, n;
	struct addrinfo hints_fonte, *res_fonte;

	memset(&hints_fonte, 0, sizeof hints_fonte);
	hints_fonte.ai_family = AF_INET;
	hints_fonte.ai_socktype = SOCK_STREAM; // stream para TCPs
	hints_fonte.ai_flags = AI_NUMERICSERV|AI_NUMERICHOST;
	char buffer[1024];

	n = getaddrinfo(ip, port, &hints_fonte, &res_fonte);
	if (n != 0)
	{
		fprintf(stderr,"error: getaddrinfo: %s\n", gai_strerror(n));
		/*error*/ exit(1);
	}

	fd = socket(res_fonte->ai_family, res_fonte->ai_socktype, res_fonte->ai_protocol);
	check_error(fd,-1, "socket in getfd_TCP_fonte");

	n=connect(fd, res_fonte->ai_addr, res_fonte->ai_addrlen);
 	check_error(n,-1, "connect in getfd_TCP_fonte");

	freeaddrinfo(res_fonte);
	return fd;
}


/*****************************************************************************************
*função getfd_servTCP()
*
*	Determina o valor do FD correspondente ao servidor TCP jusante 
*
*****************************************************************************************/
int getfd_servTCP(char tport[]){
	int fd, n, val;
	struct addrinfo hints_jusante, *res_jusante;

	memset(&hints_jusante, 0, sizeof hints_jusante);
	hints_jusante.ai_family = AF_INET;
	hints_jusante.ai_socktype = SOCK_STREAM; // stream para TCPs
	hints_jusante.ai_flags = AI_PASSIVE|AI_NUMERICSERV;

	n = getaddrinfo(NULL, tport , &hints_jusante, &res_jusante);
	if (n != 0)
	{
		fprintf(stderr,"\nerror: getaddrinfo in getfd_servTCP: %s\n", gai_strerror(n));
		/*error*/ exit(1);
	}

	fd = socket(res_jusante->ai_family, res_jusante->ai_socktype, res_jusante->ai_protocol);
	check_error(fd,-1, "socket in getfd_servTCP");

	n = bind(fd, res_jusante->ai_addr, res_jusante->ai_addrlen);
	check_error(n,-1, "bind in getfd_servTCP");

	val=listen(fd, 5);
	check_error(val,-1, "listen in getfd_servTCP");

	freeaddrinfo(res_jusante);
	return fd;
}

/*****************************************************************************************
*função getfd_clientTCP()
*
*	Determina o valor do FD correspondente ao cliente TCP jusante 
*
*****************************************************************************************/
int getfd_clientTCP(char tudo[], int estado){
	
	int fd, n;
	struct addrinfo hints, *res;
	char buffer[64], tralha1[64], tralha2[64],ip[64], tport[64];
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM; // stream para TCPs
	hints.ai_flags = AI_NUMERICSERV|AI_NUMERICHOST;

	
	if(estado==1){
		sscanf(tudo, "%s %s %s", buffer, tralha1, tralha2); // colocam apenas o IP e porto TCP do ponto de
		sscanf(tralha2, "%[^:]:%s", ip, tport);			 // acesso nas variaveis correspondentes
	}else if(estado==2){
		sscanf(tudo, "%s %s", buffer, tralha1); // colocam apenas o IP e porto TCP do ponto de
		sscanf(tralha1, "%[^:]:%s", ip, tport);			 // acesso nas variaveis correspondentes
	}

	n = getaddrinfo(ip, tport, &hints, &res);
	if (n!= 0)
	{
		fprintf(stderr,"\nerror: getaddrinfo in getfd_clientTCP: %s\n", gai_strerror(n));
		/*error*/ exit(1);
	}

	fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	if (fd != -1)
	{
		n=connect(fd, res->ai_addr, res->ai_addrlen);
		check_error(n,-1, "connect in getfd_clientTCP");
	}
	
	return fd;
}


/****************************************************************************************
*Função: read_from_serv_TCP()
*
*    Recebe e le a informacao mandada pelo seu par a montante, e decide o que fazer
*
****************************************************************************************/
int read_from_serv_TCP(int fd,char buffer[], iamroot * root)
{
	char queryID[5]={'\0'}, ip_port[64]={'\0'};
	int n=0, i=0, x=0, queryID_dec=0;

	char inicio[64]={'\0'}, fim[64]={'\0'};
	if(cmp_to_we(root, buffer) == 1)
	{
		n = write(fd, new_pop(root), strlen(new_pop(root)));
		check_error(n,-1, "write in read_from_serv_TCP");
	}

	else if(cmp_to_PQ(buffer, &queryID, &x) == 1)
	{	
		queryID[4] = '\0';
		sscanf(buffer, "%[^\n] %[^\n]", inicio, fim);
		inicio[strlen(inicio)]='\0';
		fim[strlen(fim)]='\0';
	
		if(root->free_tcpsessions>0){
			queryID_dec=build_reply(queryID, root);			
			n = write(fd, reply[queryID_dec], strlen(reply[queryID_dec]));
		    check_error(n,-1, "write in read_from_serv_TCP");
		    strcpy(reply[queryID_dec]," ");
		    x--;
		}
		if(x>0)
		decrementar_x(&inicio);
	    strcpy(query[queryID_dec],inicio); // o serv vai enviar aos filhos
	    send_down(inicio, root);
		

		if(strcmp(fim, "SF\n")==0)
		{
			send_down(fim, root);
		}
	}
		

	else if(cmp_to_da(buffer) == 1)
	{
		stream_flowing=1;
		for(i=0;i<root->tcpsessions;i++){

			if(root->stream_flowing[i]==0 && root->fd_occupation[i]==1){
				root->stream_flowing[i]=1;
				write(root->fd[i],"SF\n",strlen("SF\n"));
				if (flag_debug == 1)
				{
					printf("Sent:\nSF\n");
				}
			}
		}
		send_down(buffer, root);
		if (flag_data == 1)
		{
			if (flag_hex == 1)
			{
				printf("Received:\nDA\n");
				for (i = 2; i < strlen(buffer)-1; i++)
				{
					fprintf(stdout, "%x",buffer[i]);
				}
			}
			else
				printf("Received:\n%s\n",buffer);
		}
		n_client_temp=1;
	}
	else if(cmp_to_TQ(buffer)==1)
	{
	    build_TR(buffer, root, fd);
	}
	else if (cmp_to_re(buffer)==1)
	{
		fd=getfd_clientTCP(buffer,2);
	}	
	else if(cmp_to_BS(buffer)==1)
	{
		stream_flowing=0;
		send_down(buffer,root);
		for(i=0;i<root->tcpsessions;i++){

			if(root->stream_flowing[i]==1 && root->fd_occupation[i]==1){
				root->stream_flowing[i]=0;
			}
		}
	}
	else if(cmp_to_SF(buffer)==1)
	{
		stream_flowing=1;
		send_down(buffer,root);
	}
	if (flag_debug == 1)
	{
		if(cmp_to_da(buffer) == 0)
			printf("Received:\n%s\n", buffer);
	}

	return fd;
}


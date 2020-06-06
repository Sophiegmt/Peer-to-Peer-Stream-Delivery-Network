#include "functions.h"


void main(int argc, const char* argv[]){

	int i=0, y=0, j=0, x=0;
	
	int isroot=0; // indica se stream é raiz ou nao
	hex=0;
	stay=1;
	stream_flowing=0;

	char add[64] = {'\0'}, port[64] = {'\0'}, stream_name[64] = {'\0'}, stream_port[64] = {'\0'}, stream_ip[64] = {'\0'}, temp[64]= {'\0'}, message[1024] = {'\0'};

	iamroot newroot;
	init(&newroot);

	for( i=1 ; i < argc; i++)
	{
		if(strcmp(argv[i],"-i")==0){ //IP  da interface
			i++;
			strcpy(newroot.ipaddr, argv[i]);
			CheckIP_Format("da interface", newroot.ipaddr);
		}
		else if(strcmp(argv[i],"-t")==0){ //porto TCP onde a aplicaçao aceita ligaçoes
			i++;
			strcpy(newroot.tport, argv[i]);
			CheckPortFormat("TCP de ligação a jusante", newroot.tport);
		}
		else if(strcmp(argv[i],"-u")==0){ //porto UDP do servidor de acesso
			i++;
			strcpy(newroot.uport, argv[i]);
			CheckPortFormat("UDP do servidor de acesso", newroot.uport);
		}
		else if(strcmp(argv[i],"-s")==0){ // endereço e porto do servidor de raizes
			i++;
			strcpy(temp, argv[i]);
			y = 0;
			while (temp[y] != ':') //grava ip de sevidor raizes
			{
				add[y] = temp[y];
				y++;
			}
			CheckIP_Format("do servidor de raízes", add);
			y++; x = 0;
			while (temp[y] != '\0') { //grava porto de servidor de raizes
				port[x]=temp[y];
				y++; x++;
			}
			CheckPortFormat("do servidor de raízes", port);
			strcpy(newroot.rsaddr,add);
			strcpy(newroot.rsport,port);

		}
		else if(strcmp(argv[i],"-p")==0){ //numero de ligaçoes que este iamroot aceita
			i++;
			newroot.tcpsessions=atoi(argv[i]);
			newroot.free_tcpsessions=atoi(argv[i]);
			if (newroot.tcpsessions < 1)
			{
				printf("Número de sessões TCP inválido. \n");
				exit(1);

			}
			// Caso tcpsessions seja indicado pelo utilizador, temos de realocar o espaco de memoria com tamanho necessário 
		}
		else if(strcmp(argv[i],"-n")==0){ //numero de pontos de acesso a descobrir quando e pedido
			i++;
			newroot.n_bestpops=atoi(argv[i]);

			if (newroot.n_bestpops < 1)
			{
				printf("Número pontos de acesso a recolher inválido. \n");
				exit(1);
			}
		}
		else if(strcmp(argv[i],"-x")==0){ //intervalo de tempo do registo periodico como raiz
			i++;
			newroot.tsecs=atoi(argv[i]);
			if (newroot.tsecs < 1)
			{
				printf("Período do registo periódico inválido. \n");
				exit(1);
			}
		}
		else if(strcmp(argv[i],"-b") == 0){ // flags
			flag_data = 0;
		}
		else if (strcmp(argv[i], "-d") == 0) { // flags
			flag_debug = 1;
		}
		else if (strcmp(argv[i], "-h") == 0){ // flags
			flag_sinopse = 1;
		}
		else{		// STREAM ID								
			strcpy(newroot.streamID, argv[i]);
			y = 0; 
			while (newroot.streamID[y] != ':') {
				stream_name[y] = newroot.streamID[y];
				y++;
			} y++; x = 0;
			while (newroot.streamID[y] != ':') {
				stream_ip[x]=newroot.streamID[y];
				y++; x++;
			}
			stream_ip[x] = '\0';
			CheckIP_Format("do stream", stream_ip);
			x = 0; y++;
			while (newroot.streamID[y] != '\0') {
				stream_port[x]=newroot.streamID[y];
				y++; x++;
			}
			CheckPortFormat("do stream",stream_port);

			strcpy(newroot.fsname,stream_name);
			strcpy(newroot.fsaddr,stream_ip);
			strcpy(newroot.fsport,stream_port);
			
		}

	}		
	init_vetores_fd(&newroot);

	int fd_RS, request;
	int maxfd,counter;
	fd_set rfds;

	if(strcmp(newroot.streamID," ")==0) {   // se nao foi inserida nenhuma stream
		printf("error: insert streamID\n");
 	 // APRESENTAR LISTA DE STREAM
		request = 2;
		fd_RS = RS_clientUDP_send(newroot.streamID,newroot.rsaddr, newroot.rsport,request, newroot.ipaddr, newroot.uport);	
	}
 	else{  // se stream foi inserida
 		request = 1;
 		fd_RS= RS_clientUDP_send(newroot.streamID,newroot.rsaddr, newroot.rsport,request, newroot.ipaddr, newroot.uport); // WHOISROOT
 	}
 	
	struct timeval time;
 	time.tv_sec = 5;
 	time.tv_usec = 0;

 	FD_ZERO(&rfds);
	FD_SET(fd_RS,&rfds); maxfd=fd_RS;//estabelece FD para alguem se conectar a si em TCP  

	// SELECT : SELECIONA AS INFOS NOS FDs RECEBIDOS POR ORDEM DE CHEGADA
	counter=select(maxfd+1,&rfds, (fd_set*)NULL, (fd_set*)NULL,  &time); //printf("COUNTER: %d\n", counter);
	if(counter<0){
		fprintf(stderr,"ERROR: %s\n", strerror(errno));
		/*error*/exit(1);
	}	

	if (time.tv_sec == 0)
	{
		printf("Could not speak to root server. Exiting...\n");
		exit(1);
	}	

	if(FD_ISSET(fd_RS,&rfds)) //FD IS READY
	{
		if (request == 2)
		{
			strcpy(message, RS_clientUDP_receive(fd_RS,2)); // DUMP
			write(1,message,strlen(message));
	// libertar memória
			free(newroot.fd);
			free(newroot.fd_occupation);
			for(j=0; j< newroot.tcpsessions; j++)
			{
				free(newroot.TCPconnect[j]);
			}
			free(newroot.TCPconnect);

	// sair da applicação
			exit(1);
		}
		else if(request == 1)
		{
			strcpy(stream_raiz, RS_clientUDP_receive(fd_RS, 1)); // WHOISROOT
	 // 		
			isroot=strcmp(stream_raiz,newroot.streamID); // is root devolve 0 se for raiz e 1 se nao for raiz
			if (isroot != 0)
			{
				isroot = 1;
			}

			if(isroot==0){

				// bestpops é o vetor que vai conter o ip e porto TCP das ligaçoes nao ocupadas da árvore
				bestpops=(char **)malloc(newroot.n_bestpops * sizeof (char*));
				num_bestpops = (int*)malloc(newroot.n_bestpops * sizeof(int));

				for(i=0; i< newroot.n_bestpops ; i++) 
				{	
					bestpops[i]=(char *)malloc(1024* sizeof (char));
					strcpy(bestpops[i]," ");
					num_bestpops[i] = 0;
				}

				// criar primeiro enderenço do local da ligacao TCP jusante disponível : a raiz
				strcpy(bestpops[0],construir_POPRESP(&newroot, 1, add));  
				num_bestpops[0] = newroot.free_tcpsessions;
			}
			else
			{	
				keep_root_info(); // função que guarda ip e porto da raiz
			}
		}	
	}
	close(fd_RS);
	run_app(&newroot, isroot);	



	free(newroot.fd);
	free(newroot.fd_occupation);
	free(newroot.stream_flowing);
	for(j=0; j< newroot.tcpsessions; j++)
	{
		free(newroot.TCPconnect[j]);
	}
	free(newroot.TCPconnect); 
}


/***********************************************************************************
* função: run_app()
*
*		Funçoa que faz as funcionalidades do programa, connexões TCP e UDP
*
************************************************************************************/
int run_app(iamroot* root, int urroot){

	int m=0, i=0, tamanho=0, free_bestpops=0;
	struct timeval time;
	int maxfd=0,counter=0, fd_extra=0, last_connect=0;
	fd_set rfds;


	n_client_temp=0;
	int n_tecla=0;
	char buffer_tecla[MAX_STR]={'\0'};


	int r=0, x=0, j=0, z=0;
	int jadeseguida=0;
	int queryID=0, avails = 0;
	char tag[64]={'\0'}, hexvalue[3]={'\0'}, ip_port[64]={'\0'};
//LIGACAO SERVIDOR DE RAIZES
	int fd_rs=-1, request = 0;

// LIGAÇÃO FONTE
	int fd_fonte=-1, addrlen_fonte=0, n_fonte=0;
	struct sockaddr_in addr_fonte;
	char buffer_fonte[MAX_STR]={'\0'};

// LIGAÇÃO SERVIDOR DE ACESSO
	int fd_servUDP_access=-1, fd_clientUDPaccess=-1;
	int n_acesso=0, addrlen_acesso=0;
	struct sockaddr_in addr_acesso; 
	char buffer_acesso[MAX_STR]={'\0'}, buffer_connect[MAX_STR]={'\0'};


// LIGAÇÕES SERV TCP
	int fd_servTCP=-1;
	int afd_jusante[root->tcpsessions];
	int n_jusante=0, addrlen_jusante=0, tamanho_buffer_acesso=0;
	struct sockaddr_in addr_jusante;
	char buffer_jusante[MAX_STR]={'\0'};

	
	for (int i = 0; i < root->tcpsessions; i++)
	{
		afd_jusante[i] = -1; // inicializar
	}

  // LIGAÇÕES CLIENT TCP
	int fd_clientTCP=-1,n_client=0, addrlen_client=0;
	char buffer_client[MAX_STR]={'\0'}; 
	struct sockaddr_in addr_client;

	//inicializar tempo de reposição da raiz no servidor
	time.tv_sec = root->tsecs;
	time.tv_usec = 0;

	for(i=0; i< 65535 ; i++) 
	{	
		strcpy(reply[i], " ");
		strcpy(query[i]," ");
	}

	//fd servidor ligação TCP	
	fd_servTCP = getfd_servTCP(root->tport);

	if (urroot == 0)
	{ 
		// fd servidor de acesso à fonte
		fd_fonte= getfd_TCP_fonte(root->fsaddr,  root->fsport); 	
		// fd servidor de acesso à arvore
		fd_servUDP_access = getfd_servUDP_access(root->uport);

	} else {
		fd_clientUDPaccess = PopReq_send(WhoIsRoot_IP,  WhoIsRoot_U);
	}

	while(stay){

		FD_ZERO(&rfds);
		FD_SET(0, &rfds);
		FD_SET(fd_servTCP,&rfds); maxfd=fd_servTCP;//estabelece FD para alguem se conectar a si em TCP  
		FD_SET(fd_rs, &rfds); maxfd=max(maxfd, fd_rs);

		if(urroot==0)  // so entra no Servidor de Acesso e liga à Fonte se for a Raiz
		{
			FD_SET(fd_fonte, &rfds); maxfd=max(maxfd, fd_fonte); //estabelece FD TCP com a fonte
			FD_SET(fd_servUDP_access, &rfds); maxfd=max(maxfd,fd_servUDP_access); //estabelece FD UDP para alguem se conectar ao servidor de acessoPop_query
		}

		if (urroot != 0)   // so cria ligação cliente TCP se nao for Raiz, a Raiz está ligada à fonte
		{
			FD_SET(fd_clientTCP, &rfds);maxfd=max(maxfd, fd_clientTCP);
			FD_SET(fd_clientUDPaccess, &rfds);maxfd=max(maxfd, fd_clientUDPaccess);
		}

		for(i=0; i< root->tcpsessions; i++){
			if (afd_jusante[i] > 0)
			{
				FD_SET(afd_jusante[i], &rfds);maxfd=max(maxfd, afd_jusante[i]);
			}
		}

	// SELECT : SELECIONA AS INFOS NOS FDs RECEBIDOS POR ORDEM DE CHEGADA
		counter=select(maxfd+1,&rfds, (fd_set*)NULL, (fd_set*)NULL,  &time); //printf("COUNTER: %d\n", counter);
		if(counter<0){
			fprintf(stderr,"ERROR: %s\n", strerror(errno));
			/*error*/exit(1);
		}	

		if (time.tv_sec == 0)
		{
			time.tv_sec = root->tsecs;
			if (urroot==0)
			{//faz refresh no servidor de raizes
				RS_clientUDP_send(root->streamID, root->rsaddr, root->rsport,1, root->ipaddr, root->uport);
				if (flag_debug == 1)
				{
					printf("Refresh in root server.\n");
				}
				request=1;

				x = root->n_bestpops - root_in_bestpops(root);
				
				if(x>0){        // se bestpops não cheio raiz envia msg PQ para baixo					
					Pop_query(x, root); 
				}else if(x==0){  // se bestpops cheio não envia nada e mete os vetores a zero
					for(j=0; j<65535 ; j++){	
						strcpy(reply[j]," ");
						strcpy(query[j]," ");
					}
				}
			}
			if (urroot != 0){
				if (n_client_temp==0){
					if (stream_flowing==0){
						close(fd_clientTCP);
						fd_clientTCP=-1;
						strcpy(root->pai," ");
						request = 1;
						fd_rs = RS_clientUDP_send(root->streamID,root->rsaddr, root->rsport,request, root->ipaddr, root->uport);
					}			
				}			
				if (fd_clientUDPaccess > 0)
				{
					fd_clientUDPaccess = PopReq_send(WhoIsRoot_IP,  WhoIsRoot_U);
				}
			}
		}	

		
		if(FD_ISSET(fd_clientUDPaccess,&rfds))
		{
			strcpy(buffer_acesso, PopReq_recv(fd_clientUDPaccess));
			fd_clientUDPaccess = -1;

			if (cmp_to_me(buffer_acesso, root) == 1)
			{
				request = 1;
				fd_rs = RS_clientUDP_send(root->streamID,root->rsaddr, root->rsport, 1, root->ipaddr, root->uport);
				fd_clientTCP = -1;
				if (fd_clientTCP == -1)
				{
					fd_clientUDPaccess = PopReq_send(WhoIsRoot_IP,  WhoIsRoot_U);
				}
			}			
			else
			{
				fd_clientTCP = getfd_clientTCP(buffer_acesso,1);
				if (fd_clientTCP == -1)
				{
					fd_clientUDPaccess = PopReq_send(WhoIsRoot_IP,  WhoIsRoot_U);
				}
				else
				{
					strcpy(root->pai, rmv_tag(buffer_acesso, 3));
				}
			}
		}

		if(FD_ISSET(fd_rs,&rfds))
		{

			strcpy(stream_raiz, RS_clientUDP_receive(fd_rs, request)); // WHOISROOT
			if (request == 1)
			{
				urroot=strcmp(stream_raiz,root->streamID);
				//printf("stream_raiz: %s\n", stream_raiz);
				if (urroot != 0)
				{
					urroot = 1;
				}
				if(urroot==0){

					fd_fonte= getfd_TCP_fonte(root->fsaddr,  root->fsport); 	
					// fd servidor de acesso à arvore
					fd_servUDP_access = getfd_servUDP_access(root->uport);

					// bestpops é o vetor que vai conter o ip e porto TCP das ligaçoes nao ocupadas da árvore
					bestpops=(char **)malloc(root->n_bestpops * sizeof (char*));
					num_bestpops = (int*)malloc(root->n_bestpops * sizeof(int));
					x=root->n_bestpops;
					for(i=0; i< root->n_bestpops ; i++) 
					{	
						bestpops[i]=(char *)malloc(MAX_STR* sizeof (char));
						strcpy(bestpops[i]," ");
						num_bestpops[i] = 0;
					}
					strcpy(bestpops[0],construir_POPRESP(root, 1, NULL));  
					num_bestpops[0] = root->free_tcpsessions;
					x--;
					if(x>0){
						Pop_query(x, root); 
					}
				}
				else
				{	
					keep_root_info(); // função que guarda ip e porto da raiz
					fd_clientUDPaccess =PopReq_send(WhoIsRoot_IP,  WhoIsRoot_U);
				}
				
			}
			fd_rs = -1;
		}
//se recebeu algo de um cliente TCP a tentar ligar-se
/********************************************************************************************************************/
 		if(FD_ISSET(fd_servTCP,&rfds)) //FD IS READY
 		{
 			m=0;
			while((root->fd_occupation[m] != 0 )&&( m < root->tcpsessions)) // Verifica se ha uma ligação TCP livre na stream
			{
				m++;
			}
			if(m==root->tcpsessions){
				m--;
			}	
			if(root->fd[m]<0)  // se ainda não foi ocupado por um IAMROOT simultaneo
			{ 
				addrlen_jusante = sizeof(addr_jusante);
				root->fd[m]=accept(fd_servTCP,(struct sockaddr*)&addr_jusante,&addrlen_jusante);	

				if(root->fd[m]<1) // se deu erro fazer exit
				{  
					root->fd[m]=-1;
					fprintf(stderr,"ERROR: could not accept TCP  client: %s\n", strerror(errno));
					exit(1);

				}else{ //se nao deu erro				  
					afd_jusante[m]=root->fd[m];
					root->fd_occupation[m]=1;
					root->free_tcpsessions --;
					WELCOME(root, m);   // conseguiu aceder
					last_connect = m;
				}
			}
			else{ 
				addrlen_jusante = sizeof(addr_jusante);
				fd_extra=accept(fd_servTCP,(struct sockaddr*)&addr_jusante,&addrlen_jusante);
				if (strcmp(root->TCPconnect[last_connect]," ")!=0) {
					REDIRECT(fd_extra, root, last_connect);
					fd_extra = -1;
				}
			}			
			
		}
//se recebeu algo de alguma ligação TCP já feita, que está a comunicar agora
/*********************************************************************************************************************/
		for(i=0; i< (root->tcpsessions); i++){
		 	if(FD_ISSET(afd_jusante[i],&rfds)) //recebeu algo de jusante TCP
		 	{
		 		for(int z=0; z<(MAX_STR-1); z++)
					buffer_jusante[z] = '\0';
		 		
				if((n_jusante=read(afd_jusante[i],buffer_jusante,MAX_STR-1)) >= 0) //leu o que recebeu
				{
					check_error(n_jusante, -1, "read from afd_jusante");
					buffer_jusante[n_jusante] = '\0';
					//printf("received: %s", buffer_jusante);
					//printf("FORA: TCP CONNECT[%d]: %s\n", i, root->TCPconnect[i]);
					if (n_jusante == 0) //se nao tiver lido nada, alguem se desconectou.Vai fechar a ligação
					{
						root->fd[i]=-1;
						root->fd_occupation[i] = 0;
						afd_jusante[i] = -1;
						root->free_tcpsessions ++;
						if (urroot == 0)
						{
							clear_bestpops_with_root(root, i);
						}
						
						strcpy(root->TCPconnect[i], " ");
					}
					else if(cmp_to_np(buffer_jusante) == 1){
						if (flag_debug == 1)
						{
							printf("Received:\n%s\n", buffer_jusante);
						}
						strcpy(root->TCPconnect[i],rmv_tag(buffer_jusante, 2));
						//printf("TCP CONNECT[%d]: %s\n", i, root->TCPconnect[i]);
						root->TCPconnect[i][strlen(root->TCPconnect[i])]='\0';
						
						root->stream_flowing[i]=0;
						if(urroot==0){ 
							x=0;
							for(j=0; j<root->n_bestpops; j++){  // identificar se bestpops com lugares vazios
								if(strcmp(bestpops[j], " ")==0){
									x++;								
								}
							}
							if(x>0){        // se bestpops não cheio raiz envia msg PQ para baixo	
								free_bestpops=x;				
								Pop_query(free_bestpops, root); 
							}else if(x==0){  // se bestpops cheio não envia nada e mete os vetores a zero
								for(j=0; j<65535 ; j++){	
									strcpy(reply[j]," ");
									strcpy(query[j]," ");
								}
							}
						}
						if (fd_extra >0)
						{
							REDIRECT(fd_extra, root, i);
							fd_extra = -1;
						}	
					}
					else if(cmp_to_PR(buffer_jusante, &ip_port, &queryID, &avails) == 1){
						strcpy(reply[queryID],buffer_jusante);
						Pop_rep(root, urroot, ip_port, &x, avails, fd_clientTCP, buffer_jusante ); 	
						if (flag_debug == 1)
						{
							printf("Received:\n%s\n", buffer_jusante);
						}
					}
					else if(cmp_to_TR(buffer_jusante)==1){
						if (flag_debug == 1)
						{
							printf("Received:\n%s\n", buffer_jusante);
						}
						 if(print_tq==1){
						 	dar_para_receber--;
						 	printf("%s\n", buffer_jusante);
							 continue_TQ(buffer_jusante, root);
							 if (dar_para_receber == 0)
							 {
							 	print_tq = 0;
							 }
						 }else{
							 send_up(buffer_jusante,fd_clientTCP);
							 if (flag_debug == 1)
							{
								printf("Sent:\n%s\n", buffer_jusante);
							}
						 }
						 
					}
				}
			} 
		}

		if(FD_ISSET(0,&rfds))
		{
				n_tecla=read(0, buffer_tecla, MAX_STR-1);
				check_error(n_tecla, -1, "read from teclado");
				buffer_tecla[n_tecla]='\0';
				interface(buffer_tecla, root, urroot, &fd_rs, &request);
				if(stay == 0)
					break;
		}

//se for raiz, se recebeu algo da fonte
/********************************************************************************************************************/
		if(urroot==0){

			if (FD_ISSET(fd_fonte, &rfds))
			{		   
				for(i=0; i< (MAX_STR-1); i++)
					buffer_fonte[i]='\0';

				//printf("\n \n Fonte está activada \n \n");
				
				n_fonte=read(fd_fonte, buffer_fonte, MAX_STR-1); 
				check_error(n_fonte,-1, "read from font");
				buffer_fonte[n_fonte] = '\0';
				tamanho=strlen(buffer_fonte);
				if (flag_data == 1)
				{
					printf("Received:\n%s\n",buffer_fonte);
				}
				stream_flowing=1;
				DATA(root, buffer_fonte, tamanho);
				
			}
//se for raiz, se alguem mandou mensagem ao servidor de acesso
/********************************************************************************************************************/
			if(FD_ISSET(fd_servUDP_access, &rfds)){

				addrlen_acesso=sizeof(addr_acesso.sin_addr.s_addr);
				for(i=0; i<MAX_STR; i++)
					buffer_acesso[i]='\0';

				n_acesso=recvfrom(fd_servUDP_access, buffer_acesso,(MAX_STR-1),0,(struct sockaddr*)&addr_acesso,&addrlen_acesso);	
				check_error(n_acesso,-1, "recvfrom in Servidor de Acesso");
				buffer_acesso[n_acesso] = '\0';
				if (flag_debug == 1)
				{
					printf("Received:\n%s\n", buffer_acesso);
				}
				//write(1, buffer_acesso, strlen(buffer_acesso));
				if(strcmp(buffer_acesso, "POPREQ\n")==0);
				{		
					for(i=0; i<MAX_STR; i++)
						buffer_acesso[i]='\0';

					strcpy(buffer_acesso, answer_with_bestpop(root));
					tamanho_buffer_acesso = strlen(buffer_acesso);
				}
				
			}
			if(FD_ISSET(fd_servUDP_access, &rfds)){
				if (strcmp(buffer_acesso, " ") != 0)
				 	{
				 		n_acesso=sendto(fd_servUDP_access,buffer_acesso, tamanho_buffer_acesso, 0,(struct sockaddr*)&addr_acesso,addrlen_acesso);
						check_error(n_acesso,-1, "sendto in Servidor de Acesso");
						if (flag_debug == 1)
						{
							printf("Sent:\n%s\n", buffer_acesso);
						}
				 	} 	
				if(urroot==0){ 
					x=0;
					for(j=0; j<root->n_bestpops; j++){  // identificar se bestpops com lugares vazios
						if(strcmp(bestpops[j], " ")==0){
							x++;								
						}
					}
					if(x>0){        // se bestpops não cheio raiz envia msg PQ para baixo		
						
						free_bestpops=x;
						Pop_query(free_bestpops, root); 
					}else if(x==0){  // se bestpops cheio não envia nada e mete os vetores a zero
						for(j=0; j<65535 ; j++){	
							strcpy(reply[j], " ");
							strcpy(query[j]," ");
						}
					}
				}
			}		
		}
		else
		{
//se nao for raiz, se recebeu algo da sua ligação a TCP a montante
/********************************************************************************************************************/

			if(FD_ISSET(fd_clientTCP, &rfds)){
				for(i=0;i<(MAX_STR-1);i++){
					buffer_client[i]='\0';
				}
				//printf("ANTES DE ENTRAR free_tcpsessions é : %d\n", root->free_tcpsessions);
				n_client=read(fd_clientTCP, buffer_client, MAX_STR-1);
				check_error(n,-1, "read in read_from_serv_TCP");
				buffer_client[n_client] = '\0';
				if (n_client == 0) //se nao tiver lido nada, alguem se desconectou.Vai fechar a ligação
				{
					n_client_temp=0;
					fd_clientTCP=-1;
					request = 1;
					fd_rs = RS_clientUDP_send(root->streamID,root->rsaddr, root->rsport,request, root->ipaddr, root->uport);
					send_down("BS\n", root);
					if (flag_debug == 1)
					{
						printf("Sent:\nBS\n", buffer_acesso);
					}
					stream_flowing=0;
					for(j=0;j<root->tcpsessions;j++){

						if(root->stream_flowing[j]==1 && root->fd_occupation[j]==1){
							root->stream_flowing[j]=0;
						}

					}

				}else{
					fd_clientTCP= read_from_serv_TCP(fd_clientTCP,buffer_client, root); //vai ver o que recebeu e tomar decisoes sobre o que fazer
				}
		
			}
		}

	}//while(1)


	for(i=0; i< root->tcpsessions; i++){
		close(afd_jusante[i]);
	}
	close(fd_servTCP);
	close(fd_clientTCP);
	close(fd_servUDP_access);
	close(fd_fonte);
	close(fd_servTCP);
	close(fd_extra);

	return 0;
}









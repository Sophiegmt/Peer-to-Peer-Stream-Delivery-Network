#include "geral.h"


/****************************************************************************************
*  Função: check_error()
*	
* 	Verifica se nao ocorreu um erro: caso sim, então sai; caso não, então prossegue.
*
***************************************************************************************/
void check_error(int n,int value, char str[]){
	if(n==value) //se nao conseguir ler
	{

		fprintf(stderr,"error: %s: %s\n",str,  gai_strerror(n));
		fprintf(stderr,"ERROR: %s\n", strerror(errno));
					/*error*/exit(1);
	}

}

/****************************************************************************************
*  Função: root_in_bestpops()
*	
* 	Ve o vetor todos dos bestpops, e apaga todos, excepto se estiver a propria raiz nos 
*		bespops. Devolve 1 se encontrou a raiz, 0 se não.
*
****************************************************************************************/
int root_in_bestpops(iamroot * root)
{
	char ipport[64] = {'\0'}, stream[1024]  = {'\0'}, raiz_ipport[64] = {'\0'};
	int escreveu = 0, raiz =-1, j=0, x = 0;


	strcat(raiz_ipport, root->ipaddr);
	strcat(raiz_ipport, ":");
	strcat(raiz_ipport, root->tport);

	for (int j = 0; j < root->n_bestpops; j++)
	{
		if (strcmp(bestpops[j], " ")!=0) //se tiver algum pop
		{
			sscanf(bestpops[j], "%s %s", stream, ipport);
			if (strcmp(raiz_ipport, ipport) != 0) //se nao for igual a raiz retirar
			{
				strcpy(bestpops[j], " ");	
				num_bestpops[j] == 0;
			}
			else
				x = 1;	
		}
	}
	return x;
}
/****************************************************************************************
*  Função: clear_bestpops_with_root()
*	
* 	Quando e raiz, verifica se quem se desconectou imediatamente a seu jusante está 
*		no vetor best pops, e se estiver, retira-o e substitui pelo seu proprio porto
*		que esta novamente livre
*
****************************************************************************************/
void clear_bestpops_with_root(iamroot * root, int i)
{
	char ipport[64] = {'\0'}, stream[1024]  = {'\0'}, raiz_ipport[64] = {'\0'};
	int escreveu = 0, raiz =-1, j=0;

	strcat(raiz_ipport, root->ipaddr);
	strcat(raiz_ipport, ":");
	strcat(raiz_ipport, root->tport);

	for (int j = 0; j < root->n_bestpops; j++)
	{
		if (strcmp(bestpops[j], " ")!=0) //se tiver algum pop
		{
			sscanf(bestpops[j], "%s %s", stream, ipport);
			if (strcmp(raiz_ipport, ipport) == 0) //se for igual ao pop a retirar
				raiz = j;		
		}
	}


	for (j= 0; j < root->n_bestpops; j++)
	{
		if (strcmp(bestpops[j], " ")!=0) //se tiver algum pop
		{
			sscanf(bestpops[j], "%s %s", stream, ipport);
			if (strcmp(root->TCPconnect[i], ipport) == 0) //se for igual ao pop a retirar
			{
				strcpy(bestpops[j], " "); //apagou o pop que saiu
				num_bestpops[j] = 0;
				if (raiz != -1)
				{
					num_bestpops[j] ++;
				}
				else
				{
					if(strcmp(bestpops[0]," ")!=0) //se nao estiver vazio
					{
						strcpy(bestpops[j],bestpops[0]); //copiar para este 
						num_bestpops[j] = num_bestpops[0];
						strcpy(bestpops[0],construir_POPRESP(root, 1, stream));
						num_bestpops[0] = 1;
						escreveu++;
					}
				}
					
			}
		}
	}
	if (escreveu == 0)
	{
		strcpy(bestpops[0],construir_POPRESP(root, 1, stream));
		num_bestpops[0] = 1;
	}
}

//PODE SE APAGAR
/****************************************************************************************
*  Função: clear_bestpop()
*	
* 	Se nao for raiz manda a mensagem para montante, se for raiz, verifica se o vetor
*		bestpops contem aquele ip e porto de quem se desconectou, e se sim, remove-o
*
****************************************************************************************/
void clear_bestpop(iamroot * root, int urroot, char buffer[], int fd)
{
	char to_remove[1024] = {'\0'};
	char ipport[64] = {'\0'}, stream[1024]  = {'\0'};

	if (urroot != 0)
	{
		send_up(buffer, fd);
	}
	else if (urroot == 0)
	{
		strcpy(to_remove,rmv_tag(buffer, 2));
		for (int j = 0; j < root->n_bestpops; j++)
		{
			if (strcmp(bestpops[j], " ")!=0)
			{
				sscanf(bestpops[j], "%s %s", stream, ipport);
				if (strcmp(to_remove, ipport) == 0)
				{
					strcpy(bestpops[j], " ");
					num_bestpops[j] = 0;
				}
			}
		}
	}
}

//PODE SE APAGAR

/****************************************************************************************
*  Função: increment_bestpop()
*	
* 	Se nao for raiz manda a mensagem para montante, se for raiz, verifica se o vetor
*		bestpops contem aquele ip e porto de quem se desconectou, e se sim, aumenta
*		o numero disponiveis para se ligar
*
****************************************************************************************/
void dec_or_inc_bestpop(iamroot * root, int urroot, char buffer[], int fd, int WhichOne, int np_in_root)
{	int j =0;
	char to_increment[1024] = {'\0'};
	char ipport[64] = {'\0'}, stream[1024]  = {'\0'};

	if (urroot != 0)
	{
		send_up(buffer, fd);
	}
	else if (urroot == 0)
	{
		if (np_in_root == 1)
		{
			strcat(to_increment, root->ipaddr);
			strcat(to_increment, ":");
			strcat(to_increment, root->tport);
		}
		else
			strcpy(to_increment,rmv_tag(buffer, 2));

		for (j= 0; j < root->n_bestpops; j++)
		{
			if (strcmp(bestpops[j], " ")!=0)
			{
				sscanf(bestpops[j], "%s %s", stream, ipport);
				if (strcmp(to_increment, ipport) == 0)
				{
					if (WhichOne == 0)
					{
						num_bestpops[j]--;
						if (num_bestpops[j] == 0)
						{
							strcpy(bestpops[j], " ");
						}
					}
					else
						num_bestpops[j]++;
				}
			}
		}
	}
}


/****************************************************************************************
*  Função: Pop_query()
*	
* 	Display da mensagem "PQ hex bestpops\n" quando queremos procurar pontos de acesso
* para por no vetor bestpops
*
****************************************************************************************/
void Pop_query(int x, iamroot * root)
{
	int i;
	char hexvalue[5]={'\0'}, c[64]={'\0'};
	char buffer[1024] = {'\0'};

	strcpy(hexvalue,hexadecimal(hex));
	hexvalue[4] = '\0';
	

	strcpy(buffer, "PQ ");
	strcat(buffer, hexvalue);
	strcat(buffer, " ");
	sprintf(c, "%d", x);
	strcat(buffer, c);
	strcat(buffer, "\n");
	

	strcpy(query[hex],buffer);
	
	send_down(query[hex], root);
	if (flag_debug == 1)
	{
		printf("Sent:\n%s\n", query[hex]);
	}
	hex++;
}

/****************************************************************************************
*  Função: Pop_rep()
*	
* 	Envio da mensagem "PR queryID ip:port avails\n" entre pares para enviar os dados
*	do ponto de acesso livre para a raiz. Uma vez na raiz, vai introduzir esses dados no
*	bestpops
*
****************************************************************************************/
int Pop_rep(iamroot * root, int urroot, char ip_port[], int x, int avails, int fd_up, char buffer[]){

	int j=0, ja_existe = -1;
	char aux[64]={'\0'}, stream[64]={'\0'}, ipport[64]={'\0'};
	
	x=0;

	if (urroot == 0)
	{
		for(j=0; j<root->n_bestpops; j++){  // identificar se bestpops com lugares vazios
			if(strcmp(bestpops[j]," ")==0){
				x++;								
			}
			else
			{
				sscanf(bestpops[j], "%s %s", stream, ipport);
				if (strcmp(ip_port, ipport) == 0)
					ja_existe = j;
			}
		}
	}
	
	if(urroot==0 && x>0 ){
		j=0;
		if (ja_existe == -1)
		{
			while(strcmp(bestpops[j], " ")!=0 && j< root->n_bestpops){
			j++;
			} //encontra lugar vazio
			if(j < root->n_bestpops){
				strcpy(aux, root->streamID);
				strcat(aux, " ");
				strcat(aux, ip_port);
				aux[strlen(aux)] = '\0';
				strcpy(bestpops[j], aux);	
				bestpops[j][strlen(aux)] = '\0';
				num_bestpops[j] = avails;
				x--;
			}	
		}
	}
	else if(urroot!=0)
	{
		send_up(buffer, fd_up);

	}

	return x;
}

/****************************************************************************************
*  Função: build_reply()
*	
* 	Construção da mensagem "PR queryID ip:port avails\n" entre pares para enviar os dados
*	do ponto de acesso livre para a raiz. 
*
****************************************************************************************/
int build_reply(char hexvalue[], iamroot * root){
	int avails=0, queryID =0, rest=0, cnt=0, tempqueryID=0;
	char avails2[64]={'\0'}, extra[64]={'\0'};

	sscanf(hexvalue, "%x", &tempqueryID);
	queryID = strtol(hexvalue, NULL, 16);	 

	strcpy(extra, "PR ");
	strcat(extra, hexvalue);
	strcat(extra, " ");
	strcat(extra, root->ipaddr);
	strcat(extra, ":");
	strcat(extra, root->tport);
	strcat(extra, " ");
	avails = root->free_tcpsessions; 
	sprintf(avails2, "%d", avails);
	strcat(extra, avails2);
	strcat(extra, "\n");
	strcpy(reply[queryID],extra);

	return(queryID);
}

/****************************************************************************************
*  Função: hexadecimal()
*	
*		transforma inteiros decimais em chars hexadecimais
*
****************************************************************************************/
char * hexadecimal(int number){

	int rest = 0, i=0, x=0, j=0;
	char temp[5]={'\0'};
	static char value[5]={'\0'};

	if(number==0){
		strcpy(value,"0000\0");
	}else{

		while(number > 0 && i >=0){
			rest = number%16;
			temp[i] = rest<10 ? (char)(rest+48) : (char)(rest+55);
			number/=16;
			i++;
		} for(j=i; j<4 ;j++) temp[j]='0'; 

		for (j = 3 ; j>=0 ;j--)
		{
			value[x]=temp[j];
			x++;
		} 
	}

	return (value);
}

/****************************************************************************************
*  Função: WELCOME()
*	
* 	Display da mensagem "WELCOME ..." quando a ligacao TCP a jusante é estabelecida
*
****************************************************************************************/
void WELCOME (iamroot * root, int i){
	int n=0;
	char buffer[1024]={'\0'};

	strcpy(buffer, "WE ");
	strcat(buffer, root->streamID);
	strcat(buffer, "\n");
	n=write(root->fd[i], buffer , strlen(buffer));
	check_error(n, -1, "write in WELCOME");
	if (flag_debug == 1)
	{
		printf("Sent:\n%s\n", buffer);
	}
}


/****************************************************************************************
*  Função: DATA()
*	
* 	Display da mensagem "DA ..." mensagem a ser enviada da fonte para as aplicações 
* da árvore.
*
****************************************************************************************/
void DATA (iamroot * root,  char data[],int tamanho_n)
{
	int n=0;
	char buffer[1024] = {"\0"};
	char tamanho[64]={"\0"};

	sprintf(tamanho, "%d", tamanho_n);

	strcpy(buffer, "DA ");
	strcat(buffer, tamanho);
	strcat(buffer, "\n");
	strcat(buffer, data);
	strcat(buffer, "\n");

	send_down(buffer, root);
	if (flag_debug == 1)
	{
		printf("Sent:\n%s\n", buffer);
	}
}

/****************************************************************************************
*  Função: REDIRECT()
*	
* 	Display da mensagem "REDIRECT ..." quando a ligacao TCP a jusante ja está ocupada
*
****************************************************************************************/
void REDIRECT(int fd_extra, iamroot* root, int i){

	int n=0;
	char buffer[1024]={'\0'};

	strcpy(buffer,"RE ");
	strcat(buffer, root->TCPconnect[i]);
	n=write(fd_extra, buffer , strlen(buffer)); //write "RE ipaddr:tport\n" in fd_TCP_jusante
	check_error(n, -1, "write in REDIRECT");
	if (flag_debug == 1)
	{
		printf("Received:\n%s\n", buffer);
	}
}

/****************************************************************************************
*  Função: CheckIP_Format()
*	
* 	Verifica o formato do ip e retorna erro se nao válido
*
****************************************************************************************/
void CheckIP_Format(char *WhichOne, char IP[]) // ve se o IP esta no formato certo
{
	int y = 0, pontos = 0;

	while (IP[y] != '\0') //grava ip de sevidor raizes
	{
		if ((IP[y] < '0' || IP[y] > '9') && IP[y] != '.') //ve se sao so numeros e pontos
		{
			printf("Endereço IP %s com caracter não válido.\n", WhichOne);
			exit(1);
		}
		if((IP[y] == IP[y-1]) && IP[y] == '.')
		{
			printf("Endereço IP %s tem secção não válida.\n", WhichOne);
			exit(1);
		}
		if (IP[y] == '.') pontos++; //conta os pontos
		y++;
	}
	if (pontos != 3 || IP[y-1] == '.') { //da erro se nao tiver o formato certo
		printf("Endereço IP %s tem número de secções não válido.\n", WhichOne);
		exit(1);
	}
}



/****************************************************************************************
*  Função: CheckPortFormat()
*	
* 	Verifica o formato do porto e retorna erro se nao válido
*
****************************************************************************************/
void CheckPortFormat(char *WhichOne, char port[]) //ve se o porto consiste so em numeros
{
	int y = 0;

  	while (port[y] != '\0') //grava ip de sevidor raizes
  	{
  		if (port[y] < '0' || port[y] > '9')
  		{ // ve se sao so numeros
  			printf("Porto %s não válido. \n", WhichOne);
  			exit(1);
  		}
  		y++;
  	}
  }



/****************************************************************************************
*Função: init()
*
*  Inicializa as variáveis gerais e da struct "iamroot" com os dados do enunciado.
*
****************************************************************************************/
  void init(iamroot * newroot){

	//initializacao dos dados
  	for(int i=0; i <64; i++){
  		stream_raiz[i] = '\0';
  	}
  	flag_data = 1;
  	flag_debug = 0;
  	flag_sinopse = 0;

  	strcpy(newroot->streamID," ");
  	strcpy(newroot->ipaddr, " ");
  	strcpy(newroot->tport,"58000");
  	strcpy(newroot->uport,"58000");
  	strcpy(newroot->rsaddr,"192.168.1.1");
  	strcpy(newroot->rsport,"59000");

  	newroot->tcpsessions=1;
  	newroot->free_tcpsessions = 1;
  	newroot->n_bestpops=1;
  	newroot->tsecs=5;
  	newroot->fd_pai=0;

  //	init_vetores_fd(&newroot);
  }

/****************************************************************************************
*Função: init_vetores_fd()
*
*    Inicializa as os vetores FD da struct "iamroot" com os valores 0 pois no início as 
*  ligações TCP ainda estão todas livres.
*
****************************************************************************************/
  void init_vetores_fd(iamroot * newroot){
  	int j;
	newroot->fd = (int*) malloc(newroot->tcpsessions * sizeof(int)); //vetor para guardar quais serao as ligaçoes
	newroot->fd_occupation = (int*) malloc(newroot->tcpsessions * sizeof(int)); //vetor paraguardar quais estao livres
	newroot->TCPconnect =(char **)malloc(newroot->tcpsessions * sizeof (char*));
	newroot->stream_flowing = (int*) malloc(newroot->tcpsessions * sizeof(int)); //vetor paraguardar quais estao livres

	//iniciliazacao dos vetores
	for(j=0; j< newroot->tcpsessions; j++)
	{
		newroot->fd[j]= -1;
		newroot->stream_flowing[j]=0;
		newroot->fd_occupation[j]=0;
		newroot->TCPconnect[j] = (char *)malloc(1024* sizeof (char));
		strcpy(newroot->TCPconnect[j], " ");
	}
}

/****************************************************************************************
*Função: keep_root_info()
*
*    Guarda o ip e o porto da Raiz nas variáveis globais WhoIsRoot_IP e WhoIsRoot_U 
*
****************************************************************************************/
void keep_root_info(){

	int i=0, x=0;

	for(i=0; i<strlen(stream_raiz) && stream_raiz[i] != ' ' ; i++ );
	{    // guardar IP e Porto da Raiz
		x = 0; i++;

		//sscanf(stream_raiz, "%[^ ] %[^:]:%[^ ]", )
		while (stream_raiz[i] != ':') {
			WhoIsRoot_IP[x]=stream_raiz[i];
			i++; x++;
		}
		i++;
		x = 0;
		while (stream_raiz[i]!= '\0') {
			WhoIsRoot_U[x]=stream_raiz[i];
			i++; x++;
		}
	}
}

/****************************************************************************************
*Função: construir_POPRESP()
*
*    Constroi a mensagem de envio do servidor de acesso à applicação que pediu o ip e 
*  porto da ligacao TCP livre.
*	 Se first == 1, vai buscar os dados à estrutura, se first == 0, vai buscar os dados ao 
*  buffer
****************************************************************************************/
char * construir_POPRESP(iamroot * root, int first, char buffer_recebido[]){
	
	static char buffer[1024]={'\0'};

	strcpy(buffer, root->streamID);
	strcat(buffer, " ");

	if (first == 1 )
	{
		strcat(buffer, root->ipaddr);
		strcat(buffer, ":");
		strcat(buffer, root->tport);
		strcat(buffer, " ");
	}
	else
	{
		strcat(buffer, rmv_tag(buffer_recebido));
	}
	strcat(buffer, "\n");

	return buffer;
}


char *answer_with_bestpop(iamroot * root)
{
	static char answer[1024] = {'\0'};
	int i=0;


	strcpy(answer, "POPRESP ");

	while(i < root->n_bestpops)
	{ 
		if ((strcmp(bestpops[i], " ")!=0))
		{
			break;
		}
		i++; 
	} //anda até encontrar algum pop 

	if(i < root->n_bestpops){  //se nao tiver chegado ao fim da lista sem pops
		strcat(answer, bestpops[i]);
		if (num_bestpops[i] == 1)
		{
			strcpy(bestpops[i], " "); //bestpops=(null)
			num_bestpops[i] = 0;
		}
		else
			num_bestpops[i] --;

		answer[strlen(answer)] = '\0';
				
		return answer;
	}else{
		Pop_query(root->n_bestpops, root);
		return " ";
	}
}

/****************************************************************************************
*Função: rmv_tag()
*
*    retira a tag da mensagem de envio e guarda os dados em strings
*  	 retorna os dados
*	
****************************************************************************************/
char * rmv_tag(char buffer[], int partes)
{

	char tag[64] = {'\0'};
	static char rest[64] = {'\0'};
	static char rest2[64] = {'\0'};
	
	if(partes == 2)
	{
		sscanf(buffer, "%s %s", tag, rest);
		return rest;
	}
	else if(partes == 3)
	{
		sscanf(buffer, "%s %s %s", tag, rest, rest2);
		return rest2;
	}
	
}
/****************************************************************************************
*Função: send_up()
*
*    Manda para montante informacao de que recebeu de jusante
*
****************************************************************************************/
void send_up(char buffer[], int fd)
{
	int n=0;

	n=write(fd, buffer , strlen(buffer));
	if(n==-1) 
	{
		fprintf(stderr,"ERROR: write in send_up: %s\n", strerror(errno));
		/*error*/exit(1);
	}
}


/****************************************************************************************
*Função: send_down()
*
*    Manda para todas as ligaçoes a jusante a informacao de que recebeu de montante
*	
****************************************************************************************/
void send_down(char buffer[], iamroot * root)
{
	int n=0, i=0;
	
	for (i = 0; i < root->tcpsessions; ++i)
	{
		if (root->fd_occupation[i] == 1)
		{
			if (root->fd[i] > 0)
			{
				n=write(root->fd[i], buffer , strlen(buffer));
				check_error(n,-1, "write in send_down");
			}
		}
	}
}

/****************************************************************************************
*Função: cmp_to_we()
*
*    Compara a string recebida com o formato de uma string Welcome
*	retorna 1 se for essa instrução
****************************************************************************************/
int cmp_to_we(iamroot * root, char buffer[])
{
	char cmp_tag[1024] = {'\0'}, cmp_stream[1024] = {'\0'}, tag[64] = {'\0'}, stream[64] = {'\0'};


	sscanf(buffer, "%s %s", tag, stream);

	strcpy(cmp_tag, "WE");
	strcat(cmp_stream, root->streamID);

	if (strcmp(cmp_tag,tag) == 0)
	{
		if (strcmp(cmp_stream,stream) == 0)
			return 1;
	}
	return 0;
}

//PODE SE APAGAR
/****************************************************************************************
*Função: cmp_to_cl()
*
*    Compara a string recebida com o formato de uma string Clear pop from bestpops
*	retorna 1 se for essa instrução
****************************************************************************************/
int cmp_to_cl(char buffer[])
{
	char compare[1024] = {'\0'}, tag[64] = {'\0'}, ipport[64] = {'\0'}, rest[1024] = {'\0'}, inicio[1024] = {'\0'};


	sscanf(buffer, "%[^\n] %[^\n]", inicio, rest);
	sscanf(buffer, "%s %s", tag, ipport);
	strcpy(compare, "CL");

	if (strcmp(tag,compare) == 0)
	{
		return 1;
	}
	return 0;
}

//PODE SE APAGAR
/****************************************************************************************
*Função: cmp_to_ic()
*
*    Compara a string recebida com o formato de uma string incrementar pop from bestpops
*	retorna 1 se for essa instrução
****************************************************************************************/
int cmp_to_ic(char buffer[])
{
	char compare[1024] = {'\0'}, tag[64] = {'\0'}, ipport[64] = {'\0'};


	sscanf(buffer, "%s %s", tag, ipport);
	strcpy(compare, "IC");

	if (strcmp(tag,compare) == 0)
	{
		return 1;
	}
	return 0;
}


//PODE SE APAGAR
/****************************************************************************************
*Função: cmp_to_dc()
*
*    Compara a string recebida com o formato de uma string decrementar pop from bestpops
*	retorna 1 se for essa instrução
****************************************************************************************/
int cmp_to_dc(char buffer[])
{
	char compare[1024] = {'\0'}, tag[64] = {'\0'}, ipport[64] = {'\0'};


	sscanf(buffer, "%s %s", tag, ipport);
	strcpy(compare, "DC");

	if (strcmp(tag,compare) == 0)
	{
		return 1;
	}
	return 0;
}

/****************************************************************************************
*Função: cmp_to_np()
*
*    Compara a string recebida com o formato de uma string New Pop
*	retorna 1 se for essa instrução
****************************************************************************************/
int cmp_to_np(char buffer[])
{
	char compare[1024] = {'\0'}, real[64] = {'\0'}, real_ipporto[64] = {'\0'};


	sscanf(buffer, "%s %s", real, real_ipporto);
	strcpy(compare, "NP");

	if (strcmp(real,compare) == 0)
	{
		return 1;
	}
	return 0;
}

/****************************************************************************************
*Função: cmp_to_re()
*
*    Compara a string recebida com o formato de uma string Redirect
*	retorna 1 se for essa instrução
****************************************************************************************/
int cmp_to_re(char buffer[])
{
	char compare[1024] = {'\0'}, real[64] = {'\0'}, real_ipporto[64] = {'\0'};

	sscanf(buffer, "%s %s", real, real_ipporto);
	strcpy(compare, "RE");

	if (strcmp(real,compare) == 0)
	{
		return 1;
	}
	return 0;
}

/****************************************************************************************
*Função: cmp_to_da()
*
*    Compara a string recebida com o formato de uma string de dados da fonte
*	retorna 1 se for essa instrução
****************************************************************************************/
int cmp_to_da(char buffer[])
{
	char compare[1024] = {'\0'}, real[64] = {'\0'}, real_ipporto[64] = {'\0'};


	sscanf(buffer, "%s", real);
	strcpy(compare, "DA");

	if (strcmp(real,compare) == 0)
	{
		return 1;
	}
	return 0;
}

/*************************************************************************************** 
 *  Função: cmp_to_BS()
 *
 * compara string recebida com BS - Broken Stream
 *
 ****************************************************************************************/
int cmp_to_BS(char buffer[]){

	char compare[1024] = {'\0'}, tag[64] = {'\0'};

	sscanf(buffer, "%s", tag);
	strcpy(compare, "BS");
	
	if (strcmp(compare,tag) == 0)
	{
		return 1;
	}
	return 0;

}


 /*************************************************************************************** 
 *  Função: cmp_to_SF()
 *
 * compara string recebida com BS - Broken Stream
 *
 ****************************************************************************************/
int cmp_to_SF(char buffer[]){

	char compare[1024] = {'\0'}, tag[64] = {'\0'};

	sscanf(buffer, "%s", tag);
	strcpy(compare, "SF");
	
	if (strcmp(compare,tag) == 0)
	{
		return 1;
	}
	return 0;

}


/****************************************************************************************
*Função: cmp_to_popreply()
*
*    Compara a string recebida com o formato de uma string pop reply
*	retorna 1 se for essa instrução
****************************************************************************************/
int cmp_to_PR(char buffer[], char ip_port[], int queryID, int *avails)
{
	char compare[1024] = {'\0'}, tag[64] = {'\0'};
	char tempqueryID[4]={'\0'};
	int rest=0, igual=0;
	int cnt=0;

	queryID=0;
	sscanf(buffer, "%s %s %s %d", tag, tempqueryID, ip_port, avails);
	sscanf(buffer, "%s", tag);
	
	queryID = strtol(tempqueryID, NULL, 16);

	strcpy(compare, "PR");
	
	if (strcmp(compare,tag) == 0)
	{
		return 1;
	}
	return 0;
}
/****************************************************************************************
*Função: cmp_to_popquery()
*
*    Compara a string recebida com o formato de uma string pop query
*	retorna 1 se for essa instrução
****************************************************************************************/
int cmp_to_PQ(char buffer[],char queryID[], int x)
{
	char compare[1024] = {'\0'}, tag[64] = {'\0'};

	sscanf(buffer, "%s %s %d", tag, queryID, &x);
	strcpy(compare, "PQ");
	
	if (strcmp(compare,tag) == 0)
	{
		return 1;
	}
	return 0;
}


 /*************************************************************************************** 
 *  Função: build_TQ()
 *
 * Constroi mensagem Tree query e envia para baixo
 *
 ****************************************************************************************/

void build_TQ(iamroot*root)
{
	char buffer[128]={'\0'};
	int i = 0, n = 0;

	for (i = 0; i < root->tcpsessions; i++)
	{
		if (root->fd_occupation[i] == 1)
		{
			strcpy(buffer, "TQ ");
			strcat(buffer, root->TCPconnect[i]);
			strcat(buffer, "\n");
			buffer[strlen(buffer)] = '\0';
			n = write(root->fd[i], buffer, strlen(buffer));
			check_error(n, -1, "write in build_TQ");

		}
	}
}


 /*************************************************************************************** 
 *  Função: cmp_to_TQ()
 *
 * compara string recebida com Tree query 
 *
 ****************************************************************************************/
int cmp_to_TQ(char buffer[]){
	char compare[1024] = {'\0'}, tag[64] = {'\0'}, extra[64]={'\0'};

	sscanf(buffer, "%s %s", tag, extra);
	strcpy(compare, "TQ");
	
	if (strcmp(compare,tag) == 0)
	{
		return 1;
	}
	return 0;
}


 /*************************************************************************************** 
 * Função : build_TR
 *
 *  Constroi a string Tree_Rreply se for o ip_porto correspondente e envia para cima.
 *
 ****************************************************************************************/
void build_TR(char buffer[], iamroot*root, int  fd){
	int i=0;
	char new_buffer[1024]={'\0'}, sessions[32] = {'\0'};
	char ip_port[64]={'\0'};
	char root_ip_port[64]={'\0'};

	strcpy(ip_port,rmv_tag(buffer,2));
	strcpy(root_ip_port, root->ipaddr);
	strcat(root_ip_port, ":");
	strcat(root_ip_port, root->tport);


	if(strcmp(ip_port, root_ip_port)==0){
		strcpy(new_buffer, "TR ");
		strcat(new_buffer, ip_port);
		strcat(new_buffer, " (");
		sprintf(sessions, "%d",root->tcpsessions);
		strcat(new_buffer, sessions);


		for(i=0; i<root->tcpsessions;i++){
			if(strcmp(root->TCPconnect[i], " " )!=0){
				strcat(new_buffer, " ");
				strcat(new_buffer,root->TCPconnect[i] );
			}
		} 

		strcat(new_buffer, ")\n");
		new_buffer[strlen(new_buffer)] = '\0';

		send_up(new_buffer,fd);
	}else{
		send_down(buffer, root);
	}

}

/*************************************************************************************** 
 *Função: cmp_to_TR
 *
 * Compara a string recebida com Tree_Reply
 *
 ****************************************************************************************/
int cmp_to_TR(char buffer[]){

	char compare[1024] = {'\0'}, tag[64] = {'\0'};

	sscanf(buffer, "%s", tag);
	strcpy(compare, "TR");
	
	if (strcmp(compare,tag) == 0)
	{
		return 1;
	}
	return 0;
}

/***************************************************************************************
 * Função: decrementar_x()
 *
 * Decrementa o numero de Ligações TCP livre a jusante
 *
 ****************************************************************************************/
void decrementar_x(char buffer[]){

	char  tag[64] = {'\0'}, queryID[4]= {'\0'}, c[20]= {'\0'};
	int x=0,i=0;

	sscanf(buffer, "%s %s %d", tag, queryID, &x);

	for(int i=0; i<strlen(buffer); i++)
		buffer[i] = '\0';

	x--;
	strcpy(buffer, tag);
	strcat(buffer, " ");
	strcat(buffer, queryID);
	strcat(buffer, " ");
	sprintf(c, "%d", x);
	strcat(buffer, c);
	strcat(buffer, "\n");

}

int cmp_to_me(char buffer_acesso[], iamroot * root)
{
	char compare[32] = {'\0'}, tag[32] = {'\0'}, ipport[64] = {'\0'}, stream[1024] = {'\0'};

	sscanf(buffer_acesso, "%s %s %s", tag, stream, ipport);

	strcpy(compare, root->ipaddr);
	strcat(compare, ":");
	strcat(compare, root->tport);

	compare[strlen(compare)] = '\0';

	if(strcmp(compare, ipport) == 0)
	{
		return 1;
	}
	return 0;
}



void continue_TQ(char buffer[], iamroot * root)
{
	char lixo[64] = {'\0'}, filhos[MAX_STR] = {'\0'}, filho_atual[64] = {'\0'}, filhos_restantes[MAX_STR] = {'\0'};
	char query[64] = {'\0'};
	int num_filhos = 0, i = 0, conta = 0;


	sscanf(buffer, "%[^(](%d%[^)])", lixo, &num_filhos, filhos);

	strcat(filhos, " !\n");
	filhos[strlen(filhos)] = '\0';

	if (strcmp(filhos, " !\n") != 0)
	{
		for (i = 0; i < num_filhos; i++)
		{
			if (strcmp(filhos, "!\n") != 0)
			{
				conta++;
				sscanf(filhos, "%s %[^\n]", filho_atual, filhos_restantes);
				strcpy(filhos, filhos_restantes);
				filhos[strlen(filhos)] = '\0';

				strcpy(query, "TQ ");
				strcat(query, filho_atual);
				strcat(query, "\n");
				query[strlen(query)] = '\0';
				send_down(query, root);
				if (flag_debug == 1)
				{
					printf("Sent:\n%s\n", query);
				}
			}
		}
	}
	dar_para_receber += conta;
}


/****************************************************************************************
*Função: new_pop()
*
*    Forma a string New Pop a ser mandada a montante
*
****************************************************************************************/
char * new_pop(iamroot * root)
{
	static char buffer[1024] = {'\0'};
	strcpy(buffer, "NP ");
	strcat(buffer, root->ipaddr);
	strcat(buffer, ":");
	strcat(buffer, root->tport);
	strcat(buffer, "\n");

	return buffer;
}
#ifndef GERAL_H
#define GERAL_H

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>

#define MAX_STR 1024
#define max(A,B) ((A)>=(B)?(A):(B))

char ** bestpops;
int * num_bestpops;;

char stream_raiz[64];
char WhoIsRoot_IP[64];
char WhoIsRoot_U[64];

char query[65535][1024];
char reply[65535][1024];

//criar_SAcesso(struct iamroot* root);
int flag_data;
int flag_debug;
int flag_sinopse;
int flag_hex;

int hex;
int stream_flowing;
int n_client_temp;
int print_tq;
int dar_para_receber;

typedef struct iamroot{

	char streamID[64];
	char ipaddr[64];
	char tport[64];
	char uport[64];
    //   GUARDAR DADOS SERVIDOR DE RAIZ
	char rsaddr[64]; 
	char rsport[64]; 

	//    GUARDAR DADOS DO SERVIDOR DE FONTE
	char fsname[64];
	char fsport[64]; 
	char fsaddr[64];
	
	// dados montantes
	char pai[64];

	// dados jusantes
	int * fd;
	int * fd_occupation;
	char ** TCPconnect;
	int * stream_flowing;
    int fd_pai;
	int tcpsessions;
	int free_tcpsessions;
	int n_bestpops;


	int tsecs;

}iamroot;

void check_error();
void send_up();
void send_down();

char * construir_POPRESP();
void WELCOME();
void REDIRECT();
void DATA();
char * answer_with_bestpop();


int Pop_rep();
void Pop_query();
int build_reply();
char * hexadecimal();

void CheckIP_Format();
void CheckPortFormat();
void init_vetores_fd();
void init();
void keep_root_info();

char * rmv_tag();

int cmp_to_we();
int cmp_to_np();
int cmp_to_da();
int cmp_to_BS();
int cmp_to_SF();
int cmp_to_re();
int cmp_to_me();

int cmp_to_PQ();
int cmp_to_PR();

int cmp_to_TQ();
void build_TQ();
int cmp_to_TR();
void build_TR();
void continue_TQ();

int cmp_to_dc();
int cmp_to_ic();
int cmp_to_cl();
char * dec_or_inc();

void decrementar_x();
void clear_bestpops_with_root();
void clear_bestpop();
char * clear_pop_message();


void dec_or_inc_bestpop();

int cmp_to_TQ();
void build_TQ();

int root_in_bestpops();

char * new_pop();



#endif // GERAL_H
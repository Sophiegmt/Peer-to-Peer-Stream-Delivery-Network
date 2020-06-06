
#include "interface.h"


int interface(char buffer[], iamroot* root, int urroot, int *fd_rs, int *request)
{
  
  int i = 0;
  while(buffer[i] || buffer[i] == ' ')
  {
    buffer[i] = tolower(buffer[i]);
    i++;
  }

  if (strcmp(buffer,"streams\n") == 0)
  {
  	*fd_rs = RS_clientUDP_send(root->streamID, root->rsaddr, root->rsport, 2, root->ipaddr, root->uport);
  	*request = 2;	 	
  } 
  else if (strcmp(buffer, "status\n") == 0)
  {
    status(root, urroot);
  }
  else if (strcmp(buffer, "display on\n") == 0)
  {
    flag_data = 1;
  }
  else if (strcmp(buffer, "display off\n") == 0)
  {
    flag_data = 0;
  }
  else if (strcmp(buffer, "format ascii\n") == 0)
  {
    printf("FORMAT ASCII\n");
    flag_hex = 0;
  }
  else if (strcmp(buffer, "format hex\n") == 0)
  {
    printf("FORMAT HEX\n");
    flag_hex = 1;
  }
  else if (strcmp(buffer, "debug on\n") == 0)
  {
    flag_debug = 1;
  }
  else if (strcmp(buffer, "debug off\n") == 0)
  {
    flag_debug = 0;
  }
  else if (strcmp(buffer, "tree\n") == 0)
  {
  	printf("%s\n", root->streamID);
    build_TQ(root);
    print_tq = 1;
    dar_para_receber = (root->tcpsessions - root->free_tcpsessions);
  }
  else if (strcmp(buffer, "exit\n") == 0)
  {
    do_exit(root, urroot, fd_rs);
  }
  else
  {
      printf("buffer not recognized. Please choose from the following list:\n- streams;\n- status;\n- display on/off;\n- format ascii/hex;\n- debug on/off;\n- tree;\n- exit. \n \n");
  }
  return 0;
}


void do_exit(iamroot*root, int urroot, int *fd_rs){

  int j=0;
    if (urroot==0){ // se é a raiz
        
 		*fd_rs= RS_clientUDP_send(root->streamID, root->rsaddr, root->rsport,3, root->ipaddr, root->uport); // WHOISROOT
    
        // free do bestpops
        for(j=0; j< root->n_bestpops; j++)
        {
          free(bestpops[j]);
        }
        free(bestpops);
        free(num_bestpops);

    } 
    
    stay=0; // variável global que informa se o utilizador deu exit

}


void status(iamroot * root, int urroot)
{
  printf("Stream: %s\n\n", root->streamID);

  if (stream_flowing == 1)
  {
  	printf("Stream is flowing.\n\n");
  }
  else
  	printf("Stream is not flowing.\n\n");


  if (urroot == 0)
  {
    printf("I am root.\n\n");
    printf("IP address: %s\n", root->ipaddr);
    printf("UDP port: %s\n\n", root->uport);
  }
  else
  {
    printf("I am not root.\n\n");
    printf("IP address and TCP port amount: %s\n", root->pai);
  }
  printf("IP address and TCP port available: %s:%s\n\n", root->ipaddr, root->tport);
  printf("Number of TCP sessions supported: %d\n", root->tcpsessions);
  printf("Number of TCP sessions occupied: %d\n\n", (root->tcpsessions - root->free_tcpsessions));
  printf("Sessions downstream:\n");
  for (int i = 0; i < root->tcpsessions; ++i)
  {
      if (strcmp(root->TCPconnect[i], " ") == 0)
      {
        printf("In port %d: NONE\n", i);
      }
      else
        printf("In port %d: %s\n", i, root->TCPconnect[i]);

  }
}

#ifndef UDP_H
#define UDP_H

  int n, fd, addrlen;
  struct addrinfo hints, *res;
  struct sockaddr_in addr;


char * RS_clientUDP();
char * whoisroot();
int getfd_servUDP_access();
int PopReq_send();
char * PopReq_recv();

char * RS_clientUDP_receive();
int RS_clientUDP_send();



#endif // UDP_H
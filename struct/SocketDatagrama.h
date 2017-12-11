#ifndef SOCKET_DATAGRAMA_H_
#define SOCKET_DATAGRAMA_H_
#include "PaqueteDatagrama.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

class SocketDatagrama{
public:
   SocketDatagrama(int);
   ~SocketDatagrama();
   //Recibe un paquete tipo datagrama proveniente de este socket
   int recibe(PaqueteDatagrama & p);
 //Envía un paquete tipo datagrama desde este socket
   int envia(PaqueteDatagrama & p);
   void setTimeout(time_t, suseconds_t);
   void unsetTimeout();
   int recibeTimeout(PaqueteDatagrama & p);
   void setBroadcast();
   void unsetBroadcast();
private:
   struct sockaddr_in direccionLocal;
   struct sockaddr_in direccionForanea;
   int s; //ID socket
   struct timeval tiempofuera;
   struct timeval tiempoAntes;
   struct timeval tiempoDespues;
};

#endif

#ifndef __SOCKETDATAGRAMA__
#define __SOCKETDATAGRAMA__

#include "PaqueteDatagrama.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>

class SocketDatagrama{
    public:
        SocketDatagrama(int);
        ~SocketDatagrama();
        //Recibe un paquete tipo datagrama proveniente de este socket
        int recibe(PaqueteDatagrama &p);
        int recibeTimeout(PaqueteDatagrama &p);
        //Envía un paquete tipo datagrama desde este socket
        int envia(PaqueteDatagrama &p);
        int setTimeout(time_t segundos, suseconds_t microsegundos);
        int activaBroadcast(bool opcion);
        int unsetTimeout();
    private:
        struct sockaddr_in direccionLocal;
        struct sockaddr_in direccionForanea;
        struct timeval tiempofuera; 
        bool timeout;
        int s; //ID socket
};

#endif
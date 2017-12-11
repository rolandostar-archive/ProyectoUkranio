#include "SocketDatagrama.h"
#include "PaqueteDatagrama.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <strings.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>

SocketDatagrama::~SocketDatagrama(){
    close(s);
}
SocketDatagrama::SocketDatagrama(int puerto){
    s = socket(AF_INET, SOCK_DGRAM, 0);
    bzero((char *)&direccionLocal,sizeof(direccionLocal));
    direccionLocal.sin_family = AF_INET;
    direccionLocal.sin_addr.s_addr = INADDR_ANY;
    direccionLocal.sin_port = htons(puerto);
    bind(s, (struct sockaddr *)&direccionLocal,sizeof(direccionLocal));
}

int SocketDatagrama::recibe(PaqueteDatagrama & p){
    char recv[p.obtieneLongitud()],buffer[20];
    socklen_t forLen = sizeof(direccionForanea), locLen = sizeof(direccionLocal);
    int puerto, _res;

    getsockname(s, (struct sockaddr *)&direccionLocal, &locLen);
    //printf("Esperando Datos... %s:%d\n",inet_ntoa(direccionLocal.sin_addr),ntohs(direccionLocal.sin_port));
    gettimeofday(&tiempoAntes, NULL);
    _res = recvfrom(s, (char *)&recv, p.obtieneLongitud(), 0, (struct sockaddr *)&direccionForanea, &forLen);
    gettimeofday(&tiempoDespues, NULL);
    p.inicializaDatos(recv);
    p.inicializaPuerto(ntohs(direccionForanea.sin_port));
    p.inicializaIp(inet_ntoa(direccionForanea.sin_addr));
    return _res;
}

int SocketDatagrama::envia(PaqueteDatagrama & p){

    bzero((char *)&direccionForanea, sizeof(direccionForanea));
    direccionForanea.sin_family = AF_INET;
    direccionForanea.sin_addr.s_addr = inet_addr(p.obtieneDireccion());
    direccionForanea.sin_port = htons(p.obtienePuerto());

    return sendto(s, (void *)p.obtieneDatos(),p.obtieneLongitud(),0,(struct sockaddr*) &direccionForanea,sizeof(direccionForanea));
}

void SocketDatagrama::setTimeout(time_t segundos, suseconds_t microsegundos){
    tiempofuera.tv_sec = segundos;
    tiempofuera.tv_usec = microsegundos;

    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&tiempofuera, sizeof(tiempofuera));
}

int SocketDatagrama::recibeTimeout(PaqueteDatagrama & p){
    struct timeval result;
    int n = recibe(p);
    
    
    if (n < 0) {
        if (errno == EWOULDBLOCK) fprintf(stderr, "Tiempo para recepción transcurrido\n");
        else fprintf(stderr, "Error en recvfrom\n");
    }else{
        timersub(&tiempoDespues,&tiempoAntes,&result);
        //printf("%ld.%3lds\n", result.tv_sec,result.tv_usec);
    }
    return n;
}

void SocketDatagrama::unsetTimeout(){ setTimeout(0,0); }

void SocketDatagrama::setBroadcast(){
    int y = 1;
    setsockopt(s,SOL_SOCKET,SO_BROADCAST,&y,sizeof(int));
}


void SocketDatagrama::unsetBroadcast(){
    int n = 0;
    setsockopt(s,SOL_SOCKET,SO_BROADCAST,&n,sizeof(int));
}
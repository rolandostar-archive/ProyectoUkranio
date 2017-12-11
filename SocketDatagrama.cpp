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
#include <string.h>

SocketDatagrama::SocketDatagrama(){
    s = socket(AF_INET, SOCK_DGRAM, 0);
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
	socklen_t len = sizeof(direccionForanea);
	bind(s, (struct sockaddr *)&direccionLocal, sizeof(direccionLocal));
	int r = recvfrom(s, (char *) p.obtieneDatos(), p.obtieneLongitud(), 0, (struct sockaddr *)&direccionForanea, &len);
	char ip[16];
	unsigned char ip_int[4];
	memcpy(ip_int,&direccionForanea.sin_addr.s_addr,4);
	sprintf(ip,"%u.%u.%u.%u",ip_int[0],ip_int[1],ip_int[2],ip_int[3]);
	p.inicializaIp(ip);
	p.inicializaPuerto(direccionForanea.sin_port);
	//if(r>=0)
		//printf("Mensaje recibido desde %s:%d\n",p.obtieneDireccion(),p.obtienePuerto());
	return r;
}

int SocketDatagrama::envia(PaqueteDatagrama & p){

    bzero((char *)&direccionForanea, sizeof(direccionForanea));
    direccionForanea.sin_family = AF_INET;
    direccionForanea.sin_addr.s_addr = inet_addr(p.obtieneDireccion());
    direccionForanea.sin_port = htons(p.obtienePuerto());

    return sendto(s, (void *)p.obtieneDatos(),p.obtieneLongitud(),0,(struct sockaddr*) &direccionForanea,sizeof(direccionForanea));
}

int SocketDatagrama::responde(PaqueteDatagrama & p){
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

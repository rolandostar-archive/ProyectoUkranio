#include "SocketDatagrama.h"
#include <sys/time.h>
using namespace std;

SocketDatagrama::SocketDatagrama(int puerto)
{
    s = socket(AF_INET, SOCK_DGRAM, 0);
    bzero((char *)&direccionLocal, sizeof(direccionLocal));
    direccionLocal.sin_family = AF_INET;
    direccionLocal.sin_addr.s_addr = INADDR_ANY;
    direccionLocal.sin_port = htons(puerto);
    bind(s, (struct sockaddr *)&direccionLocal,sizeof(direccionLocal));
}

SocketDatagrama::~SocketDatagrama() // Destructor
{
    close(s);
}

int SocketDatagrama::recibe(PaqueteDatagrama & p)
{
    unsigned int addr_len = sizeof(direccionForanea);
    bzero((char *)&direccionForanea, sizeof(direccionForanea));
    int regreso = recvfrom(s,p.obtieneDatos(),p.obtieneLongitud(), 0, (struct sockaddr *) &direccionForanea, &addr_len);
    
    p.inicializaPuerto(ntohs(direccionForanea.sin_port));
    p.inicializaIp(inet_ntoa(direccionForanea.sin_addr));
    //cout << "Mensaje recibido de: " << inet_ntoa(direccionForanea.sin_addr) << ":" << ntohs(direccionForanea.sin_port) << endl;
    return regreso;
}

int SocketDatagrama::recibeTimeout(PaqueteDatagrama & p)
{
    unsigned int addr_len = sizeof(direccionForanea);
    bzero((char *)&direccionForanea, sizeof(direccionForanea));
    struct timeval tiempo, resp;    
    gettimeofday(&tiempo, NULL);
    int regreso = recvfrom(s,p.obtieneDatos(),p.obtieneLongitud(), 0, (struct sockaddr *) &direccionForanea, &addr_len);
    if (regreso < 0) {
        if (errno == EWOULDBLOCK){
            fprintf(stderr, "Tiempo para recepción transcurrido\n");
            unsetTimeout();
            return regreso;
        }else{
            fprintf(stderr, "Error en recvfrom\n");
        }
    }
    gettimeofday(&resp, NULL);
    timersub(&resp, &tiempo, &resp);
    cout << "Llego mensaje despues de " << resp.tv_sec << "." << resp.tv_usec << "s" << endl;
    p.inicializaPuerto(ntohs(direccionForanea.sin_port));
    p.inicializaIp(inet_ntoa(direccionForanea.sin_addr));
    cout << "Mensaje recibido de: " << inet_ntoa(direccionForanea.sin_addr) << ":" << ntohs(direccionForanea.sin_port) << endl;    

    return regreso;
}

int SocketDatagrama::envia(PaqueteDatagrama & p)
{
    bzero((char *)&direccionForanea, sizeof(direccionForanea));
    direccionForanea.sin_family = AF_INET;
    direccionForanea.sin_addr.s_addr = inet_addr(p.obtieneDireccion());
    direccionForanea.sin_port = htons(p.obtienePuerto());
    
    return sendto(s,p.obtieneDatos(), p.obtieneLongitud(), 0, (struct sockaddr *) &direccionForanea, sizeof(direccionForanea));
}

int SocketDatagrama::setTimeout(time_t segundos, suseconds_t microsegundos)
{
    tiempofuera.tv_sec = segundos;
    tiempofuera.tv_usec = microsegundos;
    timeout = true;
    return setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&tiempofuera, sizeof(tiempofuera));    
}

int SocketDatagrama::unsetTimeout()
{
    int y = 0;
    timeout = false;
    return setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &y, sizeof(int));    
}

int SocketDatagrama::activaBroadcast(bool opcion)
{
	int yes = opcion ? 1 : 0;
	return setsockopt(s, SOL_SOCKET, SO_BROADCAST, &yes,sizeof(int));
}

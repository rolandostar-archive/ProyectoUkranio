#include "PaqueteDatagrama.h"
#include <string.h>

PaqueteDatagrama::PaqueteDatagrama(char * msg, unsigned int length, const char * dir, int port):longitud(length),puerto(port){
    datos = new char[length];
    memcpy(datos,msg,length);
    strcpy(ip,dir);
}

PaqueteDatagrama::PaqueteDatagrama(unsigned int length):longitud(length){ datos = new char[length]; }
PaqueteDatagrama::~PaqueteDatagrama(){ delete [] datos; }
char* PaqueteDatagrama::obtieneDireccion(){ return ip; }
unsigned int PaqueteDatagrama::obtieneLongitud(){ return longitud; }
int PaqueteDatagrama::obtienePuerto(){ return puerto; }
char* PaqueteDatagrama::obtieneDatos(){ return datos; }
void PaqueteDatagrama::inicializaPuerto(int puerto){ this->puerto = puerto; }
void PaqueteDatagrama::inicializaIp(char * direccion) { strcpy(this->ip,direccion); }
void PaqueteDatagrama::inicializaDatos(char * msg){ memcpy(datos,msg,longitud); }
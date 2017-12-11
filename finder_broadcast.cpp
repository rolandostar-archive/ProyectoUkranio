#include "SocketDatagrama.h"
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace std;

vector<char*> get_servers(){
	char ip[16];
	char msg[2] = {0,0};
	
	int i;
	
	vector<char*> res;
	
	SocketDatagrama s(1000);
	//s.setTimeout(0,600000);
	PaqueteDatagrama p(8),p2(4);
	
	p.inicializaDatos((char*)msg);
	p.inicializaPuerto(7200);
	
	s.setBroadcast();
	
	sprintf(ip,"%s.255","192.168.1");
	
	p.inicializaIp(ip);
	
	s.setTimeout(5,0);
	s.envia(p);
	
	for(;;){
		if(s.recibeTimeout(p2)>0){
			printf("Servidor %s activo\n",p2.obtieneDireccion());
			
			char* aux = (char*)malloc(16);
			sprintf(aux,"%s",p2.obtieneDireccion());
			res.push_back(aux);
		}
	}
	return res;
}


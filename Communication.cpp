#include "SocketDatagrama.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

using namespace std;

struct operacion{
	int op;
	int v1;
	int v2;
	char arg[255];
	char sender[16];
};


int replyOp(SocketDatagrama s,struct operacion o){
	PaqueteDatagrama p(sizeof(struct operacion));
	
	p.inicializaDatos((char*)&o);
	
	return s.responde(p);
}

int sendOp(SocketDatagrama s,char*ip,int puerto,struct operacion o){
	
	PaqueteDatagrama p(sizeof(struct operacion));
	
	p.inicializaIp(ip);
	p.inicializaPuerto(puerto);
	p.inicializaDatos((char*)&o);
	
	s.setBroadcast();
	
	return s.envia(p);
}

int recvOp(SocketDatagrama s,struct operacion* o,int timeout){
	
	PaqueteDatagrama data(sizeof(struct operacion));
	
	int r;
	
	if(timeout){
		s.setTimeout(timeout,0);
		r = s.recibe(data);
	}else{
	
		s.unsetTimeout();
		r = s.recibeTimeout(data);
	}
	
	memcpy(o,data.obtieneDatos(),sizeof(struct operacion));
	
	memcpy(o->sender,data.obtieneDireccion(),16);
		
	return r;
}

vector<char*> get_servers(){
	char ip[16];
	char msg[2] = {0,0};
	
	int i;
	
	vector<char*> res;
	
	SocketDatagrama s(1000);
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

int main(int argc,char*argv[]){
	if(argc>1){
		cout << "Esperando operacion" << endl;
		//printf("Esperando operacion...");
		SocketDatagrama s(8081);
		struct operacion o;
		//recvOp(s,&o,5);

		PaqueteDatagrama data(sizeof(struct operacion));
	
		//int r;
		s.recibe(data);
	
		memcpy(&o,data.obtieneDatos(),sizeof(struct operacion));
	
		//memcpy(o->sender,data.obtieneDireccion(),16);
		

		printf("%d %d %d %s\n",o.op,o.v1,o.v2,o.arg);
		o.v1 = 1;

		PaqueteDatagrama send((char*)&o,sizeof(o),data.obtieneDireccion(),data.obtienePuerto());
		//replyOp(s,o);

	}else{
		struct operacion o;
		o.op = 0;
		o.v1 = 0;
		o.v2 = 0;
		SocketDatagrama s(0);
		
		//sendOp(s,"255.255.255.255",8081,o);

			PaqueteDatagrama p(sizeof(struct operacion));
	char ip[16] = "10.100.95.255";
	p.inicializaIp(ip);
	p.inicializaPuerto(8081);
	p.inicializaDatos((char*)&o);
	
	s.setBroadcast();
		s.envia(p);

		cout << "Esperando..." <<endl;
		//recvOp(s,&o,0);	
		PaqueteDatagrama data(sizeof(struct operacion));
	
		//int r;
		s.recibe(data);
	
		memcpy(&o,data.obtieneDatos(),sizeof(struct operacion));
	
		//memcpy(o->sender,data.obtieneDireccion(),16);
		

		printf("%d %d %d %s\n",o.op,o.v1,o.v2,o.arg);

		
		
	}
}

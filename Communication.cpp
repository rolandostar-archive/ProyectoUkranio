#include "SocketDatagrama.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

struct operacion{
	int op;
	int v1;
	int v2;
	char arg[255];
};

int sendOp(char*ip,int puerto,struct operacion o){
	
	PaqueteDatagrama p(sizeof(struct operacion));
	
	p.inicializaIp(ip);
	p.inicializaPuerto(puerto);
	p.inicializaDatos((char*)&o);
	
	SocketDatagrama s(0);
	
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
		printf("Esperando operacion...");
		SocketDatagrama s(8081);
		struct operacion o;
		recvOp(s,&o,0);
		printf("%d %d %d %s",o.op,o.v1,o.v2,o.arg);
	}else{
		struct operacion o;
		o.op = 1;
		o.v1 = 2;
		o.v2 = 3;
		sprintf(o.arg,"hola");
		sendOp("127.0.0.1",8081,o);	
	}
}
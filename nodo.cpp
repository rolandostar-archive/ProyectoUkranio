#include <iostream>
#include <string>
#include <vector>
#include "PaqueteDatagrama.h"
#include "SocketDatagrama.h"
#include "Searcher.h"

#include <cstring>


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



/* Flag:
	 0 - Request
	 1 - Answer
*/
void ping(int flag,char const *ip,int puerto){
	struct operacion data_ping = {
		0,  // OP Code
		flag,  // value 1
		0,  // value 2
		"HOLA MUNDO"
	};
	//char ip[16] = "127.0.0.1";
	cout << "Construyendo data_ping"<<endl;
	cout << data_ping.op << endl;
	cout << data_ping.v1 << endl;
	cout << data_ping.v2 << endl;
	cout << data_ping.arg << endl;
	cout << ip << endl;

	SocketDatagrama sock_send(0);
	sock_send.setBroadcast();

	/*SocketDatagrama sock_send(puerto);
	if(!flag) {
		sock_send.setBroadcast();
		cout << "Activando setBroadcast";
	}
	*/
	PaqueteDatagrama ping((char*)&data_ping,sizeof(data_ping),ip,puerto);
	sock_send.envia(ping);
	
}

int main(int argc, char const *argv[]) {
	int puerto = (argv[1] == NULL)?7777:atoi(argv[1]), b; // Puerto 7777 por default
	cout << "Puerto: " << puerto << endl;
	ping(0,argv[2],puerto);
	// TO-DO: Conexion para actualizar lista de nuevo nodo.

	SocketDatagrama sock_recv(puerto);
	struct operacion op_recv;
	while(1){
		PaqueteDatagrama data_recv(sizeof(struct operacion));
		cout << "Puerto: " << puerto << endl;
		
		sock_recv.recibe(data_recv);
		cout << "Recibi!" << endl;
		
		memcpy(&op_recv, data_recv.obtieneDatos(), sizeof(struct operacion));
		cout << "Mostrando op_recv"<<endl;
		cout << op_recv.op << endl;
		cout << op_recv.v1 << endl;
		cout << op_recv.v2 << endl;
		cout << op_recv.arg << endl;

		switch(op_recv.op){
			case 0: {// Ping
				cout << "Recibi Ping! OP:0" << endl;
				if(op_recv.v1 == 0) ping(1,data_recv.obtieneDireccion(),puerto);
				break;
			}
			case 1: { // Busqueda
				cout << "Recibi Busqueda! OP:1" << endl;
				createIndex();
				vector<pair<string,pair<int,int> > > found;
				string arg(op_recv.arg);
				found = searchInIndexFileByParts("INDEXFILE",arg,op_recv.v1,op_recv.v2);
				struct operacion encontrado;
				for(int i=0; i<found.size(); i++) {
					encontrado.op  = 2;
					encontrado.v1  = found[i].second.first;
					encontrado.v2  = found[i].second.second;
					memcpy(encontrado.arg,found[i].first.c_str(),found[i].first.size()+1);
					sendOp(data_recv.obtieneDireccion(),data_recv.obtienePuerto(),encontrado);
				}
				encontrado.op  = 3;
				encontrado.v1  = 0;
				encontrado.v2  = 0;
				memcpy(encontrado.arg,"END",4);
				break;
			}
			case 4: {// Descarga
				char * comando;
				sprintf(comando,"curl http://%s/text/%s",data_recv.obtieneDireccion(),op_recv.arg);
				system(comando);
				break;
			}
			/*
			default:
				cout << "Error de operacion OP:" << op_recv.op << endl;
			break;
			*/
		}

	}
	return 0;
}
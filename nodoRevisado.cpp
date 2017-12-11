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

int main(int argc, char const *argv[]){
	//int puerto = (argv[1] == NULL)?7777:atoi(argv[1]), b; // Puerto 7777 por default
	//cout << "Puerto: " << puerto << endl;
	SocketDatagrama s_send(0);
	SocketDatagrama s_recv(9444);

	s_send.activaBroadcast(true);

	struct operacion data_ping = {
		0,  // OP Code
		0,  // value 1
		0,  // value 2
		""
	};
	char broadcast[16] = "255.255.255.255";
	//char broadcast[16] = "127.0.0.1";
	PaqueteDatagrama p1((char*)&data_ping,sizeof(data_ping),broadcast,9444);
	cout << "Enviando ping inicial" << endl;
	s_send.envia(p1);
	while(1){	
		cout << "Esperando comando" << endl;
		PaqueteDatagrama p3(sizeof(struct operacion));
		s_recv.recibe(p3);
		if(strcmp(p3.obtieneDireccion(),"127.0.0.1")) {
			cout << "Saltandome" << endl;
			continue;
		}
		struct operacion op_recv;
		memcpy(&op_recv, p3.obtieneDatos(), sizeof(struct operacion));

		switch(op_recv.op){
			case 0: { // Ping
				struct operacion ping_reply = {0,1,0,""};
				PaqueteDatagrama p4((char*)&ping_reply,sizeof(struct operacion),p3.obtieneDireccion(),p3.obtienePuerto());
				if(op_recv.v1 == 0)	s_send.envia(p4);
				else{
					cout << "Recibi Reply!" << endl;
				}
				break;
			}
		}
	}

	return 0;
}
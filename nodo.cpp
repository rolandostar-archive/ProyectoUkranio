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
			case 0: // Ping
				cout << "Recibi Ping! OP:0" << endl;
				if(op_recv.v1 == 0) ping(1,data_recv.obtieneDireccion(),puerto);
			break;
			case 1: // Busqueda
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
				}
				encontrado.op  = 3;
				encontrado.v1  = 0;
				encontrado.v2  = 0;
				memcpy(encontrado.arg,"END",4);
			break;
			default:
				cout << "Error de operacion OP:" << op_recv.op << endl;
			break;
		}

	}

/*

		printf("Cliente solicito archivo: %s\n", data.obtieneDatos());
		SocketDatagrama sock_send(data.obtienePuerto());
		FILE *fp;
		// Abre archivo.
		fp=fopen(data.obtieneDatos(),"r");
		if(fp==NULL){
		  printf("Archivo no existe\n");
		  int error = 1;
		  PaqueteDatagrama err((char*)&error,sizeof(error),data.obtieneDireccion(),data.obtienePuerto());
		  sock_send.envia(err); // Informar Cliente
		  continue;
		}

		// Si si existe, encontrar tamaño
		fseek(fp,0,SEEK_END);
		size_t file_size=ftell(fp);
		rewind(fp);
		char file_buffer[512];
		// Determinamos numero de bloques.
		printf("Tamaño del archivo: %d\nNumero de Bloques: %d\n", file_size, file_size/((file_size>512)?512:file_size));
		// Leemos hasta terminar el archivo
		while(file_size != 0){
			// Cuantos bytes leemos? Si el archivo tiene mas de 512 bytes restantes, 512. De otra forma, el resto del archivo.
			int bytes = (file_size>512)?512:file_size;

		  	if(fread(file_buffer,bytes,1,fp)<=0) exit(1);
		  	PaqueteDatagrama data_archivo(file_buffer,bytes,data.obtieneDireccion(), data.obtienePuerto());
			sock_send.envia(data_archivo); // Envia Bytes.
			bzero(file_buffer,512);
			file_size -= bytes; // Actualiza tamaño del archivo.
			printf("Bytes restantes %d\n", (int)file_size);			
			PaqueteDatagrama ack(sizeof(long));
			sock_recv.recibeTimeout(ack);
			printf("Cliente recibio bloque #%d\n", b++);
			
			//long ack_block = *ack.obtieneDatos();			
			//printf("Cliente recibio %ld\n", ntohl(ack_block));
		

		}
		
	}
	*/
	return 0;
}
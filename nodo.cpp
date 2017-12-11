#include <iostream>
#include <string>
#include <vector>
#include "PaqueteDatagrama.h"
#include "SocketDatagrama.h"

using namespace std;

struct operacion{
	int op;
	int v1;
	int v2;
	string arg;
};


/* Flag:
	 0 - Request
	 1 - Answer
*/
void ping(int flag){
	struct operacion data_ping = {
		0,  // OP Code
		flag,  // value 1
		NULL,  // value 2
		NULL
	};

	SocketDatagrama sock_send(puerto);
	sock_send.setBroadcast();
	PaqueteDatagrama ping((char*)data_ping,sizeof(data_ping),argv[1],puerto);
	sock_send.envia(ping);
}

int main(int argc, char const *argv[]) {
	int puerto = (argv[2] == NULL)?7777:atoi(argv[2]), b; // Puerto 7777 por default
	ping(0);
	// TO-DO: Conexion para actualizar lista de nuevo nodo.

	while(1){
		SocketDatagrama sock_recv(puerto);
		PaqueteDatagrama data_recv(sizeof(struct operacion));
		//sock_recv.setTimeout(3,0);
		printf("Esperando recibir comando...\n");
		sock_recv.recibe(data_recv);
		struct operacion data_recv = (struct operacion)data_recv.obtieneDatos();
		switch(){
			case 0: // Ping
				if(data_recv.v1 == 0) ping(1);
			break;
			case 1: // Busqueda
			break;
			case default:

			break;
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
		*/
	}
	return 0;
}
#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include "PaqueteDatagrama.h"
#include "SocketDatagrama.h"


struct operacion{
	int op;
	int v1;
	int v2;
	char arg[255];
};

using namespace std;

int main(int argc, char const *argv[]){
	struct operacion data_ping = {
		0,  // OP Code
		1,  // value 1
		0,  // value 2
		"HOLA MUNDO"
	};

	//strcpy(data_ping.arg,"HOLA MUNDO");
	SocketDatagrama sock(0);
	char ip[16] = "127.0.0.1";
	PaqueteDatagrama data((char*)&data_ping,sizeof(data_ping),ip,7777);
	sock.envia(data);
	return 0;
}
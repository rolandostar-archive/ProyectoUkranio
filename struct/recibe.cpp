#include <iostream>
#include <vector>
#include "PaqueteDatagrama.h"
#include "SocketDatagrama.h"

#include <cstring>


struct operacion{
	int op;
	int v1;
	int v2;
	char arg[20];
};

using namespace std;

int main(int argc, char const *argv[]){
	SocketDatagrama sock(7777);
	PaqueteDatagrama data(sizeof(struct operacion));
	sock.recibe(data);
	struct operacion op_recv;
	memcpy(&op_recv, data.obtieneDatos(), sizeof(struct operacion));

	cout << "Mostrando op_recv"<<endl;
	cout << op_recv.op << endl;
	cout << op_recv.v1 << endl;
	cout << op_recv.v2 << endl;
	cout << op_recv.arg << endl;
	return 0;
}
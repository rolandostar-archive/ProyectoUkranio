#define MG_ENABLE_HTTP_STREAMING_MULTIPART 1
#include "mongoose.h"
#include "../SocketDatagrama.h"
#include <iostream>
#include <vector>

using namespace std;

struct operacion{
	int op;
	int v1;
	int v2;
	char arg[255];
};

static const char *s_http_port = "8000";
static struct mg_serve_http_opts s_http_server_opts;

char servers[16][255];
int n_servers = 0;

struct mg_str cb(struct mg_connection *c, struct mg_str file_name) {
	char* f_name = (char*)malloc(file_name.len+5);

	sprintf(f_name,"www/text/%s",file_name.p);

	file_name.len += 5;
	file_name.p = f_name;

	return file_name;
}
SocketDatagrama s_send(0);
SocketDatagrama s_recv(9444);

void discover(){

	s_send.activaBroadcast(true);

	struct operacion data_ping = {
		0,  // OP Code
		0,  // value 1
		0,  // value 2
		""
	};
	char broadcast[16] = "255.255.255.255";
	PaqueteDatagrama p1((char*)&data_ping,sizeof(data_ping),broadcast,9444);
	cout << "Enviando ping inicial" << endl;
	s_send.envia(p1);
	s_recv.setTimeout(5,0);
	n_servers = 0;
	while(1){
		cout << "Esperando respuestas..." << endl;
		PaqueteDatagrama p3(sizeof(struct operacion));
		int r;
		if((r=s_recv.recibe(p3))<=0){
			break;
		}
		printf("codigo %d\n",r);
		if(!strcmp(p3.obtieneDireccion(),"127.0.0.1")) {
			cout << "Saltandome" << endl;
			continue;
		}
		struct operacion op_recv;
		memcpy(&op_recv, p3.obtieneDatos(), sizeof(struct operacion));

		printf("%d %d %s\n",op_recv.op,op_recv.v1,p3.obtieneDireccion());
		if(op_recv.v1==1){
			memcpy(servers[n_servers++],p3.obtieneDireccion(),16);
		}
	}
	printf("%d servidores encontrados\n",n_servers);
}

void send_queries(char* query){
	int i;
	struct operacion op = {
		1,
		0,
		n_servers,
		""
	};
	memcpy(op.arg,query,strlen(query)+1);
	for(i=0;i<n_servers;i++){
		op.v1 = i+1;
		PaqueteDatagrama p1((char*)&op,sizeof(op),servers[i],9444);
		cout << "Enviando query a " << servers[i] << endl;
		s_send.envia(p1);
	}
}

void send_files(char*file){
	int i;
	struct operacion op = {
		1,
		0,
		n_servers,
		""
	};
	memcpy(op.arg,file,strlen(file)+1);
	for(i=0;i<n_servers;i++){
		op.v1 = i+1;
		PaqueteDatagrama p1((char*)&op,sizeof(op),servers[i],9444);
		cout << "Enviando query a " << servers[i] << endl;
		s_send.envia(p1);
	}
}

void get_search(){
	int done = 0;
	while(1){
		cout << "Esperando respuestas..." << endl;
		PaqueteDatagrama p3(sizeof(struct operacion));
		if(s_recv.recibe(p3)<=0){
			return;
		}
		if(!strcmp(p3.obtieneDireccion(),"127.0.0.1")) {
			cout << "Saltandome" << endl;
			continue;
		}
		struct operacion op_recv;
		memcpy(&op_recv, p3.obtieneDatos(), sizeof(struct operacion));

		printf("%d %d %s\n",op_recv.op,op_recv.v1,p3.obtieneDireccion());
		if(op_recv.v1==3){
			done++;
		}else if(op_recv.v1==2){
			//printf();
		}
		if(done==n_servers){
			break;
		}
	}
}


static void handle_search(struct mg_connection *nc, struct http_message *hm) {
		char query[256];
		// Get form variables and store settings values
		mg_get_http_var(&hm->body, "query", query,sizeof(query));
		mg_send_head(nc,200,strlen(query), "Content-Type: text/plain");
		mg_printf(nc, "%s", query);
		printf("query: %s\n",query);
		discover();
		send_queries(query);
		get_search();
}

static void ev_handler(struct mg_connection *nc, int ev, void *p) {
 	struct http_message *hm = (struct http_message *) p;
	if (ev == MG_EV_HTTP_REQUEST) {
		if (mg_vcmp(&hm->uri, "/search") == 0) {
		    handle_search(nc, hm);
		}else{
			mg_serve_http(nc, (struct http_message *) p, s_http_server_opts);
		}
	}
	if(ev == MG_EV_HTTP_PART_BEGIN||ev == MG_EV_HTTP_PART_DATA||ev == MG_EV_HTTP_PART_END){
		if(ev==MG_EV_HTTP_PART_END){
    		mg_http_send_redirect(nc, 302, mg_mk_str("/"), mg_mk_str(NULL));
    		struct mg_http_multipart_part *mp =
          (struct mg_http_multipart_part *) p;
    		printf("Archivo subido: %s\n",mp->file_name);
    	}
    	mg_file_upload_handler(nc, ev, p, cb);

	}


}
int main(void) {
	struct mg_mgr mgr;
	struct mg_connection *nc;
	mg_mgr_init(&mgr, NULL);
	printf("Starting web server on port %s\n", s_http_port);
	nc = mg_bind(&mgr, s_http_port, ev_handler);
	if (nc == NULL) {
		printf("Failed to create listener\n");
		return 1;
	}
	// Set up HTTP server parameters
	mg_set_protocol_http_websocket(nc);
	s_http_server_opts.document_root = "www"; // Serve current directory
	s_http_server_opts.enable_directory_listing = "yes";
	for (;;) {
		mg_mgr_poll(&mgr, 1000);
	}
	mg_mgr_free(&mgr);
	return 0;
}

#define MG_ENABLE_HTTP_STREAMING_MULTIPART 1
#include "mongoose.h"

static const char *s_http_port = "8000";
static struct mg_serve_http_opts s_http_server_opts;

char*servers;

struct mg_str cb(struct mg_connection *c, struct mg_str file_name) {

	char* f_name = (char*)malloc(file_name.len+5);

	sprintf(f_name,"text/%s",file_name.p);
	
	file_name.len += 5;
	file_name.p = f_name;
	
	return file_name;
}

static void handle_search(struct mg_connection *nc, struct http_message *hm) {
		char query[256];
		// Get form variables and store settings values
		mg_get_http_var(&hm->body, "query", query,sizeof(query));
		mg_send_head(nc,200,strlen(query), "Content-Type: text/plain");
		mg_printf(nc, "%s", query);
		printf("query: %s\n",query);
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

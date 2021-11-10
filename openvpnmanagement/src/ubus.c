#include "main.h"
#include "socket.h"
#include "uci_init.h"

#define NAMEMAX 256
static struct ubus_context *ubus_ctx;
static struct ubus_subscriber test_event;
static struct blob_buf b;


enum {
	SERVER_VALUE,
	__SERVER_MAX
};

static const struct blobmsg_policy server_policy[] = {
	[SERVER_VALUE] = { .name = "name", .type = BLOBMSG_TYPE_STRING },
};

static int server_status(struct ubus_context *ubus_ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	void *tbl;
	struct blob_buf b = {};
	char *messageptr = NULL;
	char *ptr = NULL;
	int rotations = 0;

	blob_buf_init(&b, 0);
	
	messageptr = socket_message(sockfd, "status");

	tbl = blobmsg_open_array(&b, "Clients");
	
	ptr = strtok(messageptr, "\n\r");
	while (ptr != NULL){
        if(rotations < 3){
            rotations++;
            ptr = strtok(NULL, "\n\r");
            continue;
        }
        if(strcmp(ptr, "ROUTING TABLE") == 0){
            break;
        }
		blobmsg_add_string(&b, NULL, ptr);
        ptr = strtok(NULL, "\n\r");
	}
	
	blobmsg_close_table(&b, tbl);

	ubus_send_reply(ubus_ctx, req, b.head);
	blob_buf_free(&b);
	free(messageptr);
	
	return 0;
}
static int server_kill(struct ubus_context *ubus_ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__SERVER_MAX];
	struct blob_buf b = {};
	char *command_blob = NULL;
	char *messageptr = NULL;
	char command[NAMEMAX];
	blobmsg_parse(server_policy, __SERVER_MAX, tb, blob_data(msg), blob_len(msg));
	
	if (!tb[SERVER_VALUE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	
	command_blob = blobmsg_get_string(tb[SERVER_VALUE]);
	
	snprintf(command,NAMEMAX,"kill %s",command_blob);
	messageptr = socket_message(sockfd, command);
	
	blob_buf_init(&b, 0);

	blobmsg_add_string(&b, "Message", messageptr);
	ubus_send_reply(ubus_ctx, req, b.head);
	
	blob_buf_free(&b);
	free(messageptr);
	
	
	return 0;
}

static const struct ubus_method server_methods[] = {
	UBUS_METHOD_NOARG("status", server_status),
	UBUS_METHOD("kill",server_kill,server_policy)
};

 struct ubus_object_type server_object_type =
	UBUS_OBJECT_TYPE("Server", server_methods);

static struct ubus_object server_object = {
	.name = "",
	.type = &server_object_type,
	.methods = server_methods,
	.n_methods = ARRAY_SIZE(server_methods)
};

void ubus_register_server(struct uci_package *package)
{
	int ret;
	char *ServerName = NULL;
	char object_name[NAMEMAX];

	ret = uci_parseServerName(package, &ServerName);
	if((ret > 0)){
		syslog(LOG_ERR, "Failed to parse server name from uci");
		cleanup(1);
	}

	snprintf(object_name, NAMEMAX,"openvpn.%s",  ServerName); 
	server_object.name=object_name;

	ret = ubus_add_object(ubus_ctx, &server_object);
	if (ret) {
		syslog(LOG_ERR,"Failed to add server object: %s\n", ubus_strerror(ret));
		cleanup(1);
	}
    ubus_add_uloop(ubus_ctx);
}

void ubus_init(void)
{
	uloop_init();
	ubus_ctx = ubus_connect(NULL);
	if (!ubus_ctx) {
		syslog(LOG_ERR,"Failed to connect to ubus");
		cleanup(1);
	}
}

void ubus_finish(void)
{
	if (ubus_ctx == NULL) {
		return;
	}
	ubus_remove_object(ubus_ctx, &server_object);
	ubus_free(ubus_ctx);
	ubus_ctx = NULL;
}
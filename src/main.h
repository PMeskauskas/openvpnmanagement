#include <libubox/runqueue.h>
#include <libubox/uloop.h>
#include <libubox/blobmsg_json.h>
#include <libubus.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <argp.h>
#include <syslog.h>
#include <uci.h>
#define CONFIG "openvpn"

struct arguments 
{
	char *args[2];
	char *port, *remoteAddress;
};

int sockfd;
struct arguments arguments;

void cleanup(int sig);
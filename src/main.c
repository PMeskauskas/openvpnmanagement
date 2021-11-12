#include "main.h"
#include "uci_init.h"
#include "ubus.h"

volatile int interrupt = 0;
const char *argp_program_version = "openvpnmanagement 1.0.0";
const char *argp_program_bug_address = "<TavoDraugas154@one.lt>";
static char args_doc[] = "-r remote_address -p port";
static char doc[] = "openvpnmanagement -- A program to interract with an openvpn server";
struct uci_context *uci_ctx = NULL;
struct uci_package *package = NULL;

static struct argp_option options[] = {
	{"remote_address", 'r', "ADDRESS", 0, "Specify the address"},
	{"port", 'p', "PORT", 0, "Specify the port"},
	{0}};

void signal_handler(int signo)
{
	signal(SIGINT, NULL);
	syslog(LOG_INFO, "Received signal: %d", signo);
	interrupt = 1;
}

void cleanup(int signo)
{
	syslog(LOG_INFO, "Closing program...");
	closelog();
	ubus_finish();
	uci_finish(uci_ctx);
	close(sockfd);
	exit(signo);
}
static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
	struct arguments *arguments = state->input;
	switch (key){
	case 'p':
		arguments->port = arg;
		break;

	case 'r':
		arguments->remote_address = arg;
		break;

	case ARGP_KEY_ARG:
		arguments->args[state->arg_num] = arg;
		break;
	case ARGP_KEY_END:
		if ((!arguments->remote_address) || (!arguments->port)){
			argp_usage(state);
		}
		break;
	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};
int main(int argc, char *argv[])
{
	openlog(NULL, LOG_CONS, LOG_USER);
	arguments.remote_address = NULL;
	arguments.port = NULL;
	argp_parse(&argp, argc, argv, 0, 0, &arguments);
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	
	uci_init(uci_ctx, CONFIG, &package);

	ubus_init();
	ubus_register_server(package);

	sockfd = socket_init(arguments.remote_address, arguments.port);

	uloop_run();
	uloop_done();
	
	cleanup(0);

	return 0;
}

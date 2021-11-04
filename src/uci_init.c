#include "main.h"

void uci_element_parseCheckOption(struct uci_option *option, char* type, char** value)
{
	if(strcmp(option->e.name, type) == 0)
		*value=option->v.string;
}

void uci_load_package(struct uci_context *ctx, const char *config_name, struct uci_package **package)
{
	if(uci_load(ctx, config_name, package) !=UCI_OK){
		syslog(LOG_ERR, "Failed to load uci");
		uci_free_context(ctx);
		cleanup(1);
	}
}

void uci_init(struct uci_context *ctx, char* configName, struct uci_package **package)
{
	ctx = uci_alloc_context();
	if(ctx == NULL){
		syslog(LOG_ERR, "Failed to allocate memory to uci context");
		cleanup(1);
	}
	uci_load_package(ctx, CONFIG, package);
}

int uci_parseServerName(struct uci_package *package, char **ServerName)
{
	struct uci_element *i, *j;
	uci_foreach_element(&package->sections, i){
		char *port = NULL;
		struct uci_section *section = uci_to_section(i);
		char *section_name = section->e.name;
		if(strcmp(section_name,"Server") == 0){
			uci_foreach_element(&section->options, j){
			struct uci_option *option = uci_to_option(j);
			uci_element_parseCheckOption(option,"_name", ServerName);
			uci_element_parseCheckOption(option,"port", &port);
		}
		if(strcmp(port, arguments.port) == 0)
			return 0;
		}
	}
    return -1;
}
void uci_finish(struct uci_context *ctx)
{
    if (ctx == NULL) {
		return;
	}
	uci_free_context(ctx);
	ctx = NULL;
}
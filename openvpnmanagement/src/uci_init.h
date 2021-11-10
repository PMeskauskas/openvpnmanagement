void uci_init(struct uci_context *ctx, char* configName, struct uci_package **package);
int uci_parseServerName(struct uci_package *package, char **ServerName);
void uci_finish(struct uci_context *ctx);
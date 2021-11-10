#include "main.h"

#define MAX 1024

int socket_create(int *sockfd)
{
    *sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (*sockfd == -1){
        syslog(LOG_ERR,"socket creation failed...\n");
        cleanup(1);
    }
    syslog(LOG_INFO,"Socket successfully created..\n");
}
void socket_connect(int *sockfd, struct sockaddr_in servaddr, char *remoteAddress, char *port)
{
    long portNum;
    char *portPtr;

    if ((remoteAddress == NULL) && (port == NULL)){
        syslog(LOG_ERR, "Must specify port and address...");
        cleanup(1);
    }

    portNum = strtol(port, &portPtr, 10);
    if ((portPtr == port)){
        syslog(LOG_ERR, "Port must be an integer...");
        cleanup(1);
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(remoteAddress);
    servaddr.sin_port = htons(portNum);

    if (connect(*sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0){
        syslog(LOG_ERR, "connection with the server failed...\n");
        cleanup(1);
    }
    else
        syslog(LOG_INFO, "connected to the server..\n");
}

char *socket_message(int sockfd, char command[])
{
    char buff[MAX];
    char *buff2 = (char*)malloc(sizeof(char)*MAX);

    strncpy(buff, command, MAX);
    strcat(buff, "\n");

    write(sockfd, buff, sizeof(buff));
    bzero(buff, sizeof(buff));
    
    read(sockfd, buff, sizeof(buff));
    strncpy(buff2, buff, MAX);

    return buff2;
}
int socket_init(char *remoteAddress, char *port)
{
    int sockfd;
    char buff[MAX];
	struct sockaddr_in servaddr;

    socket_create(&sockfd);
    socket_connect(&sockfd, servaddr, remoteAddress, port);
    read(sockfd, buff, sizeof(buff));
    bzero(buff, sizeof(buff));
    return sockfd;
}
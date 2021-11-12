#include "main.h"
#define CHUNK_MAX 1024
#define TIMEOUT 5

int socket_create(int *sockfd)
{
    *sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (*sockfd == -1){
        syslog(LOG_ERR, "socket creation failed...\n");
        cleanup(1);
    }
    syslog(LOG_INFO, "Socket successfully created..\n");
}
void socket_try_connect_to_server(int *sockfd, struct sockaddr_in servaddr){
    int rc;
    int count = 0;
    while(1) {
        rc = connect(*sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
        if (rc != 0){
            count++;
            if(count == 5){
                syslog(LOG_ERR, "Failed to connect to server 5 times, aborting...");
                cleanup(1);
            }
            syslog(LOG_INFO, "Connection to server failed %d times, retrying...\n", count);
            sleep(1);
        }
        else {
            syslog(LOG_INFO, "connected to the server..\n");
            break;
        }
    }
}
void socket_connect(int *sockfd, struct sockaddr_in servaddr, char *remote_address, char *port)
{
    long portNum;
    char *portPtr;
    int rc = 0;
    int connections = 0;
    if ((remote_address == NULL) && (port == NULL)){
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
    servaddr.sin_addr.s_addr = inet_addr(remote_address);
    servaddr.sin_port = htons(portNum);

    socket_try_connect_to_server(sockfd, servaddr);
    
}

char *socket_message(int sockfd, char command[])
{
    char buff[CHUNK_MAX];
    char *buff2 = (char *)malloc(sizeof(char) * CHUNK_MAX);
    int ret;
    int bytes;
    struct pollfd fds[2];
    bzero(buff, CHUNK_MAX);
    

    while (1){
        if ((ret = recv(sockfd, buff, CHUNK_MAX, 0)) < 0){
            break;
        }
        bzero(buff, CHUNK_MAX);
    }

    bzero(buff, CHUNK_MAX);
    strncpy(buff, command, CHUNK_MAX - 2);
    strcat(buff, "\n");

    write(sockfd, buff, sizeof(buff));
    bzero(buff, CHUNK_MAX);
    while (1){
        fds[0].fd = sockfd;
        fds[0].events = POLLIN;

        ret = poll(fds, 2, 0);
        if (ret == 0)
            syslog(LOG_ERR, "Socket closed");
        if (fds[0].revents & POLLIN){
            recv(sockfd, buff, CHUNK_MAX, 0);
            strncpy(buff2, buff, CHUNK_MAX);
            return buff2;
        }
    }
}
int socket_init(char *remote_address, char *port)
{
    int sockfd;
    char buff[CHUNK_MAX];
    struct sockaddr_in servaddr;

    socket_create(&sockfd);
    socket_connect(&sockfd, servaddr, remote_address, port);
    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    return sockfd;
}
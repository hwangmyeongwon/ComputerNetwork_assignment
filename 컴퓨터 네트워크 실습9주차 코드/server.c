#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_FILE "./sock_addr"
#define BUFFER_SIZE 1024

void error(char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    int sockfd, newsockfd, clilen;
    struct sockaddr_un serv_addr, cli_addr;
    char buffer[BUFFER_SIZE];
    int n;

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strncpy(serv_addr.sun_path, SOCKET_FILE, sizeof(serv_addr.sun_path) - 1);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    listen(sockfd, 5);

    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");

    while (1) {
        bzero(buffer, BUFFER_SIZE);
        n = read(newsockfd, buffer, BUFFER_SIZE - 1);
        if (n < 0)
            error("ERROR reading from socket");

        printf("Client: %s", buffer);

        if (strncmp(buffer, "\\quit", 5) == 0)
            break;

        bzero(buffer, BUFFER_SIZE);
        printf("Server: ");
        fgets(buffer, BUFFER_SIZE - 1, stdin);
        n = write(newsockfd, buffer, strlen(buffer));
        if (n < 0)
            error("ERROR writing to socket");

        if (strncmp(buffer, "\\quit", 5) == 0)
            break;
    }

    close(newsockfd);
    close(sockfd);
    unlink(SOCKET_FILE);
    return 0;
}

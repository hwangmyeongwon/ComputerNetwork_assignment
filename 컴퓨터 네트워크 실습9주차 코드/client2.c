#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFSIZE 100

void error_handling(char *message);

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in serv_addr;
    char message[BUFSIZE];
    int str_len, recv_len, recv_cnt;

    if (argc != 3) {
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error");

    printf("Connected to server...\n");

    while (1)
            {
                printf("Input message (\\quit): ");
                fgets(message, BUFSIZE, stdin); 

                if (send(sock, message, strlen(message), 0) == -1)
                    error_handling("send() error");
                memset(message, 0, BUFSIZE);

                str_len = recv(sock, message, BUFSIZE, 0);
                printf("[You] %s", message);

                if (strncmp(message, "\\quit", 5) == 0)
                {
                    printf("[Client] disconnected\n");
                    break;
                }
                memset(message, 0, BUFSIZE);

            }

    close(sock);

    return 0;
}

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// UNIX 및 nonblocking
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "ifdef.h"

#define BUF_SIZE 1024

void error_handling(char *message);

int main(int argc, char *argv[])
{
    /*      UNIX 부분       */
    int sfd, cfd;
    struct sockaddr_un my_addr, peer_addr;
    socklen_t peer_addr_size;

    char message_unix[BUF_SIZE]; // UNIX용

    // 소켓 생성
    sfd = socket(AF_UNIX, SOCK_STREAM, 0);

    // 소켓 주소 설정
    memset(&my_addr, 0, sizeof(struct sockaddr_un));
    my_addr.sun_family = AF_UNIX;
    strncpy(my_addr.sun_path, SOCK_PATH, sizeof(my_addr.sun_path) - 1);

    // 소켓 주소에 bind
    bind(sfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr_un));

    // 연결 요청 대기 (얘를 추가해서 확인해보는중)
    if (listen(sfd, 5) == -1)
        error_handling("listen() error");

    // 클라이언트 연결 수락
    peer_addr_size = sizeof(struct sockaddr_un);
    cfd = accept(sfd, (struct sockaddr *)&peer_addr, &peer_addr_size);

    /*      UNIX 부분 끝        */

    /*      INET 부분         */

    int sock;
    char message[BUF_SIZE];

    int str_len;
    struct sockaddr_in serv_adr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_adr.sin_port = htons(8013);

    if (connect(sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("connect() error");

    // 위에까진 서버와 연결코드

    // int flags = fcntl(sock, F_GETFL, 0);
    // fcntl(sock, F_SETFL, flags | O_NONBLOCK);
    int flags_unix = fcntl(sfd, F_GETFL, 0);
    fcntl(sfd, F_SETFL, flags_unix | O_NONBLOCK);
    // ssize_t bytes_received = recv(cfd, message_unix, sizeof(message_unix), 0);
    // printf("[황명원]: %s\n", message_unix);

    while (1)
    {
        // memset(message, 0, BUF_SIZE);
        str_len = recv(sock, message, BUF_SIZE, 0);
        printf("%s", message);

        // memset(message_unix, 0, BUF_SIZE);
        ssize_t bytes_received = recv(cfd, message_unix, sizeof(message_unix), 0);
        if (bytes_received > 0)
        {
            send(sock, message_unix, BUF_SIZE, 0); // send to server.c
        }

        // 사용자 입력이 1인 경우 : info 받기
        if (strncmp(message_unix, "1", 1) == 0)
        {
            memset(message_unix, 0, BUF_SIZE);
            memset(message, 0, BUF_SIZE);
            str_len = recv(sock, message, BUF_SIZE, 0);
            printf("%s\n", message);
            // memset(message, 0, BUF_SIZE);
            continue;
        }

        // 사용자 입력이 2인 경우 :채팅방 입장 및 채팅시작
        else if (strncmp(message_unix, "2", 1) == 0)
        {
            
            while (1)
            {
                ssize_t bytes_received = recv(cfd, message_unix, sizeof(message_unix), 0);

                send(sock, message_unix, BUF_SIZE, 0);
                if (strncmp(message_unix, "quit", 4) == 0)
                    break;
            }

            continue; // GoBack or Download done...
        }

        else if (strncmp(message_unix, "3", 1) == 0)
        {
            break;
        }
    }
    close(sock);
    close(sfd);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define SOCK_PATH "/tmp/socket1"
#define PORT 8082

void error_handling(char *message);

int main()
{
    /*      UNIX 부분       */
    int sfd, cfd;
    struct sockaddr_un my_addr, peer_addr;
    socklen_t peer_addr_size;

    char buff[1024];
    char main_buff[1024];

    // 소켓 생성
    sfd = socket(AF_UNIX, SOCK_STREAM, 0);

    // 소켓 주소 설정
    memset(&my_addr, 0, sizeof(struct sockaddr_un));
    my_addr.sun_family = AF_UNIX;
    strncpy(my_addr.sun_path, SOCK_PATH, sizeof(my_addr.sun_path) - 1);

    // 소켓 주소에 bind
    bind(sfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr_un));

    // 연결 요청 대기
    listen(sfd, 5);

    // 클라이언트 연결 수락
    peer_addr_size = sizeof(struct sockaddr_un);
    cfd = accept(sfd, (struct sockaddr *)&peer_addr, &peer_addr_size);

    /*      UNIX 부분 끝        */

    /*      INET 부분       */
    int serv_sock;
    int clnt_sock;
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;

    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(PORT);

    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error");

    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    clnt_addr_size = sizeof(clnt_addr);

    clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
    if (clnt_sock == -1)
        error_handling("accept() error");

    /*      INET 부분 끝         */

    int flags = fcntl(clnt_sock, F_GETFL, 0);
    fcntl(clnt_sock, F_SETFL, flags | O_NONBLOCK);

    // 클라이언트와 통신
    while (1)
    {
        ssize_t bytes_received = recv(cfd, buff, sizeof(buff), 0);

        while (1)
        {
            ssize_t bytes_received2 = recv(clnt_sock, main_buff, sizeof(main_buff), 0);
            if (bytes_received2 > 0)
            {
                printf("[최승호]: %s", main_buff);
            }
            else{
                break;
            }
        }

        if (bytes_received > 0)
        {
            printf("[황명원]: %s", buff);
            send(clnt_sock, buff, sizeof(buff), 0); // send to 3-c-main
        }

        if (bytes_received == 0)
        {
            printf("client disconnected...\n");
            break;
        }
        else if (bytes_received < 0)
        {
            printf("recv failed...\n");
            break;
        }
        memset(buff, 0, sizeof(buff));
        memset(main_buff, 0, sizeof(main_buff));
    }

    // 연결 종료
    close(cfd);
    close(sfd);

    // 소켓 파일 삭제
    unlink(SOCK_PATH);

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
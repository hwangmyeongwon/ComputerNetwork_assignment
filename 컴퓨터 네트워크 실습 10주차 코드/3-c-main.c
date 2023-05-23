#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define SOCK_PATH "/tmp/socket2"
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
    int sock;
    struct sockaddr_in serv_adr;

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_adr.sin_port = htons(PORT);

    if (connect(sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("connect() error");

    /*      INET 부분 끝         */

    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);

    // 클라이언트와 통신
    while (1)
    {
        ssize_t bytes_received = recv(cfd, buff, sizeof(buff), 0);

        while (1)
        {
            ssize_t bytes_received2 = recv(sock, main_buff, sizeof(main_buff), 0);

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
            send(sock, buff, sizeof(buff), 0); // send to 1-s-main
        }

        if (bytes_received == 0)
        {
            printf("client disconnected...\n");
            break;
        }
        else if (bytes_received < 0)
        {
            printf("recv failed...\n");
            // break;
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH "./sock_addr"

int main() {
    int sockfd;
    struct sockaddr_un server_addr;
    char message[1024];
    int len;

    // 소켓 생성
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    // 소켓 주소 설정
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCK_PATH);

    // 서버에 연결 요청
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect() failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("Enter message: ");
        fgets(message, sizeof(message), stdin);

        // 입력받은 메시지에 \quit 이 포함되어 있으면 종료
        if (strstr(message, "\\quit") != NULL) {
            printf("Quit the program.\n");
            break;
        }

        // 서버로 메시지 전송
        len = write(sockfd, message, strlen(message));
        if (len < 0) {
            perror("write() failed");
            exit(EXIT_FAILURE);
        }
    }

    // 소켓 닫기
    close(sockfd);

    return 0;
}

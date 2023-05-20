#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1" // 서버 IP 주소
#define SERVER_PORT 8080 // 서버 포트번호
#define BUFFER_SIZE 1024 // 버퍼 크기

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // 소켓 생성
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 설정
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    
    if (inet_pton(AF_INET, SERVER_IP, &(server_addr.sin_addr)) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    // 서버에 연결
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // 메시지 입력
        memset(buffer, 0, BUFFER_SIZE);
        printf("Enter: ");
        fgets(buffer, BUFFER_SIZE, stdin);

        // 서버로 메시지 전송
        if (send(sock, buffer, strlen(buffer)+1, 0) == -1) {
            perror("Send error");
            break;
        }

        // 종료 메시지 입력 시 클라이언트 종료
        if (strncmp(buffer, "quit", 4) == 0) {
            break;
        }
    }

    // 소켓 종료
    close(sock);

    return 0;
}

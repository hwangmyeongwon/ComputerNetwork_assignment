#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 8888
#define BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int recv_size;

    // 소켓 생성
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 설정
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    
    if (inet_pton(AF_INET, SERVER_ADDRESS, &(server_addr.sin_addr)) <= 0) {
        perror("Invalid address or address not supported");
        exit(EXIT_FAILURE);
    }

    // 서버에 연결
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    

    // 메시지 입력 및 전송
    while (1) {
        printf("Enter messages:");
        fgets(buffer, BUFFER_SIZE, stdin);
        
        // 입력한 메시지를 서버로 전송
        send(client_socket, buffer, strlen(buffer), 0);

        // 서버로부터 받은 메시지 출력
        recv_size = recv(client_socket, buffer, BUFFER_SIZE, 0);
        buffer[recv_size] = '\0';
        printf("Recv from server: %s", buffer);
        
        // "quit" 메시지를 입력하면 연결 종료
        if (strncmp(buffer, "quit", 4) == 0)
            break;
    }

    // 클라이언트 소켓 닫기
    close(client_socket);

    return 0;
}

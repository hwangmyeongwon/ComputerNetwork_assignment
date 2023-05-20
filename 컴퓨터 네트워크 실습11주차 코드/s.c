#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    int recv_size;

    // 클라이언트와의 통신 처리
    while ((recv_size = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        // 클라이언트로부터 받은 메시지 출력
        buffer[recv_size] = '\0';
        printf("Recv from client: %s", buffer);
        
        // 클라이언트로 받은 메시지를 그대로 돌려줌
        send(client_socket, buffer, recv_size, 0);
        
        // 클라이언트가 "quit" 메시지를 보낸 경우 연결 종료
        if (strncmp(buffer, "quit", 4) == 0)
            break;
    }

    // 클라이언트와의 연결 종료
    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    int client_cnt = 0;

    // 소켓 생성
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 설정
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8888);

    // 소켓에 서버 주소 할당
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    // 소켓을 수동 대기 모드로 설정
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }

    printf("Server started. Waiting for connections...\n");

    // 클라이언트 연결을 처리하는 부분
    while (1) {
        // 클라이언트 요청 대기
        client_len = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket == -1) {
            perror("Accepting connection failed");
            exit(EXIT_FAILURE);
        }

        // 접속한 클라이언트 수 증가 및 출력
        client_cnt++;
        printf("New Client! \n");
        printf("Number of service client: %d\n", client_cnt);

        // 자식 프로세스 생성하여 클라이언트 요청 처리
        pid_t pid = fork();
        if (pid == -1) {
            perror("Forking failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // 자식 프로세스는 클라이언트 요청 처리 후 종료
            close(server_socket);
            handle_client(client_socket);
            printf("Client disconnected.\n");
            exit(EXIT_SUCCESS);
        } else {
            // 부모 프로세스는 클라이언트 소켓 닫기
            close(client_socket);
        }
    }

    // 서버 소켓 닫기
    close(server_socket);

    return 0;
}

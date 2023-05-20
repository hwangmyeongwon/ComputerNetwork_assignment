#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT 8080 // 서버 포트번호
#define MAX_CLIENTS 3 // 최대 클라이언트 수
#define BUFFER_SIZE 1024 // 버퍼 크기

int main() {
    int access_sock, comm_sock[MAX_CLIENTS];
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    int client_cnt = 0;
    fd_set readfds;
    int max_sock, activity, i, ret;
    char buffer[BUFFER_SIZE];

    // 소켓 생성
    access_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (access_sock == -1) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 설정
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    // 소켓 바인딩
    if (bind(access_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding error");
        exit(EXIT_FAILURE);
    }

    // 소켓 수신 대기
    if (listen(access_sock, MAX_CLIENTS) < 0) {
        perror("Listen error");
        exit(EXIT_FAILURE);
    }


    while (1) {
        // 클라이언트 접속 허용
        if ((comm_sock[client_cnt] = accept(access_sock, (struct sockaddr*)&client_addr, &client_len)) < 0) {
            perror("Accept error");
            exit(EXIT_FAILURE);
        }

        // 접속한 클라이언트 정보 출력
        printf("Client %d connected\n", client_cnt + 1);
        client_cnt++;

        // 모든 클라이언트가 접속했으면 대기 종료
        if (client_cnt == MAX_CLIENTS) {
            break;
        }
    }

    while (1) {
        // 파일 디스크립터 설정
        FD_ZERO(&readfds);
        FD_SET(access_sock, &readfds);
        max_sock = access_sock;

        for (i = 0; i < MAX_CLIENTS; i++) {
            int sd = comm_sock[i];
            if (sd > 0) {
                FD_SET(sd, &readfds);
            }
            if (sd > max_sock) {
                max_sock = sd;
            }
        }

        // 대기 상태
        printf("Waiting at select...\n");
        activity = select(max_sock + 1, &readfds, NULL, NULL, NULL);
        if (activity == -1) {
            perror("Select error");
            exit(EXIT_FAILURE);
        }

        // 클라이언트로부터 메시지 수신
        for (i = 0; i < MAX_CLIENTS; i++) {
            int sd = comm_sock[i];
            if (FD_ISSET(sd, &readfds)) {
                memset(buffer, 0, BUFFER_SIZE);
                ret = recv(sd, buffer, BUFFER_SIZE, 0);
                if (ret == -1) {
                    perror("Recv error");
                    exit(EXIT_FAILURE);
                }
                else if (ret == 0) {
                    // 클라이언트 연결 종료
                    printf("Client %d disconnected\n", i + 1);
                    close(sd);
                    comm_sock[i] = 0;
                }
                else {
                    // 클라이언트로부터 수신한 메시지 출력
                    printf("select returned: 1 \n");
                    printf("MSG from client %d: %s\n", i + 1, buffer);
                }
            }
        }
    }

    // 소켓 종료
    close(access_sock);

    return 0;
}

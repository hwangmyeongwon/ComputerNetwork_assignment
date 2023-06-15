#include <sys/types.h>
#include <netinet/in.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/mman.h> // 구조체


#define PORTNUM 3600
#define BUF_SIZE 1024

typedef struct
{
        int count;
} SharedData;

typedef struct
{
        int chatroom0;
        int chatroom1;
        int chatroom2;
} ChatRoom;

SharedData *shared_data; // 공유 데이터 구조체 포인터
ChatRoom *Chat_Room;     // 공유 데이터 구조체 포인터
int serv_sock;

void error_handling(char *message);

void signal_handler(int signum)
{
        printf("\n(시그널 핸들러) 마무리 작업 시작!\n"); 
        if (signum == SIGINT || signum == SIGTERM)
        {
                // 소켓 닫기, 동적 할당 메모리 free 시키기 등의 작업 수행
                close(serv_sock);
                munmap(shared_data, sizeof(SharedData));
                munmap(Chat_Room, sizeof(ChatRoom));
                shm_unlink("/my_shared_memory");
                shm_unlink("/my_shared_memory1");
                
                // 프로세스 종료
        exit(0);
        }

        else if (signum == SIGCHLD)
        {
                // fork 한 프로세스 wait
                while (waitpid(-1, NULL, WNOHANG) > 0)
                {
                        // 자식 프로세스가 종료되었을 때 수행할 작업
                }
                
        }
        
        
}

int main(int argc, char **argv)
{
        
        signal(SIGINT, signal_handler);
        

        int clnt_sock;
        struct sockaddr_in serv_addr;
        struct sockaddr_in clnt_addr;
        socklen_t clnt_addr_size;
        char message[BUF_SIZE];
        int str_len;

        // 공유 데이터 구조체 생성 및 초기화
        int shm_fd = shm_open("/my_shared_memory", O_CREAT | O_RDWR, 0666);
        ftruncate(shm_fd, sizeof(SharedData));
        SharedData *shared_data = (SharedData *)mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
        shared_data->count = 0;

        // 공유 데이터 구조체 생성 및 초기화
        int shm_fd1 = shm_open("/my_shared_memory1", O_CREAT | O_RDWR, 0666);
        ftruncate(shm_fd1, sizeof(ChatRoom));
        ChatRoom *Chat_Room = (ChatRoom *)mmap(NULL, sizeof(ChatRoom), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd1, 0);
        Chat_Room->chatroom0 = 0;
        Chat_Room->chatroom1 = 0;
        Chat_Room->chatroom2 = 0;

        const char *menu = "<MENU>\n1. Chat Room List\n2. Join a Chat Room (Usage: 2 <room number>)\n3. Exit\n(0을 입력하면 메뉴가 다시 표시됩니다.)\n";

        serv_sock = socket(AF_INET, SOCK_STREAM, 0);
        if (serv_sock == -1)
                error_handling("socket() error");

        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr.sin_port = htons(8013);

        if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
                error_handling("bind() error");

        if (listen(serv_sock, 5) == -1)
                error_handling("listen() error");

        clnt_addr_size = sizeof(clnt_addr);

        int flags = fcntl(clnt_sock, F_GETFL, 0);
        fcntl(clnt_sock, F_SETFL, flags | O_NONBLOCK);

        while (1)
        {
                clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
                if (clnt_sock == -1)
                        error_handling("accept() error");

                pid_t pid = fork(); // 새로운 자식 프로세스 생성

                if (pid == -1)
                {
                        error_handling("fork() error");
                }
                else if (pid == 0)
                {
                        // 자식 프로세스에서 클라이언트와의 통신 처리
                        close(serv_sock); // 자식 프로세스에서는 서버 소켓을 사용하지 않으므로 닫음

                        shared_data->count++; // 클라이언트 번호 ++
                        int count = shared_data->count;
                        printf("클라이언트 %d가 연결되었습니다.\n", count);

                        while (1)
                        {
                                sleep(1);
                                // memset(message, 0, BUF_SIZE);
                                send(clnt_sock, menu, strlen(menu), 0);
                                str_len = recv(clnt_sock, message, BUF_SIZE, 0);

                                if (str_len == -1)
                                        error_handling("read() error");

                                if (strncmp(message, "1", 1) == 0)
                                {
                                        char room_info[100]; // 충분한 크기의 문자열 배열 선언, 채팅방 목록 및 인원 확인

                                        sprintf(room_info, "<ChatRoom info>\n[ID: 0] Chatroom-0 (%d/5)\n[ID: 1] Chatroom-1 (%d/5)\n[ID: 2] Chatroom-2 (%d/5)\n", Chat_Room->chatroom0, Chat_Room->chatroom1, Chat_Room->chatroom2);
                                        send(clnt_sock, room_info, strlen(room_info), 0);
                                        continue;
                                }
                                else if (message[0] == '2')
                                {
                                        printf("사용자 %d가 %c번 채팅방에 접속합니다. \n", count, message[2]);

                                        char chat_num[2];
                                        chat_num[0] = message[2];
                                        chat_num[1] = '\0'; // 문자열의 끝을 표시하는 NULL 문자를 추가

                                        if (message[2] == '0')
                                                Chat_Room->chatroom0++;
                                        else if (message[2] == '1')
                                                Chat_Room->chatroom1++;
                                        else if (message[2] == '2')
                                                Chat_Room->chatroom2++;

                                        while (1)
                                        {
                                                str_len = recv(clnt_sock, message, sizeof(message), 0);

                                                if (strncmp(message, "quit", 4) == 0)
                                                {
                                                        printf("[CH.%c] 사용자 %d를 채팅방에서 제거합니다.\n", chat_num[0], count);
                                                        if (chat_num[0] == '0')
                                                                Chat_Room->chatroom0--;
                                                        else if (chat_num[0] == '1')
                                                                Chat_Room->chatroom1--;
                                                        else if (chat_num[0] == '2')
                                                                Chat_Room->chatroom2--;
                                                        break;
                                                }
                                                if (chat_num[0] == '0' && Chat_Room->chatroom0 == 1)
                                                {
                                                        printf("[CH.%c] 사용자 %d가 혼자여서 메세지를 전달 안합니다.\n", chat_num[0], count);
                                                        // continue;
                                                }

                                                else if (chat_num[0] == '1' && Chat_Room->chatroom1 == 1)
                                                {
                                                        printf("[CH.%c] 사용자 %d가 혼자여서 메세지를 전달 안합니다.\n", chat_num[0], count);
                                                        // continue;
                                                }
                                                else if (chat_num[0] == '2' && Chat_Room->chatroom2 == 1)
                                                {
                                                        printf("[CH.%c] 사용자 %d가 혼자여서 메세지를 전달 안합니다.\n", chat_num[0], count);
                                                        // continue;
                                                }
                                                else
                                                        printf("[CH.%c] 사용자 %d 메세지: %s", chat_num[0], count, message);
                                        }
                                        continue;
                                }
                                else if (strncmp(message, "0", 1) == 0)
                                {
                                        continue;
                                }
                                else if (strncmp(message, "3", 1) == 0)
                                {
                                        break;
                                }
                        }
                        close(clnt_sock);
                        exit(0); // 자식 프로세스 종료
                }

                else
                {
                        close(clnt_sock); // 부모 프로세스에서는 클라이언트 소켓을 사용하지 않으므로 닫음
                }
        }

        return 0;
}

void error_handling(char *message)
{
        fputs(message, stderr);
        fputc('\n', stderr);
        exit(1);
}

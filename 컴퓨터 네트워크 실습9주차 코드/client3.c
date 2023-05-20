#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>


#define BUF_SIZE 1024

void error_handling(char *message);

int main(int argc, char *argv[])
{
    int sock;
    char message[BUF_SIZE];
    int str_len;
    struct sockaddr_in serv_adr;

    // book.txt 에 필요한 변수
    int valread;
    FILE *fp;
    char buffer[1024] = {0};

    // HallymUniv.jpg 에 필요한 변수
    char buf[256];

    // echo server에 필요한 변수
    int recv_len, recv_cnt;

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_adr.sin_port = htons(8011);

    if (connect(sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("connect() error");

    // 위에까진 서버와 연결코드 

    while (1)
    {
        memset(message, 0, BUF_SIZE);
        str_len = recv(sock, message, BUF_SIZE + 1, 0); // Recv Service List Text
        if (str_len == -1)
            error_handling("read() error"); // Recv Error

        printf("%s", message);
        memset(message, 0, BUF_SIZE);


        fgets(message, BUF_SIZE, stdin);  // 사용자 입력 : service 선택, 입력받는 함수중 하나
        send(sock, message, BUF_SIZE, 0); // send service number
        
        // 사용자 입력이 1인 경우 : Current Time Service
        if (strncmp(message, "1", 1) == 0)
        {
            str_len = recv(sock, message, BUF_SIZE, 0);
            printf("%s\n", message);
            memset(message, 0, BUF_SIZE);
            continue;
        }

        // 사용자 입력이 2인 경우 : Download Service
        else if (strncmp(message, "2", 1) == 0)
        {
            str_len = recv(sock, message, BUF_SIZE, 0); //[Available File List] 받음
            printf("%s", message);  //[Available File List] 출력
            memset(message, 0, BUF_SIZE);

            fgets(message, BUF_SIZE, stdin); // 사용자 입력 : 파일 선택
            send(sock, message, BUF_SIZE, 0);

            if (strncmp(message, "1", 1) == 0) // Book.txt
            {
                // Book.txt
                fp = fopen("Book.txt", "w");
                while (1)
                {
                    valread = recv(sock, buffer, sizeof(buffer), 0);
                    if (valread < sizeof(buffer)){
                        fwrite(buffer, sizeof(char),  strlen(buffer), fp);
                        break;
                    }
                    fwrite(buffer, sizeof(char),  sizeof(buffer), fp);
                    memset(buffer, 0, sizeof(buffer));
                    
                }
                printf("done...\n");
                // 파일과 소켓 닫기
                fclose(fp);
            }
            else if (strncmp(message, "2", 1) == 0) // HallymUniv.jpg
            {
                // HallymUniv.jpg
                int nbyte = 256;
                size_t filesize = 0, bufsize = 0;
                FILE *file = NULL;

                file = fopen("HallymUniv.jpg" /* 새로 만들 파일 이름 */, "wb");

                bufsize = 256;

                while (1)
                {
                    nbyte = recv(sock, buf , bufsize, 0);
                    if (nbyte < sizeof(buf)){
                        fwrite(buf, sizeof(char), strlen(buf), file);
                        memset(buf, 0, bufsize);
                        break;
                    }
                    fwrite(buf, sizeof(char), nbyte, file);
                    memset(buf, 0, bufsize);
                }


                fclose(file);
            }
            memset(message, 0, BUF_SIZE);
            continue; // GoBack or Download done...
        }
        // 사용자 입력이 3인 경우 : Echo Server Service
        else if (strncmp(message, "3", 1) == 0)
        {
            while (1)
            {
                // send & recv
                printf("Input message (\\quit): ");
                fgets(message, BUF_SIZE, stdin); // 사용자 입력

                if (send(sock, message, strlen(message), 0) == -1)
                    error_handling("send() error");
                memset(message, 0, BUF_SIZE);

                str_len = recv(sock, message, BUF_SIZE, 0);
                printf("[You] %s\n", message);

                if (strncmp(message, "\\quit", 5) == 0)
                {
                    break;
                }
                memset(message, 0, BUF_SIZE);

            }
            continue;
        }
        if (strncmp(message, "\\quit", 5) == 0)
        {
            printf("Disconnected from server.\n");
            break;
        }
    }
    close(sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define BUF_SIZE 1024
#define PACKET_SIZE 1024
#define BUFFER_SIZE 1024 // 한 번에 읽어올 버퍼 크기

void error_handling(char *message);

int main(int argc, char *argv[])
{
    int serv_sock;
    int clnt_sock;
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;
    char message[BUF_SIZE];
    int str_len;
    int new_str_len;
    char *file_list = "[Available File List]\n1. Book.txt\n2.HallymUniv.jpg\n3.Go back\nEnter : ";
    char *service_list = "[Service List]\n1. Get Current Time\n2. Download File\n3.Echo Server\nEnter : ";

    // 이미지 파일 전송
    char buf[256];
    char buffer[1024] = {0};

    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8011);

    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error");

    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    clnt_addr_size = sizeof(clnt_addr);

    clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
    if (clnt_sock == -1) error_handling("accept() error");

    // 위에까지는 클라이언트와 연결 코드

    while (1)
    {
        sleep(1);
        memset(message, 0, BUF_SIZE);
        // accept() 함수 이후 클라이언트에게 메시지 전송
        send(clnt_sock, service_list, strlen(service_list), 0);
        str_len = recv(clnt_sock, message, BUF_SIZE + 1, 0);

        if (str_len == -1)
            error_handling("read() error");

        if (strncmp(message, "1", 1) == 0)
        {
            // 알려주신 코드로 변경
            time_t rawtime;
            struct tm *timeinfo;

            time(&rawtime);
            timeinfo = localtime(&rawtime);
            char *time = asctime(timeinfo);
            //time[strlen(time)+1]='\0'; //이렇게 하면 메세지의 끝을 구분할수 있다.

            send(clnt_sock, time, strlen(time), 0);

            continue;
        }

        // Download File 을 선택했을 경우
        else if (strncmp(message, "2", 1) == 0)
        {
            send(clnt_sock, file_list, strlen(file_list), 0);
            str_len = recv(clnt_sock, message, BUF_SIZE, 0);
            if (str_len == -1)
                error_handling("read() error");

            if (strncmp(message, "1", 1) == 0)
            {
                // File read
                // Write File using socket
                // book.txt 파일 전송 부분

                char *file_path = "Book.txt"; // 전송할 파일 경로
                FILE* fd = fopen(file_path, "r");
                
                int  bytes_read = -1;
                // 파일 내용 전송
                while (1)
                {
                    bytes_read = fread(buffer, sizeof(char), BUF_SIZE, fd);
                    // 파일 내용 전송
                    if (send(clnt_sock, buffer, bytes_read, 0) == -1)
                    {
                        perror("send");
                        exit(EXIT_FAILURE);
                    }
                   
                    memset(buffer, 0, BUF_SIZE);

                    if (bytes_read <= 0) break;
                }
                // 파일 닫기
                fclose(fd);
                printf("Download Done...(Book.txt)\n");
                continue;
            }
            else if (strncmp(message, "2", 1) == 0)
            {
                FILE *file = NULL;
                size_t nsize = 0;
                /* 전송할 파일 이름을 작성합니다 */
                file = fopen("HallymUniv.jpg" /* 파일이름 */, "rb");

                /* 파일 크기 계산 */
                // move file pointer to end
                fseek(file, 0, SEEK_END);
                // calculate file size
                size_t fsize = ftell(file);
                // move file pointer to first
                fseek(file, 0, SEEK_SET);

                // send file contents
                while (nsize != fsize)
                {
                    // read from file to buf
                    // 1byte * 256 count = 256byte => buf[256];
                    int fpsize = fread(buf, 1, 256, file);
                    nsize += fpsize;
                    send(clnt_sock, buf, fpsize, 0);
                }

                fclose(file);
                continue;
            }
            else if (strncmp(message, "3", 1) == 0)
            {
                // go back 누를 경우
                continue;
            }
        }
        else if (strncmp(message, "3", 1) == 0)
        {

            // while 문으로 서로 계속 통신
            while (1)
            {

                fputs("Input message(\\quit): ", stdout);
                fgets(message, BUF_SIZE, stdin);
                write(clnt_sock, message, strlen(message));

                str_len = read(clnt_sock, message, BUF_SIZE - 1);
                if (str_len == -1)
                    error_handling("read() error");

                message[str_len] = '\0';

                if (!strcmp(message, "\\quit\n"))
                {
                    printf("[Client] disconnected\n");
                    break;
                }

                printf("[You] %s", message);
            }
            continue;
        }
        else
        {
            break;
        }
    }

    close(clnt_sock);
    close(serv_sock);

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

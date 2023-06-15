#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <fcntl.h>


#include "ifdef.h"


int main() {
    int sfd;
    struct sockaddr_un server_addr;

    char buff[1024];

    // 소켓 생성
    sfd = socket(AF_UNIX, SOCK_STREAM, 0);

    // 소켓 주소 설정
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCK_PATH, sizeof(server_addr.sun_path) - 1);

    // 서버에 연결
    connect(sfd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_un));

    // int flags_unix = fcntl(sfd, F_GETFL, 0);
    // fcntl(sfd, F_SETFL, flags_unix | O_NONBLOCK);

    // 서버에서 보낸 데이터 수신
    while (1)
    {
        memset(buff, 0, sizeof(buff));
        printf("Enter : ");
        fgets(buff, sizeof(buff), stdin);
        
        // write(sfd, buff, sizeof(buff));
        send(sfd, buff, sizeof(buff), 0);
    }

    // 연결 종료
    close(sfd);

    return 0;
}

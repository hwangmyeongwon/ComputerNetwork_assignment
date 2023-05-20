#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handler(int signum) {
    printf("Handler is called.\n");
    exit(EXIT_SUCCESS);
}

int main() {
    signal(SIGINT, handler);
    printf("Sleep begins!\n");
    sleep(1000);
    printf("Wake up!\n");

    return 0;
}

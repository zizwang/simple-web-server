#include "response.h"

int main(int argc, char *argv[]) {
    int                 servfd, clifd;
    struct sockaddr_in  servaddr, cliaddr;
    socklen_t           clilen;

    servfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(8080);

    bind(servfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    listen(servfd, 128);
 
    signal(SIGCHLD, SIG_IGN);

    while(1) {
        clilen = sizeof(cliaddr);
        clifd = accept(servfd, (struct sockaddr *) &cliaddr, &clilen);

        if (fork() == 0) {
            close(servfd);
            response(clifd);
            close(clifd);
            exit(0);
        }
        close(clifd);
    }
}

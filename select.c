#include "response.h"

int main(int argc, char *argv[]) {
    char                buf[BUFSIZE];
    int                 servfd, clifd, fd, buflen;
    struct sockaddr_in  servaddr, cliaddr;
    socklen_t           clilen;
    fd_set              testset, allset;

    servfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(8080);

    bind(servfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    listen(servfd, 128);

    FD_ZERO(&allset);
    FD_SET(servfd, &allset);

    while(1) {
        testset = allset;
        select(FD_SETSIZE, &testset, NULL, NULL, NULL);

        for (fd = 0; fd < FD_SETSIZE; ++fd)
            if (FD_ISSET(fd, &testset)) {
                if (fd == servfd) {
                    clilen = sizeof(cliaddr);
                    clifd = accept(servfd, (struct sockaddr *) &cliaddr, &clilen);
                    FD_SET(clifd, &allset);
                }
                else {
                    ioctl(fd, FIONREAD, &buflen);
                    if (buflen == 0) {
                        close(fd);
                        FD_CLR(fd, &allset);
                    }
                    else {
                        response(fd);
                    }
                }
            }
    }
}

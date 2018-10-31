#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/ioctl.h>

#define BUFSIZE 1024
#define FILESIZE 2

struct {
    char *ext;
    char *type;
} file[FILESIZE] = {
    {"jpg",  "image/jpeg"},
    {"jpeg", "image/jpeg"}
};

char *content = "HTTP/1.1 200 OK\r\nContent-Type: ";
char *charset = "; charset=utf-8\r\n\r\n";

void response(int clifd) {
    char buf[BUFSIZE];
    int i, buflen, extlen, filefd;

    memset(buf, 0, BUFSIZE);
    read(clifd, buf, BUFSIZE);
    for (i = 5; i < strlen(buf); ++i) 
        if (buf[i] == ' ') {
            buf[i] = 0;
            break;
        }

    chdir("./webpage");

    write(clifd, content, strlen(content));
    for (i = 0; i < FILESIZE; ++i) {
        buflen = strlen(buf);
        extlen = strlen(file[i].ext);
        if (!strcmp(&buf[buflen - extlen], file[i].ext)) {
            write(clifd, file[i].type, strlen(file[i].type));
            filefd = open(&buf[5], O_RDONLY);
            break;
        }
    }
    if (!strcmp(buf, "GET /")) {
        write(clifd, "text/html", 9);
        filefd = open("index.html", O_RDONLY);
    }
    write(clifd, charset, strlen(charset));

    while ((buflen = read(filefd, buf, BUFSIZE)) > 0) {
        buf[buflen] = 0;
        write(clifd, buf, buflen);
    }
    close(filefd);
}

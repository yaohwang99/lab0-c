#include <arpa/inet.h> /* inet_ntoa */
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#ifndef _TINY_H
#define _TINY_H

#define LISTENQ 1024 /* second argument to listen() */
#define MAXLINE 1024 /* max length of a line */
#define RIO_BUFSIZE 1024

#ifndef DEFAULT_PORT
#define DEFAULT_PORT 9999 /* use this port if none given as arg to main() */
#endif

#ifndef FORK_COUNT
#define FORK_COUNT 4
#endif

#ifndef NO_LOG_ACCESS
#define LOG_ACCESS
#endif
extern int listenfd;
extern bool noise;
typedef struct RIO_ELE rio_t, *rio_ptr;

struct RIO_ELE {
    int rio_fd;                /* File descriptor */
    int rio_cnt;               /* Unread bytes in internal buffer */
    char *rio_bufptr;          /* Next unread byte in internal buffer */
    char rio_buf[RIO_BUFSIZE]; /* Internal buffer */
    rio_ptr prev;              /* Next element in stack */
};

/* Simplifies calls to bind(), connect(), and accept() */
typedef struct sockaddr SA;

typedef struct {
    char filename[512];
    off_t offset; /* for support Range */
    size_t end;
} http_request;


void rio_readinitb(rio_t *rp, int fd);

ssize_t writen(int fd, void *usrbuf, size_t n);


/*
 * rio_read - This is a wrapper for the Unix read() function that
 *    transfers min(n, rio_cnt) bytes from an internal buffer to a user
 *    buffer, where n is the number of bytes requested by the user and
 *    rio_cnt is the number of unread bytes in the internal buffer. On
 *    entry, rio_read() refills the internal buffer via a call to
 *    read() if the internal buffer is empty.
 */
/* $begin rio_read */
ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n);
/*
 * rio_readlineb - robustly read a text line (buffered)
 */
ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);

const char *get_mime_type(char *filename);

int open_listenfd(int port);


void url_decode(char *src, char *dest, int max);

void parse_request(int fd, http_request *req);

char *process(int fd, struct sockaddr_in *clientaddr);

#endif /* _TINY_H */
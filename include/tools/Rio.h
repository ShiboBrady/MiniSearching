#ifndef RIO_H_
#define RIO_H_
#define BUFFERSIZE 8192
#define MAXLINE 1024
#include <stdlib.h>
#include <unistd.h>

class Rio
{
    public:
        Rio(int fd);
        ssize_t readn(char *usrbuf, size_t len);
        ssize_t writen(const char *usrbuf, size_t len);
        ssize_t readLine(char *usrbuf, size_t len);
    private:
        ssize_t read(char *usrbuf, size_t len);
        int fd_;
        size_t left_;
        char *bufptr_;
        char buffer_[BUFFERSIZE];
};

#endif  /*RIO_H*/

#include "Rio.h"
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <iostream>
using namespace std;


Rio::Rio(int fd)
    :fd_(fd),
     left_(0),
     bufptr_(buffer_)
{
    bzero(buffer_, BUFFERSIZE);
}

ssize_t Rio::read(char *usrbuf, size_t len)
{
    int nread;
    while(left_ <= 0){
        bzero(buffer_, BUFFERSIZE);
        nread = ::read(fd_, buffer_, BUFFERSIZE);
        if(nread == -1){
            if(errno == EINTR)
                continue;
            return -1;
        }else if(nread == 0)
            return 0;
        left_ = nread;
        bufptr_ = buffer_;
    }
    nread = len;
    if(left_ < len)
        nread = left_;
    memcpy(usrbuf, bufptr_, nread);
    bufptr_ += nread;
    left_ -= nread;
    return nread;
}

ssize_t Rio::readn(char *usrbuf, size_t len)
{
    size_t nleft = len;
    ssize_t nread;
    char *bufp = usrbuf;

    while(nleft > 0){
        nread = read(bufp, nleft);
        if(nread == -1)
            return -1;
        else if(nread == 0)
            break;
        nleft -= nread;
        bufp += nread;
    }
    return (len - nleft);
}

ssize_t Rio::writen(const char *usrbuf, size_t len)
{
    size_t nleft = len;
    ssize_t nwrite;
    const char *bufp = usrbuf;

    while(nleft > 0){
        nwrite = write(fd_, bufp, nleft);
        if(nwrite <= 0){
            if(errno == EINTR)
                continue;
            return -1;
        }
        nleft -= nwrite;
        bufp += nwrite;
    }
    return len;
}

ssize_t Rio::readLine(char *usrbuf, size_t len)
{
    ssize_t nread;
    char c;
    char *bufp = usrbuf;
    size_t i;

    for(i = 0; i < len; ++i){
         nread = read(&c, 1);
         //cout << "nread = " <<nread << endl;
         if(nread == -1)
             return -1;
         else if(nread == 0){
            if(i == 0)
                return 0;
            break;
         }
         *bufp++ = c;
         if(c == '\n')
             break;
    }
    *bufp = '\0';
    return i;
}


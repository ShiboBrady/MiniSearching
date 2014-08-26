#ifndef DOCUMENT_H
#define DOCUMENT_H 
#include <string>

struct Document
{
    size_t doctid_;
    std::string title_;
    std::string url_;
    std::string content_;
    long offset_;
    size_t len_;
};

#endif  /*DOCUMENT_H*/

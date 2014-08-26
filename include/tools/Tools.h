#ifndef TOOLS_H
#define TOOLS_H 
#include <string>
#include <set>
#include "Document.h"

namespace tools
{
    std::string formateDocument(const Document &doct);
    void deleteEnPunct(std::string &str);
    std::string trimEnter(const std::string &str);
    void trimSpace(char *str);
}

#endif  /*TOOLS_H*/

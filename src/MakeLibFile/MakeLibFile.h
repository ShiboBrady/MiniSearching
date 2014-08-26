#ifndef MAKELIBFILE
#define MAKELIBFILE 

#include "Word.h"
#include "Document.h"
#include <map>
#include <set>
#include <list>
#include <string>

class GetConfig;

class MakeLibFile
{
    public:
        MakeLibFile(GetConfig *config);
        void traverseDirc(const std::string &path);
        void readfile(const std::string &file, const std::string &path);
        void saveLib();
        void start();
    private:
        std::list<Document> document_; //存放所有文章
        std::string pagefilepath_;
        std::string pagefile_;
        std::string libfile_;
        std::string libindexfile_;
        size_t doctid_;
};

#endif  /*MAKELIBFILE*/

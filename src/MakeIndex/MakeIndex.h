#ifndef MAKEINDEX_H
#define MAKEINDEX_H 
#include "MixSegment.hpp"
#include "Document.h"
#include <list>
#include <map>
#include <set>
#include <string>

class GetConfig;

class MakeIndex
{
    public:
        MakeIndex(GetConfig *config);
        void readExcludeFile();
        void readLibFile();
        void countDoctFrequence();
        void calcWeight();
        void normalization();
        void saveIndex();
        void start();
    private:
        std::unordered_set<std::string> exclude_;
        std::list<Document> document_;
        std::map<std::string, std::map<size_t, int> > df_;
        std::map<std::string, std::map<size_t, double> >weight_;
        CppJieba::MixSegment segment_;
        std::string libfile_;
        std::string libindexfile_;
        std::string excludefile_;
        std::string indexfile_;
        size_t doctid_;
};

#endif  /*MAKEINDEX_H*/

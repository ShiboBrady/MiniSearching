#ifndef DELETEREPEAT_H
#define DELETEREPEAT_H 
#include "MixSegment.hpp" 
#include "Word.h"
#include "Document.h"
#include <string>
#include <map>
//#include <unordered_set>
#include <list>

class GetConfig;

class DeleteRepeat
{
    public:
        DeleteRepeat(GetConfig *config);
        void readExcludeFile();
        void readLibFile();
        void countFrequence();
        void makeFeatureValue();
        void deleteRepeat();
        void saveLib();
        bool isSimilar(std::map<std::string, int> &doc1, std::map<std::string, int> &doc2);
        void start();
    private:
        CppJieba::MixSegment segment_;
        std::list<Document> document_;
        std::map<size_t, std::map<std::string, int> > frequence_;
        std::map<size_t, std::map<std::string, int> > feature_;
        std::unordered_set<std::string> exclude_;

        std::string libfile_;
        std::string libindexfile_;
        std::string excludefile_;
        size_t doctid_;
};

#endif  /*DELETEREPEAT_H*/

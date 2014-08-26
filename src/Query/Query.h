#ifndef QUERY_H
#define QUERY_H 
#include "MixSegment.hpp"
#include "GetConfig.h"
#include "Tools.h"
#include "Document.h"
//#include <unordered_map>
#include <map>
#include <set>
#include <string>

struct PageSimilarity
{
    size_t doctid_;
    double similarity_;

    bool operator < (const PageSimilarity &other) const{
        return similarity_ < other.similarity_;
    }
};

class Query
{
    public:
        Query(GetConfig *config);
        void readExcludeFile();
        void readLibIndexFile();
        void readIndexFile();
        void query(std::string str);
        int queryWordNormaraize(std::string str, 
                                 std::map<std::string, double> &queryword);
        void display(size_t doctid);
    private:
        std::map<std::string, std::map<size_t, double> > weight_word_;
        std::map<size_t, Document> libindex_;
        std::unordered_set<std::string> exclude_;
        CppJieba::MixSegment segment_;
        std::string excludefile_;
        std::string indexfile_;
        std::string libfile_;
        std::string libindexfile_;
        size_t doctid_;
};

#endif  /*QUERY_H*/

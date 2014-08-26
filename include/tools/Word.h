#ifndef WORD_H
#define WORD_H 
#include <string>

struct Word
{
    std::string word_;
    int frequence_;

    bool operator < (const Word &other) const{
            return frequence_ < other.frequence_;
        }   
};

#endif  /*WORD_H*/

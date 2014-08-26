#include "MixSegment.hpp"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

int main(int argc, const char *argv[])
{
    string str = "In this paper, a new BDI model for rational agents is presented by introducing assumptive belief with traditional belief in order to express the intuitive meaning of desire and intention on the cognitive aspect of the metal state of rational agents. Comparing with the BDI models from Cohen & Levesque, Rao & Georgeff, and Konolige & Pollack, this model overcomes the misunderstanding of the concepts of BDI, solves the transference problem and the side-effect problem for desire and intention, and shows both of the static and the dynamic relations between BDI, especially the maintaining and triggering role of desire."; 
    vector<string> words;
    string dict_path = "../dict/jieba.dict.utf8";
    string model_path = "../dict/hmm_model.utf8";
    CppJieba::MixSegment segment(dict_path.c_str(), model_path.c_str());
    segment.cut(str, words);
    for(vector<string>::iterator it = words.begin(); it != words.end(); ++it){
        cout << *it << endl;    
    }
    return 0;
}

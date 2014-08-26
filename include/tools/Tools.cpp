#include "Tools.h"
#include <sstream>
#include <string.h>
using namespace std;
string tools::formateDocument(const Document &docInfo)
{
    ostringstream oss;
    oss << "<doc>" << endl
        << "    <docid>" << docInfo.doctid_ << "</docid>" << endl
        << "    <url>" << docInfo.url_ << "</url>" << endl
        << "    <title>" << docInfo.title_ << "</title>" << endl
        << "    <content>" << endl
        << "        " << docInfo.content_
        << "    </content>" << endl
        << "</doc>" << endl;
    return oss.str();
}

void tools::deleteEnPunct(string &str)
{
    for(string::size_type i = 0; i != str.size(); ++i){
        if(ispunct(str[i]))
            str[i] = 32;
        else if(isupper(str[i]) && islower(str[i+1]))
            str[i] += 32;
    }
}

string tools::trimEnter(const string &str)
{
    char temp[1024] = {0};
    strcpy(temp, str.c_str());
    temp[strlen(temp) - 1] = 0;
    return string(temp);
}

void tools::trimSpace(char *temp)
{
    char *p = temp;
    int count = 0;
    while(*p == 32){
        count++;
        p++;
    }
    if(count > 0){
        while(*p != 0){
            *(p - count) = *p;
            p++;
        }
        *(p - count) = 0;
    }
}

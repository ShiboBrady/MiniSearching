#include "Query.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, const char *argv[])
{
    GetConfig *config = GetConfig::getInstance();
    config->readConf("../conf/search.conf");
    Query query(config);
    char buffer[1024];
    cout << "Welcome to use WangDao Mini Searching!" << endl;
    cout << "Please input: " << endl;
    while(fgets(buffer, 1024, stdin) > 0){
        buffer[strlen(buffer) - 1] = 0;
        query.query(string(buffer));
        cout << "Please input: " << endl;
    }
    return 0;
}

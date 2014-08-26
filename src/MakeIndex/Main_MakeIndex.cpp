#include "MakeIndex.h"
#include "GetConfig.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, const char *argv[])
{
    GetConfig *config = GetConfig::getInstance();
    config->readConf("../conf/search.conf");
    MakeIndex index(config);
    index.start();
    return 0;
}

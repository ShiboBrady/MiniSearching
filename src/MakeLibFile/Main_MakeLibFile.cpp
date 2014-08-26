#include "MakeLibFile.h"
#include "GetConfig.h"
#include <string>

using namespace std;

int main(int argc, const char *argv[])
{
    GetConfig *config = GetConfig::getInstance();
    config->readConf("../conf/search.conf");
    MakeLibFile makelib(config);
    makelib.start();  
    return 0;
}

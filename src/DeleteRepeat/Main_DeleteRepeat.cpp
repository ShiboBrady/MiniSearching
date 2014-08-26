#include "DeleteRepeat.h"
#include "GetConfig.h"

int main(int argc, const char *argv[])
{
    GetConfig *config = GetConfig::getInstance();
    config->readConf("../conf/search.conf");
    DeleteRepeat del(config);
    del.start();
    return 0;
}

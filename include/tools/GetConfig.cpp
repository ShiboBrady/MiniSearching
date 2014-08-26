#include "GetConfig.h"
#include "Rio.h"
#include <fstream>
#include <stdexcept>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

GetConfig *GetConfig::pInstance_ = NULL;
pthread_once_t GetConfig::once_ = PTHREAD_ONCE_INIT;

void GetConfig::readConf(const string &filename)
{
    int fd_read = open(filename.c_str(), O_RDONLY);
    if(fd_read == -1)
        throw runtime_error("open config file failed.");
    Rio rio(fd_read);
    char buffer[1024] = {0};
    while(rio.readLine(buffer, 1024) > 0){
        string line(buffer);
        string type;
        string param;
        string::size_type pos;
        pos = line.find("=");
        type = line.substr(0, pos);
        param = line.substr(pos + 1);

        if(type == "pagefilepath")
            pagefilepath_ = param;
        else if(type == "pagefile")
            pagefile_ = param;
        else if(type == "libfile")
            libfile_ = param;
        else if(type == "libfilename")
            libfilename_ = param;
        else if(type == "libIndexfile")
            libIndexfile_ = param;
        else if(type == "libIndexfilename")
            libIndexfilename_ = param;
        else if(type == "excludefile")
            excludefile_ = param;
        else if(type == "excludefilename")
            excludefilename_ = param;
        else if(type == "indexfile")
            indexfile_ = param;
        else if(type == "indexfilename")
            indexfilename_ = param;
        else if(type == "dict_path")
            dict_path_ = param;
        else if(type == "model_path")
            model_path_ = param;
    }
}

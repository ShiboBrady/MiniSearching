#include "MakeLibFile.h"
#include "GetConfig.h"
#include "Rio.h"
#include "Tools.h"
#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <queue>
using namespace std;

MakeLibFile::MakeLibFile(GetConfig *config)
    :pagefilepath_(tools::trimEnter(config->getPagefilepath())),
     pagefile_(tools::trimEnter(config->getPagefile())),
     libfile_(tools::trimEnter(config->getLibfileName())),
     libindexfile_(tools::trimEnter(config->getLibIndexfileName())),
     doctid_(1)
{}

void MakeLibFile::start()
{
    traverseDirc(pagefilepath_);
    saveLib();
}

//遍历目录
void MakeLibFile::traverseDirc(const string &path)
{
    DIR *dir;
    struct dirent *reader;
    struct stat my_stat;
    dir = opendir(path.c_str());
    if(dir == NULL)
        throw runtime_error("opendir");
    chdir(path.c_str());
    cout << getcwd(NULL, 0) << endl;
    while((reader = readdir(dir)) != NULL){
        if(!((strncmp(reader->d_name, ".", 1) == 0) || (strncmp(reader->d_name, "..", 2) == 0))){
           stat(reader->d_name, &my_stat);
           if(my_stat.st_mode & 0040000)
               traverseDirc(reader->d_name);
           else
               readfile(reader->d_name, path); 
        }
    }
    chdir("..");
    closedir(dir);
}

//读网页文件，并对其进行分析
void MakeLibFile::readfile(const string &file, const string &path)
{
    Document docInfo;
    string line;
    char buffer[1024] = {0};
    docInfo.doctid_ = (doctid_ ++); //记下网页编号
    docInfo.url_ = path + "/" + file; //记下网页路径

    int fd_read = open(file.c_str(), O_RDONLY);
    if(fd_read == -1)
        throw runtime_error("open pagefile to read fail.");
    Rio rio(fd_read);

    rio.readLine(buffer, 1024);  //读出第一行，并把该行作为网页的标 

    //修改一行文字后面的换行符
    buffer[strlen(buffer) - 1] = 0;
    buffer[strlen(buffer) - 1] = '\n'; //去掉标题后面的/r，保留/n
    docInfo.content_ += buffer;

    //去掉第一行文字后面的换行符，作为网页标题
    buffer[strlen(buffer) - 1] = 0; //取出字符串后面的/n
    tools::trimSpace(buffer); //去除标题前方的空格
    docInfo.title_ = buffer;  

    //将一片文档内容全部读出来
    bzero(buffer, 1024);
    while(rio.readLine(buffer, 1024) > 0){ 
        buffer[strlen(buffer) - 1] = 0;
        buffer[strlen(buffer) - 1] = '\n';
        line = string(buffer);
        if(line != "\n")
            docInfo.content_ += line;
        bzero(buffer, 1024);
    }
    close(fd_read);

    //这一块儿代码将网页信息加入到网页数组中
    document_.push_back(docInfo);
}

void MakeLibFile::saveLib()
{   
    int lib_write, index_write;
    lib_write = open(libfile_.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(lib_write == -1)
        throw runtime_error("open lib file to write fail.");
    index_write = open(libindexfile_.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(index_write == -1)
        throw runtime_error("open index file to write fail.");

    Rio lib_rio(lib_write);
    Rio index_rio(index_write);

    list<Document>::iterator it;
    for(it = document_.begin(); it != document_.end(); ++it){
        //将格式化后的网页信息写到库文件中
        it->offset_ = lseek(lib_write, 0, SEEK_CUR);  //记下文件开头处的文件偏移量值
        string fmatedoc = tools::formateDocument(*it);  //对网页内容格式化处理
        it->len_ = fmatedoc.size();
        lib_rio.writen(fmatedoc.c_str(), it->len_);

        //将库文件索引信息保存到库索引文件中
        ostringstream oss;
        oss << it->doctid_ << " " << it->offset_ << " " << it->len_ << endl;
        string index = oss.str();
        index_rio.writen(index.c_str(), index.size());
    } 
}


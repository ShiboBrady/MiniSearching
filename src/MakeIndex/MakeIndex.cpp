#include "MakeIndex.h"
#include "GetConfig.h"
#include "Rio.h"
#include "Tools.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

using namespace std;

MakeIndex::MakeIndex(GetConfig *config)
    :segment_(tools::trimEnter(config->getDict_path()).c_str(), 
              tools::trimEnter(config->getModel_path()).c_str()),
     libfile_(tools::trimEnter(config->getLibfile())),
     libindexfile_(tools::trimEnter(config->getLibIndexfile())),
     excludefile_(tools::trimEnter(config->getExcludefile())),
     indexfile_(tools::trimEnter(config->getIndexfile())),
     doctid_(1)
{
}

void MakeIndex::start()
{
    readExcludeFile(); //第一步：读入排除集文档
    readLibFile();  //第二步：读入库文件
    countDoctFrequence();  //第四步：计算某个单词在多少网页中出现过：df
    calcWeight();  //第五步：计算每篇网页中每次单词的词频
    normalization(); //第六步：权重归一化
    saveIndex(); //第七步：保存到索引文件中
}

//读入排除集文档
void MakeIndex::readExcludeFile()
{
    int fd_exclude = open(excludefile_.c_str(), O_RDONLY);
    if(fd_exclude == -1)
        throw runtime_error("open exclude file to read failed.");
    char buffer[1024] = {0};
    Rio rio_exclude(fd_exclude);
    while(rio_exclude.readLine(buffer, 1024) > 0){
        buffer[strlen(buffer) - 2] = 0;
        exclude_.insert(string(buffer));
    }
    exclude_.insert("\r");
    exclude_.insert("\n");
    cout << "Read exclude file over." << endl;
    close(fd_exclude);
}

//读入库文件
void MakeIndex::readLibFile()
{
    int fd_lib = open(libfile_.c_str(), O_RDONLY);
    if(fd_lib == -1)
        throw runtime_error("open lib file to read failed.");
    int fd_index = open(libindexfile_.c_str(), O_RDONLY);
    if(fd_index == -1)
        throw runtime_error("open lib index file to read failed.");

    Rio rio_lib(fd_lib);
    Rio rio_index(fd_index);

    char buffer[1024] = {0};
    size_t doctid;
    long offset;
    size_t len;
    while(rio_index.readLine(buffer, 1024) > 0){
        Document document;

        sscanf(buffer, "%u %ld %u", &doctid, &offset, &len);
        document.doctid_ = doctid;
        document.offset_ = offset;
        document.len_ = len;

        lseek(fd_lib, document.offset_, SEEK_SET);
        char contentbuffer[100000] = {0};
        read(fd_lib, contentbuffer, document.len_);

        string doct(contentbuffer);
        string::size_type pos1, pos2;

        //读取网页的url
        pos1 = doct.find("<url>") + 5;
        pos2 = doct.find("</url>");
        document.url_ = doct.substr(pos1, pos2 - pos1);

        //读取网页的标题
        pos1 = doct.find("<title>") + 7;
        pos2 = doct.find("</title>");
        document.title_ = doct.substr(pos1, pos2 - pos1);

        //读取网页的内容
        pos1 = doct.find("<content>") + 9;
        pos2 = doct.find("</content>");
        document.content_ = doct.substr(pos1, pos2 - pos1);

        document_.push_back(document);
        doctid_ ++;
    }
    cout << "Read lib file over." << endl;
}

//统计每一篇网页的词频：tf
void MakeIndex::countDoctFrequence()
{
    list<Document>::iterator listit;
    for(listit = document_.begin(); listit != document_.end(); ++listit){
        //cout << "Count one page over."  << endl;
        //第一步：去掉英文的标点符号，并把首字母大写的单词转换为小写
        string content = listit->content_;
        tools::deleteEnPunct(content);

        //第二步：分词
        vector<string>words;
        segment_.cut(content, words);

        //第三步：统计词频
        vector<string>::iterator it;
        unordered_set<string>::iterator setit;
        map<string, int> wordfre;
        for(it = words.begin(); it != words.end(); ++it){
            setit = exclude_.find(*it);
            if(setit == exclude_.end())
                df_[*it][listit->doctid_]++;
        }
    }
    cout << "TF and DF is count over." << endl;
}

//计算每一个单词的权重
void MakeIndex::calcWeight()
{
    //这一步主要是遍历每篇文章中的每一个单词，计算它的权重
    map<string, map<size_t, int> >::iterator dfit;
    map<size_t, int>::iterator tfit;
    //计算权重，将文章编号，单词，权重插入权重数据结构中
    for(dfit = df_.begin(); dfit != df_.end(); ++dfit)
        for(tfit = dfit->second.begin(); tfit != dfit->second.end(); ++tfit)
            weight_[dfit->first][tfit->first] = tfit->second * log(doctid_ / dfit->second.size());
    cout << "Weight is count over." << endl;
}

//权重归一化运算
void MakeIndex::normalization()
{
    /*
    list<Document>::iterator listit; 
    map<string, map<size_t, double> >::iterator mapit;
    map<size_t, double>::iterator sizeit;
    for(listit = document_.begin(); listit != document_.end(); ++listit){
        double denominator = 0;
        //第一趟循环，找出对应某一篇网页的所有单词权重，计算它们的平方和
        for(mapit = weight_.begin(); mapit != weight_.end(); ++mapit)
            if((sizeit = mapit->second.find(listit->doctid_)) != mapit->second.end())
                denominator += sizeit->second * sizeit->second;

        //对分母开方
        denominator = sqrt(denominator);

        cout << listit->doctid_ << endl;
        //第二趟循环，用该篇网页的所有单词权重除以上面计算出来的分母，即权重归一化
        for(mapit = weight_.begin(); mapit != weight_.end(); ++mapit)
            if((sizeit = mapit->second.find(listit->doctid_)) != mapit->second.end())
                sizeit->second /= denominator;
    }
    */

    //权值归一化过程也可以使用下面的代码，下面的更清晰易懂，只是多耗费了一倍的内存
    //数据结构及迭代器声明
    map<size_t, map<string, double> > normalize;
    map<size_t, map<string, double> >::iterator doctit; 
    map<string, map<size_t, double> >::iterator wordit;
    map<size_t, double>::iterator sizeit;
    map<string, double>::iterator strit;

    //第一步：将map<string, map<size_t, double> >类型的数据结构
    //    转换为map<size_t, map<string, double> >类型的数据结构
    for(wordit = weight_.begin(); wordit != weight_.end(); ++wordit)
         for(sizeit = wordit->second.begin(); sizeit != wordit->second.end(); ++sizeit)
             normalize[sizeit->first].insert(make_pair(wordit->first, sizeit->second));

    //第二步：进行权重归一化计算
    for(doctit = normalize.begin(); doctit != normalize.end(); ++doctit){
        double denominator = 0;
        for(strit = doctit->second.begin(); strit != doctit->second.end(); ++strit)
            denominator += strit->second * strit->second;
        denominator = sqrt(denominator);
        for(strit = doctit->second.begin(); strit != doctit->second.end(); ++strit)
            strit->second /= denominator;
        cout << doctit->first << endl;
    }
    //第三步：用归一化后的权重值更新之前的权重值
    for(doctit = normalize.begin(); doctit != normalize.end(); ++doctit)
        for(strit = doctit->second.begin(); strit != doctit->second.end(); ++strit)
            weight_[strit->first][doctit->first] = strit->second;
    cout << "Normalization is over." << endl;
}

//保存索引信息到索引文件中
void MakeIndex::saveIndex()
{
    int index_write;
    if(-1 == (index_write = open(indexfile_.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666)))
        throw runtime_error("Open index file to write fail.");

    Rio rio_write(index_write);

    map<string, map<size_t, double> >::iterator wordit;
    map<size_t, double>::iterator sizeit;
    for(wordit = weight_.begin(); wordit != weight_.end(); ++wordit){
        ostringstream oss;
        oss << wordit->first;

        //第一步：格式化索引信息
        for(sizeit = wordit->second.begin(); sizeit != wordit->second.end(); ++sizeit)
             oss << " " << sizeit->first << " " << sizeit->second;
        oss << endl;

        //第二步：写入索引文件
        string indexstr = oss.str();
        rio_write.writen(indexstr.c_str(), indexstr.size()); 
    }
    close(index_write);
}


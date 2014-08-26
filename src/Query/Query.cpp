#include "Query.h"
#include "Rio.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <queue>
using namespace std;

Query::Query(GetConfig *config)
    :segment_(tools::trimEnter(config->getDict_path()).c_str(), 
              tools::trimEnter(config->getModel_path()).c_str()),
     excludefile_(tools::trimEnter(config->getExcludefile().c_str())),
     indexfile_(tools::trimEnter(config->getIndexfile())),
     libfile_(tools::trimEnter(config->getLibfile())),
     libindexfile_(tools::trimEnter(config->getLibIndexfile())),
     doctid_(0)
{
    readExcludeFile();
    readLibIndexFile();
    readIndexFile(); 
}

void Query::readExcludeFile()
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

void Query::readLibIndexFile()
{
    int fd_index = open(libindexfile_.c_str(), O_RDONLY);
    if(fd_index == -1)
        throw runtime_error("open lib index file to read failed.");

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

        libindex_[doctid] = document;
        doctid_ ++;
    }
    close(fd_index);
    cout << "Read lib index file over." << endl;
}

void Query::readIndexFile()
{
    //将索引表中的内容按：单词、网页编号、归一化权重。。。的格式读出来
    FILE *fp_index = fopen(indexfile_.c_str(), "r");
    if(fp_index == NULL)
        throw runtime_error("open index file to read fail.");

    char buffer[100000];
    string param1;
    string param2;
    string word;
    int doctid;
    double weight;
    while(fgets(buffer, 100000, fp_index) != NULL){
        istringstream oss(buffer);
        oss >> word;
        while(oss >> doctid >> weight)
            weight_word_[word].insert(make_pair(doctid, weight));
    }
    fclose(fp_index);
    cout << "Read index file over." << endl;
}

void Query::query(string str)
{
    map<string, double> queryword;
    //对输入的字符串解析，并计算归一化的权值
    if(!queryWordNormaraize(str, queryword)){
        cout << "Sorry, we haven't found!" << endl;
        return;
    }
    
    //计算查询单词中df最小的单词
    vector<size_t> df_count;
    map<string, double>::iterator witit;
    for(witit = queryword.begin(); witit != queryword.end(); ++witit){
        df_count.push_back(weight_word_[witit->first].size()); 
    }
    vector<size_t>::iterator svec;
    size_t mindf = df_count.front();
    map<string, double>::iterator witpreit, witcurit;
    witpreit = witcurit = queryword.begin();
    for(svec = df_count.begin(); svec != df_count.end(); ++svec, ++witcurit){
        if(*svec < mindf){
            mindf = *svec;
            witpreit = witcurit;
        }
    }
    //求df最小的那个单词出现的网页列表
    set<size_t>docids;
    map<size_t, double>::iterator sizeit;
    map<size_t, double> &docidlist = weight_word_[witpreit->first];
    for(sizeit = docidlist.begin(); sizeit != docidlist.end(); ++ sizeit)
        docids.insert(sizeit->first);

    //求所有单词对应的网页列表的交集
    set<size_t>::iterator setit; //用来存放网页交集的数据结构
    map<size_t, double>::iterator rtn;
    string firstword = witpreit->first;
    for(witit = queryword.begin(); witit != queryword.end(); ++witit){

        //第一步：判断是否是df最小的那个单词，如果是，则跳过
        if(witit->first == firstword)
            continue;

        //第二步：遍历网页列表，在每个网页中查找要查询的单词，
        //如果某个网页没找到，则说明该网页不是所要的网页，删之~
        map<size_t, double> &temp = weight_word_[witit->first];
        for(setit = docids.begin(); setit != docids.end(); ++setit){
            if(temp.find(*setit) == temp.end()){
                docids.erase(setit);
                setit --;
            }

            //说明所有单词组合在一起的时候，是没有合适的结果的。
            if(docids.size() == 0){
                cout << "Sorry, we haven't found!" << endl;
                return;
            }
        }
    } 

    //依次用求得的符合条件的网页单词向量和查询文本单词向量相乘并排序
    //将结果存放在优先级队列中，最后只需从头到尾的从队列中取出即可
    priority_queue<PageSimilarity> queue;
    cout << "Total found about " << docids.size() << " pages." << endl;
    //从头到尾遍历文档交集，计算相似度，放入优先级队列
    for(setit = docids.begin(); setit != docids.end(); ++setit){
        double similarity = 0;
        for(witit = queryword.begin(); witit != queryword.end(); ++witit){
            similarity += witit->second * weight_word_[witit->first][*setit];
        }
        PageSimilarity newpage = {*setit, similarity};
        queue.push(newpage);
    }
    //将最终查询好的网页列表发回前台展示
    while(!queue.empty()){
        display(queue.top().doctid_);
        queue.pop();
    }
}

int Query::queryWordNormaraize(string str, map<string, double> &queryword)
{
    //对输入的查询字符串切词
    vector<string> words;
    segment_.cut(str, words);

    //去掉停用词并统计词频
    tools::deleteEnPunct(str);
    map<string, int> querywordfrequence; //用来记录输入字符串中每个词语的频率
    vector<string>::iterator ivec;
    for(ivec = words.begin(); ivec != words.end(); ++ ivec){
        if(exclude_.find(*ivec) == exclude_.end())
            querywordfrequence[*ivec] ++;
    }
    
    cout << querywordfrequence.size() << endl;
    for(map<string, int>::iterator it = querywordfrequence.begin(); it != querywordfrequence.end(); ++it)
        cout << it->first << endl;

    //计算权值
    for(ivec = words.begin(); ivec != words.end(); ++ ivec){
        double denominator = weight_word_[*ivec].size();
        //当这个词在网页库中不存在时，返回的值是零，需要把该词弃之
        //当用户输入的字符串中没有一个单词是词库中有的，就返回查询结果为零
        if(denominator == 0){
            words.erase(ivec);
            --ivec;
            if(words.size() == 0)
                return 0;
            continue;
        }
        //该词存在时，计算其权值
        queryword[*ivec] = querywordfrequence[*ivec] * log(doctid_ / denominator);
    }
    
    //求权值归一化的分母
    double denominator;
    map<string, double>::iterator witit;
    for(witit = queryword.begin(); witit != queryword.end(); ++ witit)
        denominator += witit->second * witit->second;
    denominator = sqrt(denominator);

    //查询文本的权值归一化
    for(witit = queryword.begin(); witit != queryword.end(); ++ witit)
        witit->second /= denominator;
    return 1;
}

void Query::display(size_t doctid)
{
    Document doct = libindex_[doctid];
    int fd_lib = open(libfile_.c_str(), O_RDONLY);
    if(fd_lib == -1)
        throw runtime_error("open lib file to read failed.");
    
    lseek(fd_lib, doct.offset_, SEEK_SET);
    char buffer[100000] = {0};
    read(fd_lib, buffer, doct.len_);
    cout << buffer << endl;
    close(fd_lib);
}

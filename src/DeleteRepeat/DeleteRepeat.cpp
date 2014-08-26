#include "DeleteRepeat.h"
#include "GetConfig.h"
#include "Tools.h"
#include "Rio.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <queue>
#include <stdexcept>
#include <vector>
#include <sstream>

using namespace std;

DeleteRepeat::DeleteRepeat(GetConfig *config)
    :segment_(tools::trimEnter(config->getDict_path()).c_str(), tools::trimEnter(config->getModel_path()).c_str()),
     libfile_(tools::trimEnter(config->getLibfile())),
     libindexfile_(tools::trimEnter(config->getLibIndexfile())),
     excludefile_(tools::trimEnter(config->getExcludefile())),
     doctid_(0)
{
}

void DeleteRepeat::start()
{
    readExcludeFile(); //第一步：读排除集文档
    readLibFile();  //第二步：读库文件
    countFrequence();  //第三步：计算词频
    makeFeatureValue();  //第四步：计算每篇文章的特征词
    deleteRepeat(); //第五步：删除重复的文档
    saveLib(); //第六步：将保留下来的文档重新写入网页库文件
}

//读排除集文档
void DeleteRepeat::readExcludeFile()
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
}

//读库文件
void DeleteRepeat::readLibFile()
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
        //rio_lib.readn(contentbuffer, document.len_);
        read(fd_lib, contentbuffer, document.len_);

        string doct(contentbuffer);
        string::size_type pos1, pos2;

        //取出网页的url
        pos1 = doct.find("<url>") + 5;
        pos2 = doct.find("</url>");
        document.url_ = doct.substr(pos1, pos2 - pos1);

        //取出网页的标题
        pos1 = doct.find("<title>") + 7;
        pos2 = doct.find("</title>");
        document.title_ = doct.substr(pos1, pos2 - pos1);

        //取出网页的内容
        pos1 = doct.find("<content>") + 9;
        pos2 = doct.find("</content>");
        document.content_ = doct.substr(pos1, pos2 - pos1);

        document_.push_back(document);
        doctid_ ++;
    }
    cout << "Read lib file over." << endl;
}

//统计词频
void DeleteRepeat::countFrequence()
{
    list<Document>::iterator listit; 
    for(listit = document_.begin(); listit != document_.end(); ++listit){

        //第一步：将英文的标点符号去掉，将首字母为大写字母的单词的首字母转换为小写字母
        string content = listit->content_;
        tools::deleteEnPunct(content);

        //第二步：分词
        vector<string>words;
        segment_.cut(content, words);

        //第三步：统计每一篇网页中的词频
        vector<string>::iterator it;
        unordered_set<string>::iterator setit;
        map<string, int> wordfre;
        for(it = words.begin(); it != words.end(); ++it){
            setit = exclude_.find(*it);
            if(setit == exclude_.end())
                wordfre[*it]++;
        }
        frequence_[listit->doctid_] = wordfre;
    }
    cout << "Count frequence over." << endl;
}

//生成每篇文章的特征词
void DeleteRepeat::makeFeatureValue()
{
    map<size_t, map<string, int> >::iterator freqit;//存放所有网页词频的数据结构的迭代器
    map<string, int>::iterator queueit; //存放一篇网页词频数据结构的迭代器
    //对每篇文章的词频数据进行遍历，找出其中的Top10词频
    for(freqit = frequence_.begin(); freqit != frequence_.end(); ++freqit){
        priority_queue<Word> queue;
        for(queueit = freqit->second.begin(); queueit != freqit->second.end(); ++queueit){
            Word word = {queueit->first, queueit->second};
            queue.push(word);
        }
        for(int i = 0; i < 10; ++i){
            Word word = queue.top();
            feature_[freqit->first].insert(make_pair(word.word_, word.frequence_));
            queue.pop();
        }
    }
    cout << "feature make over." << endl;
}

//网页去重
void DeleteRepeat::deleteRepeat()
{
    //第一步：统计哪些网页时重复的
    vector<int> flags(doctid_, 1); //记录哪些网页保存下来，哪些被删除的数据结构，是一个bitmap。
    map<size_t, map<string, int> >::size_type i, j; //记录所有网页词频的数据结构的迭代器
    for(i = 0; i != feature_.size(); ++i){
        if(flags[i] == 0)
            continue;
        for(j = i + 1; j != feature_.size(); ++j){
            if(flags[j] == 0)
                continue;
            if(isSimilar(feature_[i+1], feature_[j+1]))
                flags[j] = 0;
        }
    }
    cout << "similar count over." << endl;

    //第二步：网页去重运算
    vector<int>::size_type iv = 0;
    list<Document>::iterator doctit;
    for(doctit = document_.begin(); doctit != document_.end(); ++doctit){
        if(flags[iv++] == 0){
            document_.erase(doctit);
            doctit--;
        }
    }

    //第三步：对去过重的网页重新编号
    size_t doctid = 1;
    for(doctit = document_.begin(); doctit != document_.end(); ++doctit)
         doctit->doctid_ = (doctid ++);
    cout << "Delete repeate over." << endl;
}

//将保留下来的网页重新写入库文件
void DeleteRepeat::saveLib()
{   
    int lib_write, index_write;
    lib_write = open(libfile_.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0646);
    if(lib_write == -1)
        throw runtime_error("open lib file to write fail.");
    index_write = open(libindexfile_.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0646);
    if(index_write == -1)
        throw runtime_error("open index file to write fail.");

    Rio lib_rio(lib_write);
    Rio index_rio(index_write);

    cout << "Begin to write document to lib file." << endl;
    list<Document>::iterator it;
    for(it = document_.begin(); it != document_.end(); ++it){
        it->offset_ = lseek(lib_write, 0, SEEK_CUR);  //记下文件开头处的文件偏移量值
        string fmatedoc = tools::formateDocument(*it);
        it->len_ = fmatedoc.size();
        lib_rio.writen(fmatedoc.c_str(), it->len_);  //  将格式化后的网页内容写入文件

        //将文件索引信息同步保存到文件中
        ostringstream oss;
        oss << it->doctid_ << " " << it->offset_ << " " << it->len_ << endl;
        string index = oss.str();
        index_rio.writen(index.c_str(), index.size());
    } 
}

//网页相似度判断算法，此处用的是Top10
bool DeleteRepeat::isSimilar(std::map<std::string, int> &doc1, std::map<std::string, int> &doc2)
{
    int samewordnum = 0;
    map<string, int>::iterator mapi, mapj;
    for(mapi = doc1.begin(); mapi != doc1.end(); ++mapi){
        for(mapj = doc2.begin(); mapj != doc2.end(); ++mapj){
            if(mapi->first == mapj->first){
                samewordnum ++;
                break;
            }
        }
    }
    if(samewordnum > 6)
        return true;
    else
        return false;
}


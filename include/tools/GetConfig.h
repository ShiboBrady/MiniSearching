#ifndef GETCONFIG_H
#define GETCONFIG_H 

#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <string>

class GetConfig
{
    public:
        static GetConfig *getInstance()
        {
            pthread_once(&once_, &initInstance);
            return pInstance_;
        }

        static void initInstance()
        {
            ::atexit(&destroyInstance);
            pInstance_ = new GetConfig;
        }
          
        static void destroyInstance()
        {
            std::cout << "destroy" << std::endl;
            delete pInstance_;
        }

        void readConf(const std::string &filename);

        const std::string &getPagefilepath() const { return pagefilepath_; }
        const std::string &getPagefile() const { return pagefilepath_; }
        const std::string &getLibfile() const { return libfile_; }
        const std::string &getLibfileName() const { return libfilename_; }
        const std::string &getLibIndexfile() const { return libIndexfile_; }
        const std::string &getLibIndexfileName() const { return libIndexfilename_; }
        const std::string &getExcludefile() const { return excludefile_; }
        const std::string &getExcludefileName() const { return excludefilename_; }
        const std::string &getIndexfile() const { return indexfile_; }
        const std::string &getIndexfilename() const { return indexfilename_; }
        const std::string &getDict_path() const { return dict_path_; }
        const std::string &getModel_path() const { return model_path_; }

    private:
        GetConfig(){};
        GetConfig(const GetConfig &);
        void operator=(const GetConfig &);

        static GetConfig *pInstance_;
        static pthread_once_t once_;

        std::string pagefilepath_;
        std::string pagefile_;
        std::string libfile_;
        std::string libfilename_;
        std::string libIndexfile_;
        std::string libIndexfilename_;
        std::string excludefile_;
        std::string excludefilename_;
        std::string indexfile_;
        std::string indexfilename_;
        std::string dict_path_;
        std::string model_path_;
};

#endif  /*GETCONFIG_H*/

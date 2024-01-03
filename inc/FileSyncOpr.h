#pragma once

/*
* CurlOpr类使用libcurl创建下载任务，并通过共享内存的方式写文件
*/

#include <iostream>
#include <mutex>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <signal.h>

#include <curl/curl.h>
#include <curl/easy.h>
#include "MmapWriteFile.h"
#include "WorkDeque.h"

enum DownloadType {
    FTP,
    HTTP,
    OTHER
};

class CurlOpr {
public:
    CurlOpr() {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        m_pCurlIns = curl_easy_init();
        m_pMmapIns = MmapOpr::GetInstance();
    }

    ~CurlOpr() {
        m_pMmap = nullptr;
        m_pMmapIns = nullptr;
        //释放curl对象
        curl_easy_cleanup(m_pCurlIns);
        //释放全局curl对象
        curl_global_cleanup();
        m_pCurlIns = nullptr;
    }

    static int ProgressFunc(void *ptr, const double uTotalToDownload, const double uNowDownloaded, const double uTotalToUpLoad, const double uNowUpLoaded);
    static size_t WriteData(void *ptr, size_t size, size_t nmemb, void *stream);

    double GetDownloadFileLenth (const std::string& strUrl);     
    bool CurlDownLoadFile(FileNode* pNode, const std::string& strUrl);
    bool CurlDownLoadFile(FileNode* pNode, const std::string& strPwd, const std::string& strUrl, const std::string& strUserName);
private:
    // 共享内存地址
    char *m_pMmap = nullptr;

    // 保存curl对象指针
    CURL *m_pCurlIns = nullptr;

    // 保存MmapOpr单例对象指针
    MmapOpr * m_pMmapIns = nullptr;
};
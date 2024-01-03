#include <stdio.h>
#include "curl/curl.h"
#include <future>
#include <mutex>
#include "FileSyncOpr.h"
#include "MmapWriteFile.h"
#include "WorkDeque.h"

/*
* 线程池类ThreadPool
*/

// 用来记录线程个数
long long g_dDownFileThreadSize = 0;

class ThreadPool {
public:
    ThreadPool(const std::string &strPwd, const std::string& strUrl, const std::string& strUserName) {
        m_strPwd = strPwd;
        m_strUrl = strUrl; 
        m_strUserName = strUserName; 
        m_threads.resize(g_dDownFileThreadSize);
    }

    void init(unsigned int uiFileSize) {
        long uPartSize = uiFileSize / g_dDownFileThreadSize;

        for (int i = 0; i < m_threads.size(); ++i) {
            FileNode *pNode = new FileNode();

            if (i < (g_dDownFileThreadSize - 1)) {
                pNode->m_iStartPos = i * uPartSize;
                pNode->m_iEndPos = (i + 1) * uPartSize - 1;
            } else {
                pNode->m_iStartPos = i * uPartSize;
                pNode->m_iEndPos = uiFileSize - 1;
            }

            pNode->m_pFp = MmapOpr::GetInstance()->GetMmapFd();
            m_queue.DequeBackPush(pNode);
        }

        for (int i = 0; i < m_threads.size(); ++i) {
            m_threads[i] = std::thread(WorkerThread(this));
        }

        
    }

    void shutdown() {
        m_shutdown = true;
        std::cout << "shutdown" << std::endl;
        for (int i = 0; i < m_threads.size(); ++i) {
            if (m_threads[i].joinable()) {
                m_threads[i].join();
            }
        }
        
        std::mutex mutex;
        std::unique_lock<std::mutex> unique_locker(mutex);
        --g_dDownFileThreadSize;
    }

    void submit(FileNode* pNode){
        m_queue.DequeFrontPush(pNode);
        return;
    }

    std::string m_strPwd;
    std::string m_strUrl;
    std::string m_strUserName;
    WorkDeque m_queue;
    bool m_shutdown = false;

private:
    class WorkerThread {
    public:
        WorkerThread(ThreadPool *pool) : m_pool(pool) { }

        // 仿函数用来下载文件内容
        void operator()() {
            FileNode* pFileNode = nullptr;
            while (!m_pool->m_shutdown) {
                if (m_pool->m_queue.DequePop(pFileNode)) {
                    if (pFileNode == nullptr) {
                        continue;
                    }
                    
                    CurlOpr curlIns;
                    // 如果下载失败，则继续放入队列，由线程抢占式处理
                    unsigned int uTmpStartPos = pFileNode->m_iStartPos;
                    if (!curlIns.CurlDownLoadFile(pFileNode, m_pool->m_strUrl)) {
                        pFileNode->m_iStartPos = uTmpStartPos;
                        m_pool->submit(pFileNode);
                    } else {
                        delete pFileNode;
                        pFileNode = nullptr;

                        std::mutex mutex;
                        std::unique_lock<std::mutex> unique_locker(mutex);
                        --g_dDownFileThreadSize;
                        break;
                    }
                }

                // 判断是否已经全部下载完成
                if (g_dDownFileThreadSize <= 0) {
                    m_pool->shutdown();
                }
            }

            return;
        }

    private:
        ThreadPool *m_pool = nullptr;
    };

    std::vector<std::thread> m_threads;
}; 

int main()
{
    std::string strPwd;
    std::string strUserName;
    std::string strFileUrl;
    std::string strFileName;

    std::cout << "请输入您需要的线程数:" << std::endl;
    std::cin >> g_dDownFileThreadSize;
    std::cout << "请输入您需要下载的文件链接:" << std::endl;
    std::cin >> strFileUrl;
    std::cout << "请输入您想保存的位置(包括文件名):" << std::endl;
    std::cin >> strFileName;

    CurlOpr curlIns;
    long uFileSize = curlIns.GetDownloadFileLenth(strFileUrl);
    std::cout << "file length : " << uFileSize << std::endl;

    MmapOpr* pMmapIns = MmapOpr::GetInstance();
    pMmapIns->MmapInit(strFileName, uFileSize);

    ThreadPool threadPoolIns(strPwd, strFileUrl, strUserName);
    threadPoolIns.init(uFileSize);
    while (g_dDownFileThreadSize >= 0) {
		usleep (1000000000L);
	}

    pMmapIns->MmapRelease();
    return 0;
}
/*
* CurlOpr类使用libcurl创建下载任务，并通过共享内存的方式写文件
*/

#include "MmapWriteFile.h"
#include "FileSyncOpr.h"

int CurlOpr::ProgressFunc(void *ptr, const double uTotalToDownload, const double uNowDownloaded, const double uTotalToUpLoad, const double uNowUpLoaded) {
	int iPercent = 0;
	if (uTotalToDownload > 0) {
		iPercent = static_cast<int>(uNowDownloaded / uTotalToDownload * 100);
	}

    if((iPercent % 20) == 0) {
        std::cout << "下载进度" << iPercent << "%" << std::endl;
    }

	return 0;
}

size_t CurlOpr::WriteData(void *ptr, size_t size, size_t nmemb, void *stream) {
    FileNode* pNode = static_cast<FileNode*>(stream);
    if ((pNode == nullptr) || (pNode->m_pFp == nullptr)) {
        std::cout << "pNode | m_pFp = nullptr" << std::endl;
        return 0;
    }

    size_t iWriteLen = 0;
    pNode->m_pFp += pNode->m_iStartPos;
    if (pNode->m_iStartPos + size * nmemb <= pNode->m_iEndPos) {
        memcpy((char*)pNode->m_pFp, ptr, size * nmemb);
        iWriteLen = size * nmemb;
	} else {
        memcpy((char*)pNode->m_pFp, ptr, pNode->m_iEndPos - pNode->m_iStartPos + 1);
        iWriteLen = pNode->m_iEndPos - pNode->m_iStartPos + 1;
	}

    return iWriteLen;
}

bool CurlOpr::CurlDownLoadFile(FileNode* pNode, const std::string& strUrl) {
    if (m_pMmapIns == nullptr) {
        std::cout << "exec MmapOpr::GetInstance fail!" << std::endl;
        return false;
    }

    curl_easy_setopt (m_pCurlIns, CURLOPT_URL, strUrl.c_str ());
    curl_easy_setopt (m_pCurlIns, CURLOPT_WRITEFUNCTION, WriteData);
    curl_easy_setopt (m_pCurlIns, CURLOPT_WRITEDATA, static_cast<void*>(pNode));
    curl_easy_setopt (m_pCurlIns, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt (m_pCurlIns, CURLOPT_PROGRESSFUNCTION, ProgressFunc);
    curl_easy_setopt (m_pCurlIns, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt (m_pCurlIns, CURLOPT_LOW_SPEED_LIMIT, 1L);
    curl_easy_setopt (m_pCurlIns, CURLOPT_LOW_SPEED_TIME, 5L);
    std::string strRange = std::to_string(pNode->m_iStartPos) + '-' + std::to_string(pNode->m_iEndPos);
    std::cout << "strRange :" << strRange << std::endl;
    curl_easy_setopt(m_pCurlIns, CURLOPT_RANGE, strRange.c_str());

    if (curl_easy_perform(m_pCurlIns) != 0) {
        return false;
    }

    return true;
}

bool CurlOpr::CurlDownLoadFile(FileNode* pNode, const std::string& strPwd, const std::string& strUrl, const std::string& strUserName) {
    if (m_pMmapIns == nullptr) {
        std::cout << "exec MmapOpr::GetInstance fail!" << std::endl;
        return false;
    }

    if (m_pCurlIns == nullptr) {
        std::cout << "exec curl_easy_init fail!" << std::endl;
        return false;
    }

    std::string strRange = std::to_string(pNode->m_iStartPos) + '-' + std::to_string(pNode->m_iEndPos);
    curl_easy_setopt(m_pCurlIns, CURLOPT_URL, strUrl.c_str());
    curl_easy_setopt(m_pCurlIns, CURLOPT_USERNAME, strUserName.c_str());
    curl_easy_setopt(m_pCurlIns, CURLOPT_PASSWORD, strPwd.c_str());
    curl_easy_setopt(m_pCurlIns, CURLOPT_WRITEFUNCTION, WriteData);
    curl_easy_setopt(m_pCurlIns, CURLOPT_WRITEDATA, pNode);
    curl_easy_setopt(m_pCurlIns, CURLOPT_RANGE, strRange.c_str());
    
    CURLcode curl_code = curl_easy_perform(m_pCurlIns);
    if (CURLE_OK != curl_code) {
        std::cout << "exec curl_easy_perform fail!" << std::endl;
        return false;
    }

    return true;
}

double CurlOpr::GetDownloadFileLenth (const std::string& strUrl) {
    double uDownloadFileLenth = 0;
    curl_easy_setopt (m_pCurlIns, CURLOPT_URL, strUrl.c_str());
    // 只需要header头
    curl_easy_setopt (m_pCurlIns, CURLOPT_HEADER, 1);
    // 不需要body
    curl_easy_setopt (m_pCurlIns, CURLOPT_NOBODY, 1);
    if (curl_easy_perform (m_pCurlIns) == CURLE_OK) {
        curl_easy_getinfo (m_pCurlIns, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &uDownloadFileLenth);
    } else {
        uDownloadFileLenth = -1;
    }

    return uDownloadFileLenth;
}
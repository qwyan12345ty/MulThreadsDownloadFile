#include "MmapWriteFile.h"

MmapOpr* MmapOpr::m_pMmapIns = nullptr;

bool MmapOpr::MmapInit(const std::string& strFileName, long uFileSize) {
    m_uFileSize = uFileSize;
    // 创建文件
    int iDstFd = open(strFileName.c_str(), O_RDWR |O_CREAT |O_TRUNC,0664);
    if(iDstFd < 0) {
        std::cout << "open file fail!" << std::endl;
        return false;
    }

    // 根据文件大小拓展目标文件。
    int iRet = ftruncate(iDstFd, uFileSize);
    if(iRet < 0) {
        std::cout << "ftruncate fail! " << strerror(errno) << std::endl;;
        close(iDstFd);
        return false;
    }

    // 为源文件创建映射。
    m_pMmap = static_cast<char*>(mmap(nullptr, uFileSize, PROT_READ | PROT_WRITE, MAP_SHARED, iDstFd, 0)); 
    if(m_pMmap == MAP_FAILED) {
        close(iDstFd);
        std::cout << "exec mmapfail! " << strerror(errno) << std::endl;;
        return false;
    }

    close(iDstFd);
    return true;
}

MmapOpr* MmapOpr::GetInstance() {
    if (m_pMmapIns == nullptr) {
        m_pMmapIns = new MmapOpr;
    }

    return m_pMmapIns;
}

void MmapOpr::MmapRelease() {
    if (m_pMmap != nullptr) {
        munmap(m_pMmap, m_uFileSize);
        m_pMmap = nullptr;
    }
}

char* MmapOpr::GetMmapFd() {
    return m_pMmap;
}
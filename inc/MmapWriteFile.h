#pragma once

/*
* MmapOpr用来创建目标文件，并对目标文件创建共享内存, 单例类
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <iostream>

class MmapOpr {
public:
    bool MmapInit(const std::string& strFileName, long uFileSize);
    static MmapOpr* GetInstance();

    void MmapRelease();

    char* GetMmapFd();

    MmapOpr(MmapOpr&&) = delete;
	MmapOpr(const MmapOpr&) = delete;
	void operator=(const MmapOpr&) = delete;
protected:
    MmapOpr() = default;
	virtual ~MmapOpr() = default;

private:
    long m_uFileSize = 0;
    char *m_pMmap = nullptr;
    static MmapOpr* m_pMmapIns;
};
#pragma once

/*
* WorkDeque用来创建一个线程安全的队列用来保存文件下载下标
*/

#include <iostream>
#include <deque>
#include <mutex>
#include <vector>

struct FileNode {
	char *m_pFp = nullptr;
	long m_iStartPos = 0;
	long m_iEndPos = 0;;
};

class WorkDeque {
public:
    WorkDeque() = default;
    ~WorkDeque() = default;

    // 判断队列是否为空
    bool Empty();

    // 获取队列大小
    int WorkDequeSize();

    // 从队尾插入任务下标
    void DequeBackPush(FileNode* pNode);

    // 从队头插入任务下标
    void DequeFrontPush();

    // 从队尾插入任务下标
    void DequeFrontPush(FileNode* pNode);

    // 从队头取出任务下标
    bool DequePop(FileNode*& pNode);
private:
    // 互斥锁
    std::mutex m_mutex;

    // 双向队列存储任务下标
    std::deque<FileNode*> m_WorkDeque;
};
/*
* WorkDeque用来创建一个线程安全的队列用来保存文件下载下标
*/

#include "WorkDeque.h"

// 判断队列是否为空
bool WorkDeque::Empty() {
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_WorkDeque.empty();
}

// 获取队列大小
int WorkDeque::WorkDequeSize() {
    std::unique_lock<std::mutex> lock(m_mutex);

    return m_WorkDeque.size();
}

// 从队尾插入任务下标
void WorkDeque::DequeBackPush(FileNode* pNode) {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_WorkDeque.push_back(pNode);
}

// 从队头插入任务下标
void WorkDeque::DequeFrontPush(FileNode* pNode) {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_WorkDeque.push_front(pNode);
}

// 从队头取出任务下标
bool WorkDeque::DequePop(FileNode*& pNode) {
    if (m_WorkDeque.empty()) {
        return false;
    }

    std::unique_lock<std::mutex> lock(m_mutex);
    pNode = m_WorkDeque.front();
    m_WorkDeque.pop_front();
    return true;
}
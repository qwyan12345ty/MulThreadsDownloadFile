1, 该工程共有5个类，WorkDeque， CurlOpr，MmapOpr， ThreadPool，WorkerThread 
   工作队列类WorkDeque，使用互斥锁实现一个线程安全的工作队列
   curl工作类CurlOpr，对curlAPI的封装
   共享内存类MmapOpr，单例对目标文件进行共享映射处理
   ThreadPool线程类，实现线程创建
   WorkerThread工作线程类，实现线程下载文件的功能

2，大概流程
   先写队列任务，后创建线程任务并运行
   每次都通过CURLOPT_RANGE属性对下载文件的区间下载，实现下载内容的不重复
   写文件采用内存映射的方式实现高效写，
   通过工作队列的方式实现下载失败重新下载

3, 文件结构
.
├── bin
├── inc
│   ├── FileSyncOpr.h
│   ├── MmapWriteFile.h
│   └── WorkDeque.h
├── Makefile
├── README.txt
└── src
    ├── FileSyncOpr.cpp
    ├── main.cpp
    ├── MmapWriteFile.cpp
    └── WorkDeque.cpp
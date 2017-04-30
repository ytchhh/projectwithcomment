Code With Comments
关于C和C++的一些小项目 带注释
=========

### Tinyhttpd
介绍：tinyhttpd 是一个不到 500 行的超轻量型 Http Server，非常适合学习服务器工作原理

源码分析：[在此](http://blog.csdn.net/programvae/article/details/71036295)

链接：[tinyhttpd](https://github.com/yangtingcheng/tinyhttpd)

### MemoryPool
介绍：一个用 C++ 实现的简单内存池

源码分析：[在此](http://blog.csdn.net/jcjc918/article/details/44997089)

链接：[MemoryPool(98 版本)](https://github.com/AngryHacker/code-with-comments/tree/master/memorypool/C-98)

### Webbench
介绍：Webbench是一个在linux下使用的非常简单的网站压测工具。它使用fork()模拟多个客户端同时访问我们设定的URL，
      测试网站在压力下工作的性能，最多可以模拟3万个并发连接去测试网站的负载能力。

源码分析：[在此](http://blog.csdn.net/jcjc918/article/details/44965951)

链接：[webbench](https://github.com/AngryHacker/code-with-comments/tree/master/webbench)

### Threadpool
介绍：基于 pthread 实现的简单线程池

源码分析：[在此](http://blog.csdn.net/jcjc918/article/details/50395528)

链接：[Threadpool](https://github.com/AngryHacker/code-with-comments/tree/master/threadpool)

### Leveldb
介绍：Jeff Dean 和 Sanjay Ghemawat 两位超级大神实现的高效 kv 数据库。

源码分析：

* [LevelDb 日知录](http://www.cnblogs.com/haippy/archive/2011/12/04/2276064.html)
* [leveldb 实现解析 - 那岩](https://github.com/AngryHacker/code-with-comments/blob/master/attachment/leveldb%E5%AE%9E%E7%8E%B0%E8%A7%A3%E6%9E%90.pdf)

以上是我觉得分析得最好的两篇。不过随着 leveldb 项目的推进，有些代码也发生了变动。我进行注释的源码是最新的 1.18 版本，结合两篇解析写自己的理解，目前已阅读的是主要流程，水平有限，有些地方也不甚清楚，欢迎交流。

链接：[Leveldb](https://github.com/AngryHacker/code-with-comments/tree/master/leveldb)


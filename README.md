# cweb
HTTP server implemented with LIBEVENT, integrated with Glog, Berkeley DB, Jansson JSON

本项目的目的是创建一个简单的C/C++ 工程项目。
主程序使用Libevent2带的demo实现的一个http server，同时整合了常用的开源工具，当前版本用到了：
1 glog:日志工具
2 Berkeley db:数据库读写
3 Jansson: json生成和解析

以上开源库从官方下载，使用cmake生成工程，然后vc2017编译，在上传的代码中已经包含编译好的lib和dll。

# cweb
HTTP server implemented with LIBEVENT, integrated with Glog, Berkeley DB, Jansson JSON

本项目的目的是创建一个简单的C/C++ 工程项目。
主程序使用Libevent2带的demo实现的一个http server，同时整合了常用的开源工具，当前版本用到了：

1 glog:日志工具

2 Berkeley db:数据库读写

3 Jansson: json生成和解析

4 集成paho.mqtt.c,实现mqtt客户端功能。

5 集成hiredis，实现redis 客户端功能。

以上开源库从官方下载，使用cmake生成工程，然后vc2017编译，在上传的代码中已经包含编译好的lib和dll。

下一步计划：
1 集成libcurl，实现ftp,http,smtp client功能

2 集成openssl，实现AES, Hash, Base64等常用加密解密编码功能。


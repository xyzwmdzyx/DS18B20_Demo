# DS18B20_Demo

## 1 项目描述

树莓派上通过一线协议（1-Wire）链接DS18B20，然后采用网络Socket编程同时实现客户端和服务器端程序。其中客户端主要实现定时上报功能，服务器端则采集客户端上报的数据并永久储存到数据库中。

**客户端**

1. 能够通过命令行参数指定服务器的`IP`地址与端口号；
2. 每三十分钟采样并上传一次数据给服务器，该时间也可以通过命令行参数来调整；
3. 客户端上传的数据包含设备序列号、采样时间、采样温度值，且以字符串的形式上报；
4. 如果网络`Socket`异常（如网络断线、服务器端退出等），在网络故障恢复后客户端能自动重连；
5. 在网络`Socket`异常期间，定时采样正常进行，且在这期间所有的采样数据临时储存到`SQLite`数据库中；
6. 网络`Socket`异常恢复后，客户端程序自动将之前暂存到数据库中的数据上传给服务器，并从本地数据库中删除这些数据。

**服务器端**

1. 能够通过命令行参数指定监听的端口号；
2. 采用多路复用`epoll`机制实现多客户端并发上报的数据接收功能；
3. 服务器端接收客户端上报的数据并解析成功后，将该数据永久储存到SQLite数据库中。

## 2 安装项目

1. 使用`git clone`命令将项目源码下载到本地：
```c
git clone git@github.com:xyzwmdzyx/DS18B20_Demo.git
```

2. 使用` cd`命令分别进入Client、Server文件夹，并使用`make`命令分别编译Client、Server：
```c
cd ./Client
make
```
```c
cd ./Server
make
```

3. 在Client、Server文件夹下分别使用`make install`命令安装Client、Server：
```c
make install
```


## 3 运行项目

1. 在Client、Server文件夹下分别使用`./Start.sh`命令运行Client、Server。Client可在Start.sh脚本中配置服务器IP地址、端口号、读取温度的时间间隔；Server可在Start.sh脚本中配置端口号。
```c
./Start.sh
```

## 4 清空项目日志等数据及卸载项目

1. 在Client、Server文件夹下分别使用`make distclean`命令清除程序运行产生的数据库文件与日志文件：
```c
make distclean
```
2. 在Client、Server文件夹下分别使用`make unisntall`命令卸载程序：
```c
make uninstall
```
## 5 作者联系方式

如有项目相关的问题，可发邮件到作者邮箱：programauthor@gmail.com
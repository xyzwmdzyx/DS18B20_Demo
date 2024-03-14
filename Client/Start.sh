#!/bin/bash

# -i 代表服务器IP地址 -p 代表服务器端口号 -t 代表读取温度的时间间隔
PARSE="-i 127.0.0.1 -p 8888 -t 1"

SCRIPT_PATH=$(readlink -f "$0")
SCRIPT_DIR=$(dirname "$SCRIPT_PATH")

# 设置环境变量
LD_LIBRARY_PATH=$SCRIPT_DIR/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH

# 启动服务器程序
./bin/Client $PARSE

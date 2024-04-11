#!/bin/bash

# -p 代表服务器端口号
PARSE="-p 8888"

SCRIPT_PATH=$(readlink -f "$0")
SCRIPT_DIR=$(dirname "$SCRIPT_PATH")

# 设置环境变量
LD_LIBRARY_PATH=$SCRIPT_DIR/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH

# 启动服务器程序
./bin/Server $PARSE

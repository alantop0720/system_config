#!/bin/bash
# 定义监听的端口号
port=10266
# 创建一个简单的 server，监听指定的端口，并打印接收到的信息
while true; do
nc -l $port | while IFS= read -r line; do
echo "$line"
done
done

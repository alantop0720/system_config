#!/bin/bash

# 查找所有 Java 进程
java_pids=$(ps aux | grep -v grep | grep java | awk '{print $2}')

# 检查是否有 Java 进程
if [ -z "$java_pids" ]; then
  echo "没有找到 Java 进程。"
else
  echo "找到的 Java 进程 PID：$java_pids"

  # 杀死所有 Java 进程
  for pid in $java_pids; do
    echo "正在杀死进程：$pid"
    kill -9 $pid
  done

  echo "所有 Java 进程已杀死。"
fi
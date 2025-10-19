#!/bin/bash

# 提取并统计lastb中的IP失败次数
ips=()
while read -r line; do
    # 跳过表头和空行
    if [[ $line == *"Username"* || $line == *"IP"* || -z "$line" ]]; then
        continue
    fi
    
    # 提取IP地址（支持IPv4和IPv6）
    ip=$(echo "$line" | grep -oE '\b([0-9]{1,3}\.){3}[0-9]{1,3}\b|\b([0-9a-fA-F]{1,4}:){7}[0-9a-fA-F]{1,4}\b')
    
    # 只添加非空IP
    if [[ -n "$ip" ]]; then
        ips+=("$ip")
    fi
done < <(lastb)

# 统计IP出现次数并排序
echo "IP地址\t\t登录失败次数"
echo "-------------------------"
printf "%s\n" "${ips[@]}" | sort | uniq -c | sort -nr | awk '{printf "%-15s\t%d\n", $2, $1}'

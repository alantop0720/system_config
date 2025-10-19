#!/usr/bin/env python3
import subprocess
import re
from collections import Counter

def get_lastb_output():
    """获取lastb命令的输出"""
    try:
        result = subprocess.run(['lastb'], capture_output=True, text=True, check=True)
        return result.stdout
    except subprocess.CalledProcessError as e:
        print(f"执行lastb命令失败: {e.stderr}")
        return ""
    except Exception as e:
        print(f"发生错误: {e}")
        return ""

def extract_ips(lastb_output):
    """从lastb输出中提取有效的IP地址"""
    # 跳过表头行
    lines = lastb_output.splitlines()[1:] if lastb_output else []
    
    # 正则表达式匹配IPv4和IPv6地址
    ip_pattern = re.compile(
        r'\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\b'  # IPv4
        r'|\b(?:[0-9a-fA-F]{1,4}:){7}[0-9a-fA-F]{1,4}\b'  # IPv6 完整形式
        r'|\b(?:[0-9a-fA-F]{1,4}:){0,6}[0-9a-fA-F]{1,4}:(?:[0-9a-fA-F]{1,4}:){0,6}[0-9a-fA-F]{1,4}\b'  # IPv6 压缩形式
    )
    
    ips = []
    for line in lines:
        # 跳过空行
        if not line.strip():
            continue
            
        # 从每行中提取IP
        match = ip_pattern.search(line)
        if match:
            ips.append(match.group())
    
    return ips

def count_and_report(ips):
    """统计IP出现次数并生成报告"""
    if not ips:
        print("未发现登录失败记录或无法提取IP地址")
        return
    
    # 统计IP出现次数
    ip_counts = Counter(ips)
    
    # 按次数降序排序
    sorted_ips = sorted(ip_counts.items(), key=lambda x: x[1], reverse=True)
    
    # 计算最长IP长度用于对齐
    max_ip_length = max(len(ip) for ip, _ in sorted_ips) if sorted_ips else 0
    
    # 打印表头
    print(f"{'IP地址':<{max_ip_length}}\t登录失败次数")
    print("-" * (max_ip_length + 20))
    
    # 打印统计结果
    for ip, count in sorted_ips:
        print(f"{ip:<{max_ip_length}}\t{count}")
    
    # 打印总计
    print(f"\n总计: {len(sorted_ips)} 个不同IP尝试登录，共{sum(ip_counts.values())}次失败")

if __name__ == "__main__":
    lastb_output = get_lastb_output()
    ips = extract_ips(lastb_output)
    count_and_report(ips)

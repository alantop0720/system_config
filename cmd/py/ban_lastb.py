#!/usr/bin/env python3
import subprocess
import re
from collections import Counter
import sys
import os

def get_lastb_output():
    """获取lastb命令的输出"""
    try:
        result = subprocess.run(['lastb'], capture_output=True, text=True, check=True)
        return result.stdout
    except subprocess.CalledProcessError as e:
        print(f"执行lastb命令失败: {e.stderr}", file=sys.stderr)
        return ""
    except Exception as e:
        print(f"发生错误: {e}", file=sys.stderr)
        return ""

def extract_ips(lastb_output):
    """从lastb输出中提取有效的IP地址"""
    # 跳过表头行
    lines = lastb_output.splitlines()[1:] if lastb_output else []
    
    # 正则表达式匹配IPv4地址
    ipv4_pattern = re.compile(r'\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\b')
    
    ips = []
    for line in lines:
        # 跳过空行
        if not line.strip():
            continue
            
        # 从每行中提取IPv4地址
        match = ipv4_pattern.search(line)
        if match:
            ips.append(match.group())
    
    return ips

def count_ips(ips):
    """统计IP出现次数"""
    if not ips:
        return {}
    
    # 统计IP出现次数
    ip_counts = Counter(ips)
    
    # 返回出现次数>=10的IP
    return {ip: count for ip, count in ip_counts.items() if count >= 10}

def block_ips(ip_counts):
    """使用firewall-cmd阻止指定IP"""
    if not ip_counts:
        print("没有需要阻止的IP地址")
        return 0, []
    
    blocked_ips = []
    failed_ips = []
    
    print("\n即将阻止以下IP地址:")
    for ip, count in sorted(ip_counts.items(), key=lambda x: x[1], reverse=True):
        print(f"  - {ip} (失败次数: {count})")
    
    
    # 执行防火墙命令
    for ip in ip_counts:
        try:
            # 添加永久规则
            cmd = [
                'firewall-cmd', 
                '--permanent', 
                '--add-rich-rule', 
                f'rule family=ipv4 source address="{ip}" drop'
            ]
            subprocess.run(cmd, check=True, capture_output=True)
            blocked_ips.append(ip)
        except subprocess.CalledProcessError as e:
            print(f"阻止IP {ip} 失败: {e.stderr}", file=sys.stderr)
            failed_ips.append(ip)
    
    # 重新加载防火墙
    if blocked_ips:
        try:
            subprocess.run(['firewall-cmd', '--reload'], check=True, capture_output=True)
            print("\n防火墙规则已成功重新加载")
        except subprocess.CalledProcessError as e:
            print(f"重新加载防火墙失败: {e.stderr}", file=sys.stderr)
            failed_ips.extend(blocked_ips)
            blocked_ips = []
    
    return len(blocked_ips), failed_ips

def main():
    # 检查是否有root权限
    if os.geteuid() != 0:
        print("此脚本需要root权限才能运行", file=sys.stderr)
        sys.exit(1)
    
    print("正在分析登录失败记录...")
    
    # 获取并处理lastb输出
    lastb_output = get_lastb_output()
    ips = extract_ips(lastb_output)
    ip_counts = count_ips(ips)
    
    if not ip_counts:
        print("没有发现失败次数超过10次的IP地址")
        sys.exit(0)
    
    # 阻止IP
    blocked_count, failed_ips = block_ips(ip_counts)
    
    # 输出结果
    print("\n==== 操作结果 ====")
    print(f"成功阻止: {blocked_count} 个IP")
    if failed_ips:
        print(f"阻止失败: {len(failed_ips)} 个IP")
        for ip in failed_ips:
            print(f"  - {ip}")
    

if __name__ == "__main__":
    main()

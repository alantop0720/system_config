#!/usr/bin/env python3
import sys
import os
import subprocess


class CommandInfo:
    def __init__(self, description="", flag=0, command=""):
        self.description = description
        self.flag = flag
        self.command = command


def replace_bracketed_content(input_str):
    output = input_str
    start_pos = 0
    while True:
        start_pos = output.find('[', start_pos)
        if start_pos == -1:
            break
        end_pos = output.find(']', start_pos)
        if end_pos == -1:
            break
        key = output[start_pos + 1:end_pos]
        value = input("请输入 {}: ".format(key))
        output = output[:start_pos] + value + output[end_pos + 1:]
        start_pos = start_pos + len(value)
    return output


def display_command_with_newlines(command):
    i = 0
    while i < len(command) - 1:
        if command[i] == '\\' and command[i + 1] == 'r':
            print()
            i += 2  # 跳过 '\r'
        else:
            print(command[i], end='')
            i += 1
    print(command[-1])  # 输出最后一个字符


def main():
    if len(sys.argv) == 1:
        try:
            with open("/home/cmd/readme", "r") as file:
                for line in file:
                    print(line.rstrip())
            subprocess.run(["ls", "/home/cmd"])
        except FileNotFoundError:
            print("无法打开文件readme", file=sys.stderr)
            return 1
        return 0
    
    arg_name = sys.argv[1]
    file_path = "/home/cmd/" + arg_name
    command_list = []
    
    try:
        with open(file_path, "r") as file:
            line_count = 1
            info = CommandInfo()
            sequence_number = 1
            
            for line in file:
                line = line.rstrip()
                if line_count == 1:
                    info.description = line
                elif line_count == 2:
                    info.flag = int(line)
                elif line_count == 3:
                    info.command = line
                    command_list.append(info)
                    print("{}. {}".format(sequence_number, info.description))
                    sequence_number += 1
                    line_count = 0
                    info = CommandInfo()
                line_count += 1
    except FileNotFoundError:
        print("无法打开文件", file=sys.stderr)
        return 1
    
    try:
        choice = int(input("请输入要执行的序列号: "))
    except ValueError:
        print("无效的序列号")
        return 1
    
    if 1 <= choice <= len(command_list):
        selected_command = command_list[choice - 1]
        if selected_command.flag == 1:
            print(selected_command.command)
            subprocess.run(selected_command.command, shell=True)
        else:
            display_command_with_newlines(selected_command.command)
            output = replace_bracketed_content(selected_command.command)
            print(output)
            subprocess.run(output, shell=True)
    else:
        print("无效的序列号")


if __name__ == "__main__":
    main()
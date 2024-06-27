#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <string>
#include <unistd.h>
#include <limits.h>

// 定义一个结构体来存储每行的信息
struct CommandInfo {
    std::string description;
    int flag;
    std::string command;
};

void displayCommandWithNewlines(const std::string& command) {
    for (size_t i = 0; i < command.size() - 1; ++i) {
        if (command[i] == '\\' && command[i + 1] == 'r') {
            std::cout << std::endl;
            ++i;  // 跳过第二个 '\r'
        } else {
            std::cout << command[i];
        }
    }
    std::cout << command.back() << std::endl;  // 输出最后一个字符
}

int main(int argc, char *argv[]) {
    if (argc==1)
    {
       std::cout << "缺少参数. cmd fhq "  << std::endl;
       return 1 ;
    }
    char buff[PATH_MAX];
    std::string argName = argv[1];
    std::string output =  "/home/cmd/" +  argName ;
    std::list<CommandInfo> commandList;
    std::ifstream file(output);  // 替换为实际的文件名

    if (!file.is_open()) {
        std::cerr << "无法打开文件" << std::endl;
        return 1;
    }

    std::string line;
    int lineCount = 1;
    CommandInfo info;
    int sequenceNumber = 1;  // 序列号

    while (std::getline(file, line)) {
        switch (lineCount) {
            case 1:
                info.description = line;
                break;
            case 2:
                info.flag = std::stoi(line);  // 将字符串转换为整数
                break;
            case 3:
                info.command = line;
                commandList.push_back(info);
                std::cout << sequenceNumber << ". " << info.description << std::endl;
                sequenceNumber++;
                lineCount = 0;
                break;
        }
        lineCount++;
    }

    file.close();

    int choice;
    std::cout << "请输入要执行的序列号: ";
    std::cin >> choice;

    auto it = commandList.begin();
    std::advance(it, choice - 1);  // 将迭代器移动到用户选择的位置

    if (choice == 0 && it!= commandList.end()) {
        displayCommandWithNewlines(it->command);
    } else if (it!= commandList.end()) {
        if (it->flag == 1) {
            std::system(it->command.c_str());  // 在 Linux 下执行命令
        } else {
            std::cout << it->command << std::endl;  // 在屏幕上显示命令
        }
    } else {
        std::cout << "无效的序列号" << std::endl;
    }

    return 0;
}

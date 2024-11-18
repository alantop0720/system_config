#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <sstream>
#include <windows.h>





// 定义一个结构体来存储每行的信息
struct CommandInfo
{
    std::string description;
    int flag;
    std::string command;
};

// 替换方括号内内容的函数，修改为可处理多个方括号
std::string replaceBracketedContent(const std::string& input) {
    std::string output = input;
    size_t startPos = 0;
    while ((startPos = output.find('[', startPos)) != std::string::npos) {
        size_t endPos = output.find(']', startPos);
        if (endPos == std::string::npos) {
            break;
        }
        std::string key = output.substr(startPos + 1, endPos - startPos - 1);
        std::cout << "请输入 " << key << ": ";
        std::string value;
        std::cin >> value;
        output.replace(startPos, endPos - startPos + 1, value);
        startPos = endPos + value.size();
    }
    return output;
}



void displayCommandWithNewlines(const std::string& command)
{
    for (size_t i = 0; i < command.size() - 1; ++i)
    {
        if (command[i] == '\\' && command[i + 1] == 'r')
        {
            std::cout << std::endl;
            ++i; // 跳过第二个 '\r'
        }
        else
        {
            std::cout << command[i];
        }
    }
    std::cout << command.back() << std::endl; // 输出最后一个字符
}



int main(int argc, char* argv[])
{
    std::string pre_path;

    wchar_t executablePath[MAX_PATH];
    GetModuleFileNameW(NULL, executablePath, MAX_PATH);

    // 将获取到的宽字符路径转换为std::string类型以便后续处理
    int bufferSize = WideCharToMultiByte(CP_ACP, 0, executablePath, -1, NULL, 0, NULL, NULL);
    std::string executablePathStr(bufferSize, '\0');
    WideCharToMultiByte(CP_ACP, 0, executablePath, -1, &executablePathStr[0], bufferSize, NULL, NULL);

    //... 后续代码保持不变，继续处理路径相关操作
    // 找到最后一个路径分隔符的位置（在Windows下是'\\'）
    size_t lastSlashPos = executablePathStr.find_last_of("\\");
    if (lastSlashPos != std::string::npos) {
        // 截取到路径分隔符之前的部分，得到运行程序所在目录
        std::string currentDir = executablePathStr.substr(0, lastSlashPos);
        pre_path = currentDir;
        std::cout << "运行程序所在目录: " << currentDir << std::endl;

        //std::string dir = "dir /p /w " + currentDir;
        //system(dir.c_str());
    }
    else {
        std::cerr << "获取当前目录失败!" << std::endl;
    }



    /*
 
    TCHAR currentDir[MAX_PATH];
    DWORD length = GetCurrentDirectory(MAX_PATH, currentDir);

    if (length > 0)
    {
        // 估算转换后所需字节数
        int targetSize = WideCharToMultiByte(CP_ACP, 0, currentDir, -1, NULL, 0, NULL, NULL);

        // 创建目标缓冲区
        //char *convertedDir=[targetSize];

        char* convertedDir = new char[targetSize];

        // 执行转换操作
        WideCharToMultiByte(CP_ACP, 0, currentDir, -1, convertedDir, targetSize, NULL, NULL);

        std::cout << "当前工作目录：" << convertedDir << std::endl;

        delete[]convertedDir;
    }
    else
    {
        std::cerr << "获取当前工作目录失败！" << std::endl;
    }
    */
    if (argc == 1)
    {



        std::ifstream file(pre_path.append("/readme.txt"));
        if (file.is_open())
        {
            std::string line;
            while (std::getline(file, line))
            {
                std::cout << line << std::endl;
            }
            file.close();
            
        }
        else
        {
            std::cerr << "无法打开文件 readme.txt" << std::endl;
            return 1;
        }


        return 0;
    }
    std::string argName = argv[1];
    std::string output = pre_path + "/" +  argName;
    std::list<CommandInfo> commandList;
    std::ifstream file(output);

    if (!file.is_open())
    {
        std::cerr << "无法打开文件" << std::endl;
        return 1;
    }

    std::string line;
    int lineCount = 1;
    CommandInfo info;
    int sequenceNumber = 1; // 序列号

    while (std::getline(file, line))
    {
        switch (lineCount)
        {
        case 1:
            info.description = line;
            break;
        case 2:
            info.flag = std::stoi(line); // 将字符串转换为整数
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
    bool validInput = false;

    while (!validInput) {
        std::cout << "请输入要执行的序列号: ";

        std::string input;
        std::getline(std::cin, input);

        try {
            choice = std::stoi(input);
            validInput = true;
        }
        catch (...) {
            std::cerr << "请输入错误，请输入序列号！" << std::endl;
        }
    }


    auto it = commandList.begin();
    std::advance(it, choice - 1); // 将迭代器移动到用户选择的位置

    if (it != commandList.end())
    {
        if (it->flag == 1)
        {
            std::cout << it->command << std::endl;
            system(it->command.c_str());
        }
        else
        {
            // std::cout << it->command << std::endl; // 在屏幕上显示命令
            displayCommandWithNewlines(it->command);
            std::string output = replaceBracketedContent(it->command);
            std::cout << output << std::endl;
            system(output.c_str());
        }
    }
    else
    {
        std::cout << "无效的序列号" << std::endl;
    }

    return 0;
}
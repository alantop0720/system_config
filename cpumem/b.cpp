#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>
#include <dirent.h>
#include <sys/stat.h>
#include <cstring>
#include <csignal>

// 用于标记是否接收到终止信号
volatile sig_atomic_t stopProgram = 0;

// 获取目录下文件尺寸最大的文件名
std::string getLargestFileInDirectory(const std::string& dirPath) {
    std::string largestFileName;
    off_t largestFileSize = 0;

    DIR* dir = opendir(dirPath.c_str());
    if (!dir) {
        std::cerr << "无法打开目录: " << dirPath << std::endl;
        return "";
    }

    struct dirent* entry;
    while ((entry = readdir(dir))!= nullptr) {
        if (entry->d_type == DT_REG) {
            std::string filePath = dirPath + "/" + entry->d_name;
            struct stat fileStat;
            if (stat(filePath.c_str(), &fileStat) == 0) {
                if (fileStat.st_size > largestFileSize) {
                    largestFileSize = fileStat.st_size;
                    largestFileName = entry->d_name;
                }
            }
        }
    }

    closedir(dir);
    return largestFileName;
}

// 信号处理函数
void signalHandler(int signum) {
    std::cout << "接收到终止信号，正在清理资源并退出..." << std::endl;
    stopProgram = 1;
}

int main() {
    // 注册信号处理函数
    signal(SIGINT, signalHandler);

    while (!stopProgram) {
        // 获取当前目录下文件尺寸最大的文件名
        std::string largestFileName = getLargestFileInDirectory(".");
        if (largestFileName.empty()) {
            std::cerr << "未找到文件" << std::endl;
            continue;
        }

        std::ifstream file(largestFileName, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "无法打开文件: " << largestFileName << std::endl;
            continue;
        }

        file.seekg(0, std::ios::end);
        std::size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<char> originalBuffer(fileSize);
        file.read(originalBuffer.data(), fileSize);
        file.close();

        std::vector<char> invertedBuffer(fileSize);
        for (std::size_t i = 0; i < fileSize; ++i) {
            invertedBuffer[i] = ~originalBuffer[i];
        }

        std::cout << "内容已取反并拷贝，暂停1分钟..." << std::endl;
        std::this_thread::sleep_for(std::chrono::minutes(1));
    }

    std::cout << "程序已退出" << std::endl;
    return 0;
}

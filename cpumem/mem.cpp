#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
std::mutex mtx;
std::condition_variable cv;
bool inputReceived = false;
int inputValue = 0;

void memoryAllocationAndRelease() {
    while (true) {
        // 分配 1G 内存
        char* memory = new char[1024 * 1024 * 1024]; 
        // 在这里可以对分配的内存进行操作，例如填充数据等
        for (int i = 0; i < 1024 * 1024 * 1024; ++i) {
            memory[i] = '1' + (i % 9);
        }

        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, []{ return inputReceived; });

        if (inputValue == 1) {
            std::cout << "输入为 1，线程结束" << std::endl;
            return; 
        }
		delete[] memory;
    }
}

int main(int argc, char* argv[]) {
    int num = std::stoi(argv[1]);
    std::vector<std::thread> threads;
    for (int i = 0; i < num; ++i) {
        threads.emplace_back(memoryAllocationAndRelease);
    }

    std::cout << "请在另一个线程中输入一个整数：";

    // 这里模拟另一个线程输入指令
    std::cin >> inputValue; 
    {
        std::lock_guard<std::mutex> lock(mtx);
        inputReceived = true;
    }
    cv.notify_all(); 

    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}

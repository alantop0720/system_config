#include <iostream>
#include <thread>
#include <ctime>

void cpuIntensiveTask() {
    while (true) {
	clock_t start = clock();
    while ((clock() - start) / CLOCKS_PER_SEC < 10) {  // 运行 10 秒
        // 进行一些无意义的计算，以消耗 CPU 资源
        int result = 0;
        for (int i = 0; i < 10000000; ++i) {
            result += i;
        }
    }
	}
}


int main(int argc, char* argv[]) {
    int number = std::stoi(argv[1]);
    std::thread threads[number];

    for (int i = 0; i < number; ++i) {
        threads[i] = std::thread(cpuIntensiveTask);
    }

    for (int i = 0; i < number; ++i) {
        threads[i].join();
    }

    return 0;
}

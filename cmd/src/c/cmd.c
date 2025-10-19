#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#define MAX_LINE_LENGTH 1024
#define MAX_COMMANDS 100

// 定义一个结构体来存储每行的信息
typedef struct {
    char description[MAX_LINE_LENGTH];
    int flag;
    char command[MAX_LINE_LENGTH];
} CommandInfo;

void replaceBracketedContent(char* input, char* output) {
    int i = 0, j = 0;
    while (input[i] != '\0') {
        if (input[i] == '[') {
            // 找到左括号，开始提取key
            int start = i + 1;
            int keyLen;
            char key[256];
            char value[256];
            i++;
            while (input[i] != ']' && input[i] != '\0') {
                i++;
            }
            if (input[i] == ']') {
                // 提取key
                keyLen = i - start;
                strncpy(key, input + start, keyLen);
                key[keyLen] = '\0';
                
                // 提示用户输入
                printf("请输入 %s: ", key);
                if (fgets(value, sizeof(value), stdin)) {
                    // 移除换行符
                    value[strcspn(value, "\n")] = '\0';
                    // 将value复制到output
                    strcpy(output + j, value);
                    j += strlen(value);
                }
                i++; // 跳过右括号
            } else {
                // 没有找到右括号，直接复制左括号
                output[j++] = '[';
            }
        } else {
            output[j++] = input[i++];
        }
    }
    output[j] = '\0';
}

void displayCommandWithNewlines(const char* command) {
    int len = strlen(command);
    int i;
    for (i = 0; i < len - 1; i++) {
        if (command[i] == '\\' && command[i + 1] == 'r') {
            printf("\n");
            i++; // 跳过第二个字符
        } else {
            printf("%c", command[i]);
        }
    }
    if (len > 0) {
        printf("%c\n", command[len - 1]); // 输出最后一个字符
    }
}

void readAndDisplayReadme() {
    FILE *file = fopen("/home/cmd/readme", "r");
    if (file != NULL) {
        char line[MAX_LINE_LENGTH];
        while (fgets(line, sizeof(line), file)) {
            // fgets保留换行符，直接打印即可
            printf("%s", line);
        }
        fclose(file);
        system("ls /home/cmd");
    } else {
        fprintf(stderr, "无法打开文件readme\n");
    }
}

int processCommandFile(const char* filename, CommandInfo commands[], int maxCommands) {
    FILE *file = fopen(filename, "r");
    char line[MAX_LINE_LENGTH];
    int lineCount = 1;
    int commandCount = 0;
    int sequenceNumber = 1;
    
    if (!file) {
        fprintf(stderr, "无法打开文件\n");
        return -1;
    }
    
    while (fgets(line, sizeof(line), file) && commandCount < maxCommands) {
        // 移除换行符
        line[strcspn(line, "\n")] = '\0';
        
        switch (lineCount) {
            case 1:
                strcpy(commands[commandCount].description, line);
                break;
            case 2:
                commands[commandCount].flag = atoi(line);
                break;
            case 3:
                strcpy(commands[commandCount].command, line);
                printf("%d. %s\n", sequenceNumber, commands[commandCount].description);
                sequenceNumber++;
                commandCount++;
                lineCount = 0;
                break;
        }
        lineCount++;
    }
    
    fclose(file);
    return commandCount;
}

int main(int argc, char *argv[]) {
    char filepath[PATH_MAX];
    CommandInfo commands[MAX_COMMANDS];
    int commandCount;
    int choice;
    int c;
    CommandInfo *selectedCommand;
    char processedCommand[MAX_LINE_LENGTH * 2];
    
    if (argc == 1) {
        readAndDisplayReadme();
        return 0;
    }
    
    // 构建文件路径
    snprintf(filepath, sizeof(filepath), "/home/cmd/%s", argv[1]);
    
    commandCount = processCommandFile(filepath, commands, MAX_COMMANDS);
    
    if (commandCount <= 0) {
        return 1;
    }
    
    printf("请输入要执行的序列号: ");
    if (scanf("%d", &choice) != 1) {
        printf("输入无效\n");
        return 1;
    }
    
    // 清空输入缓冲区
    while ((c = getchar()) != '\n' && c != EOF);
    
    if (choice < 1 || choice > commandCount) {
        printf("无效的序列号\n");
        return 1;
    }
    
    selectedCommand = &commands[choice - 1];
    
    if (selectedCommand->flag == 1) {
        printf("%s\n", selectedCommand->command);
        system(selectedCommand->command);
    } else {
        displayCommandWithNewlines(selectedCommand->command);
        replaceBracketedContent(selectedCommand->command, processedCommand);
        printf("%s\n", processedCommand);
        system(processedCommand);
    }
    
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define getcwd _getcwd
#else
#include <unistd.h>
#endif

#define MAX_LINE_LENGTH 1024
#define MAX_COMMANDS 100

#ifdef _WIN32
#define DEFAULT_BASE_PATH "C:\\opt\\cmd"
#else
#define DEFAULT_BASE_PATH "/opt/cmd"
#endif

int get_exe_dir(char *dir, size_t size) {
#ifdef _WIN32
    DWORD len = GetModuleFileName(NULL, dir, (DWORD)size);
    if (len == 0 || len >= size) return -1;
    char *sep = strrchr(dir, '\\');
    if (sep) *sep = '\0';
    return 0;
#else
    ssize_t len = readlink("/proc/self/exe", dir, size - 1);
    if (len == -1) return -1;
    dir[len] = '\0';
    char *sep = strrchr(dir, '/');
    if (sep) *sep = '\0';
    return 0;
#endif
}

void read_config(char *base_path, size_t size) {
    char exe_dir[PATH_MAX];
    char ini_path[PATH_MAX];

    if (get_exe_dir(exe_dir, sizeof(exe_dir)) == 0) {
        snprintf(ini_path, sizeof(ini_path), "%s/cmd.ini", exe_dir);
    } else {
        snprintf(ini_path, sizeof(ini_path), "cmd.ini");
    }

    FILE *file = fopen(ini_path, "r");
    if (file != NULL) {
        if (fgets(base_path, size, file)) {
            base_path[strcspn(base_path, "\r\n")] = '\0';
        } else {
            strncpy(base_path, DEFAULT_BASE_PATH, size);
        }
        fclose(file);
    } else {
        strncpy(base_path, DEFAULT_BASE_PATH, size);
    }
}

typedef struct {
    char description[MAX_LINE_LENGTH];
    int flag;
    char command[MAX_LINE_LENGTH];
} CommandInfo;

void replaceBracketedContent(char* input, char* output) {
    int i = 0, j = 0;
    while (input[i] != '\0') {
        if (input[i] == '[') {
            int start = i + 1;
            int keyLen;
            char key[256];
            char value[256];
            i++;
            while (input[i] != ']' && input[i] != '\0') {
                i++;
            }
            if (input[i] == ']') {
                keyLen = i - start;
                strncpy(key, input + start, keyLen);
                key[keyLen] = '\0';
                printf("请输入 %s: ", key);
                if (fgets(value, sizeof(value), stdin)) {
                    value[strcspn(value, "\r\n")] = '\0';
                    strcpy(output + j, value);
                    j += strlen(value);
                }
                i++;
            } else {
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
            i++;
        } else {
            printf("%c", command[i]);
        }
    }
    if (len > 0) {
        printf("%c\n", command[len - 1]);
    }
}

void readAndDisplayReadme(const char *base_path) {
    char filepath[PATH_MAX];
    snprintf(filepath, sizeof(filepath), "%s/readme", base_path);
    FILE *file = fopen(filepath, "r");
    if (file != NULL) {
        char line[MAX_LINE_LENGTH];
        while (fgets(line, sizeof(line), file)) {
            printf("%s", line);
        }
        fclose(file);
        char cmd[PATH_MAX + 10];
#ifdef _WIN32
        snprintf(cmd, sizeof(cmd), "dir \"%s\"", base_path);
#else
        snprintf(cmd, sizeof(cmd), "ls \"%s\"", base_path);
#endif
        system(cmd);
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
        line[strcspn(line, "\r\n")] = '\0';

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
    char base_path[PATH_MAX];
    char filepath[PATH_MAX];
    CommandInfo commands[MAX_COMMANDS];
    int commandCount;
    int choice;
    int c;
    CommandInfo *selectedCommand;
    char processedCommand[MAX_LINE_LENGTH * 2];

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    read_config(base_path, sizeof(base_path));

    if (argc == 1) {
        readAndDisplayReadme(base_path);
        return 0;
    }

    snprintf(filepath, sizeof(filepath), "%s/%s", base_path, argv[1]);

    commandCount = processCommandFile(filepath, commands, MAX_COMMANDS);

    if (commandCount <= 0) {
        return 1;
    }

    if (argc >= 3) {
        choice = atoi(argv[2]);
    } else {
        printf("请输入要执行的序列号: ");
        if (scanf("%d", &choice) != 1) {
            printf("输入无效\n");
            return 1;
        }
        while ((c = getchar()) != '\n' && c != EOF);
    }

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

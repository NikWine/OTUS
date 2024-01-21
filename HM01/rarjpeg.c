#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define ZIP_MAGIC_HEADER "\x50\x4B\x03\x04"

// Функция для проверки, является ли файл Rarjpeg
int is_rarjpeg(const char *filename) {
    // Открытие файла для чтения в бинарном режиме
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("fopen");
        return 0;
    }

    printf("In rarjpeg\n");

    char header[4];
    int position, i, sz;
    // Определение размера файла
    fseek(file, 0L, SEEK_END);
    sz = ftell(file);
    // Поиск заголовка ZIP со смещением в 4 байта
    fseek(file, 0, SEEK_SET);
    for (i = 1; i < sz; i++) {
        fseek(file, i, SEEK_SET);
        fread(header, 4, 1, file);
        if (memcmp(header, ZIP_MAGIC_HEADER, 4) == 0) {
            position=i;
            printf("position %i", position);
            return 1;
        }
    }

    return 0;
}

// Функция для получения списка файлов из RAR архива
void list_rar_files(const char *filename) {
    // Создание процесса
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    
    // Испытательная работа извлекается в дочернем процессе
    if (pid == 0) {
        execlp("unzip", "unzip", "-lv", filename, NULL);
        perror("exec");
        exit(EXIT_FAILURE);
    } 
    // Родительский процесс считывает статус завершения дочернего процесса
    else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            fprintf(stderr, "Process exited with error %d\n", WEXITSTATUS(status));
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Использование: %s <имя файла>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];

    if (is_rarjpeg(filename)) {
        printf("Файл '%s' является Rarjpeg.\nСодержимое RAR архива:\n", filename);
        list_rar_files(filename);
    } else {
        printf("Файл '%s' не является Rarjpeg.\n", filename);
    }

    return 0;
}
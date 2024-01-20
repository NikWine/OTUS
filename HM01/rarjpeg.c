//Compile with gcc -o ftest ftest.c -Wall -Wextra -Wpedantic -std=c11
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define RAR_SIGNATURE "Rar"
#define ZIP_MAGIC_HEADER "\x50\x4B\x03\x04"

//Функция проверки файла на rarjpeg
int is_rarjpeg(const char *filename) {
// Открытие файла на чтение в бинарном режиме
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("fopen");
        return 0;
    }

  printf("In rarjpeg\n");

  char header[4];
  int position, i, sz;
  //Опеределение размера файла
  fseek(file, 0L, SEEK_END);
  sz = ftell(file);
  //Поиск заголовка zip со смещением по 4 байта
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
// Функция для извлечения списка файлов из RAR архива
void list_rar_files(const char *filename) {
    char command[256];
    //Сборка команды для системного вызова unzip с параметрами -l  list files (short format) -v  list verbosely/show version info
    snprintf(command, sizeof(command), "unzip -lv \"%s\"", filename);
    //Запуск 
    system(command);
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
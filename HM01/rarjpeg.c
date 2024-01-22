#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>

// Эти макросы используются для обработки имен файлов
#define SIZEARR(ARR) (sizeof(ARR) / sizeof(ARR[0]))
#define MAX_FILE_NAME_LEN 512
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))

// Сигнатура для локального заголовка файла ZIP
const unsigned char zip_lfh_sign[] = { '\x50', '\x4b', '\x03', '\x04'};

// Структура локального заголовка файла ZIP без сигнатуры
#pragma pack(1)
struct lfh_tail {
    uint8_t something[14];
    uint32_t compressed_size;
    uint32_t uncompressed_size;
    uint16_t filename_len;
    uint16_t extra_len;
};
#pragma pack()

// Сигнатуры JPEG
const unsigned char jpeg_soi[] = {'\xFF', '\xD8'};
const unsigned char jpeg_eoi[] = {'\xFF', '\xD9'};

// Функция для чтения файла до тех пор, пока не встречается указанный шаблон
bool read_until_match(FILE *f, const unsigned char pattern[], size_t sz, int *io_error) {
    size_t i = 0;
    int ch = '\0';

    // Цикл чтения каждого символа
    while(i < sz && (ch = fgetc(f)) != EOF) {
        if(ch == pattern[i]) {
            i++;
        }
        else
        {
            i = 0;
        }
    }

    // Проверяем на наличие ошибки ввода/вывода
    if(!io_error) {
        *io_error = (ch == EOF && !feof(f)) ? errno : 0;
    }

    // Возвращаем true, если встретили полный шаблон, или false в противном случае
    return (i == sz);
}

// Главная функция
int main(int argc, char *argv[]) {
    // Проверка правильности числа аргументов
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Открытие файла для чтения
    const char *filename = argv[1];
    FILE *f = fopen(filename, "rb");
    if(!f) {
        printf("Error opening file\n");
        return EXIT_FAILURE;
    }

    // Счетчик ошибок ввода/вывода
    int io_error = 0;

    // Проверка, начинается ли файл с начала JPEG
    if(read_until_match(f, jpeg_soi, SIZEARR(jpeg_soi), &io_error)) {
        // Цикл для чтения каждого "локального заголовка файла" (Local File Header) в ZIP
         int file_counter = 0;
         while(read_until_match(f, zip_lfh_sign, SIZEARR(zip_lfh_sign), &io_error)) {
           char string_buf[MAX_FILE_NAME_LEN + 1];
           struct lfh_tail lfh;
           size_t retval;
           uint16_t len;

           // Чтение структуры информации о файле
           retval = fread(&lfh, sizeof(lfh), 1, f);
           if(retval == 0) {
               io_error = !feof(f);
               break;
           }

           // Чтение имени файла
           len = lfh.filename_len;
           retval = fread(string_buf, 1, MIN(MAX_FILE_NAME_LEN, len), f);
           if(retval == 0) {
               io_error = !feof(f);
               break;
           }

           // Запоминаем имя файла
           string_buf[retval] = '\0';

           // Смещаем указатель файла на длину скомпрессированного файла и дополнительного поля
           fseek(f, lfh.compressed_size + lfh.extra_len, SEEK_CUR);

           // Выводим информацию о файле
           printf("File: %s, Size: %d\n", string_buf, lfh.compressed_size);
           file_counter++;
       }

       // Проверка на наличие ошибок ввода/вывода
       if(io_error != 0 && !feof(f)) {
           printf("Error reading file\n");
       }
       // Вывод сообщения в зависимости от содержимого файла
       else if(file_counter == 0) {
           printf("%s это файл JPEG\n", filename);
       }
       else {
           printf("%s это архив rarjpeg\n", filename);
       }
    }
    else {
        printf("%s это не архив JPEG или rarjpeg\n", filename);
    }
    // Закрытие файла
    fclose(f);
    return EXIT_SUCCESS;
}
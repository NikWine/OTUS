#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Таблицы соответствия кодов символов для различных кодировок
#include "cp1251_to_utf8.h"
#include "koi8r_to_utf8.h"
#include "iso88595_to_utf8.h"

// Функция конвертации байта в UTF-8
void convert_byte_to_utf8(FILE *input, FILE *output, const unsigned char (*table)[3]) {
    unsigned char byte;
    if (fread(&byte, 1, 1, input) == 1) {
        const unsigned char *utf8 = table[byte];
        fwrite(utf8, 1, strlen((const char *)utf8), output); // Используем strlen для определения длины UTF-8 последовательности
    }
}

// Основная функция конвертации файла
int convert_file(const char *input_filename, const char *encoding, const char *output_filename) {
    FILE *input, *output;

    // Открываем входной файл для чтения
    input = fopen(input_filename, "rb");
    if (!input) {
        fprintf(stderr, "Ошибка открытия входного файла: %s\n", strerror(errno));
        return 1;
    }

    // Открываем выходной файл для записи
    output = fopen(output_filename, "wb");
    if (!output) {
        fprintf(stderr, "Ошибка открытия выходного файла: %s\n", strerror(errno));
        fclose(input);
        return 1;
    }

    // Выбираем таблицу соответствия на основе заданной кодировки
    const unsigned char (*conversion_table)[3] = NULL; // Указатель на массив из трёх unsigned char
    
    if (strcmp(encoding, "CP-1251") == 0) {
        conversion_table = cp1251_to_utf8_table;
    } else if (strcmp(encoding, "KOI8-R") == 0) {
        conversion_table = koi8r_to_utf8_table;
    } else if (strcmp(encoding, "ISO-8859-5") == 0) {
        conversion_table = iso88595_to_utf8_table;
    } else {
        fprintf(stderr, "Неизвестная кодировка: %s\n", encoding);
        // Предполагается, что файлы уже открыты, закрываем их
        fclose(input);
        fclose(output);
        return 1;
    }

    // Конвертируем входной файл в UTF-8
    while (!feof(input)) {
        convert_byte_to_utf8(input, output, conversion_table);
    }

    // Закрываем файлы
    fclose(input);
    fclose(output);

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Использование: %s <входной файл> <кодировка> <выходной файл>\n", argv[0]);
        return 1;
    }

    const char *input_filename = argv[1];
    const char *encoding = argv[2];
    const char *output_filename = argv[3];

    return convert_file(input_filename, encoding, output_filename);
}

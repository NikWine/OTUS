#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ZIP_MAGIC_HEADER "\x50\x4B\x03\x04"

// Определение структуры заголовка ZIP
struct zip_header {
    char signature[4];
    short version;
    short flags;
    short compression;
    short mod_time;
    short mod_date;
    int crc32;
    int compressed_size;
    int uncompressed_size;
    short filename_length;
    short extra_field_length;
};

// Функция для проверки, является ли файл Rarjpeg
int is_rarjpeg(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("fopen");
        return 0;
    }

    struct zip_header header;
    size_t sz;
    fseek(file, 0L, SEEK_END);
    sz = ftell(file);

    fseek(file, 0, SEEK_SET);
    for (size_t i = 0; i < sz - sizeof(header); ++i) {
        fseek(file, i, SEEK_SET);
        fread(&header, sizeof(header), 1, file);
        // Проверка подписи ZIP
        if (memcmp(header.signature, ZIP_MAGIC_HEADER, 4) == 0) {
            fseek(file, i + sizeof(header), SEEK_SET);
            char* filename_in_zip = (char*) malloc(header.filename_length + 1);
            fread(filename_in_zip, header.filename_length, 1, file);
            filename_in_zip[header.filename_length] = '\0';  // Добавление конца строки
            printf("%s\n", filename_in_zip);
            free(filename_in_zip);
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];

    if (is_rarjpeg(filename)) {
        printf("Файл '%s' является rarjpeg.\n", filename);
    } else {
        printf("Файл '%s' не является rarjpeg.\n", filename);
    }

    return EXIT_SUCCESS;
}
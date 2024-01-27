#include <stdio.h>
#include <stdint.h>

uint32_t table[256]; // Таблица предварительно вычисленных значений crc

// Функция для генерации таблицы CRC32
void generate_table() {
    for (int i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc >>= 1;
                crc ^= 0xEDB88320;
            } else {
                crc >>= 1;
            }
        }
        table[i] = crc;
    }
}

// Функция для вычисления checksum по содержимому файла
uint32_t calculate_crc(FILE* file) {
    uint32_t crc = -1;
    unsigned char buf[4096];
    size_t len;

    // Читаем содержимое файла блоками
    while ((len = fread(buf, 1, sizeof(buf), file)) != 0) {
        for (size_t i = 0; i < len; i++) {
            crc = table[(crc ^ buf[i]) & 0xFF] ^ (crc >> 8);
        }
    }

    return crc ^ -1;
}

// Главная функция
int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    FILE* file = fopen(argv[1], "rb"); // Открыть файл

    if (!file) {
        perror(argv[1]);
        return 1;
    }

    // Сгенерировать таблицу crc32, вычислить контрольную сумму и вывести её
    generate_table();
    printf("CRC32: %08X\n", calculate_crc(file));

    fclose(file); // Закрыть файл
    return 0;
}
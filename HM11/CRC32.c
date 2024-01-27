#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

uint32_t table[256]; // Таблица предварительно вычисленных значений crc

// Функция для генерации таблицы CRC32
void generate_table() {
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
uint32_t calculate_crc(unsigned char* buf, size_t len) {
    uint32_t crc = -1;

    for (size_t i = 0; i < len; i++) {
        crc = table[(crc ^ buf[i]) & 0xFF] ^ (crc >> 8);
    }

    return crc ^ -1;
}

// Главная функция
int main(int argc, char* argv[]) {
    int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY); // Открываем файл
    if (fd == -1) {
        perror(argv[1]);
        return 1;
    }

    // Получаем размер файла
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("fstat");
        return 1;
    }

    // Отображаем файл в память
    unsigned char* file_in_memory = mmap(0, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_in_memory == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    generate_table();
    printf("CRC32: %08X\n", calculate_crc(file_in_memory, sb.st_size));

    munmap(file_in_memory, sb.st_size); // Удалить отображение в память
    close(fd); // Закрыть файл
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Прототипы функций */
void convert_koi8r_to_utf8(const unsigned char *input, char *output);
void convert(const char* encoding, const char* input, char* output);
void convert_iso8859_5_to_utf8(const char* input, char* output);
void convert_cp1251_to_utf8(const char* input, char* output);
int unicode_to_utf8(unsigned int codepoint, unsigned char *output);
void convert_cp1251_to_utf8(const char* input, char* output) {
  static const char *cp1251_utf8[128] = {
    // 0x80 -> 0x8F
    "\xE2\x82\xAC", "\xC2\x81", "\xE2\x80\x9A", "\xC6\x92", "\xE2\x80\x9E", "\xE2\x80\xA6", "\xE2\x80\xA0", "\xE2\x80\xA1",
    "\xCB\x86", "\xE2\x80\xB0", "\xE2\x80\xB9", "\xE2\x82\xAC", "\xC2\x8C", "\xC2\x8A", "\xC2\x8B", "\xC2\x8F",
    // 0x90 -> 0x9F
    "\xC2\x90", "\xE2\x80\x98", "\xE2\x80\x99", "\xE2\x80\x9C", "\xE2\x80\x9D", "\xE2\x80\xA2", "\xE2\x80\x93", "\xE2\x80\x94",
    "\xCB\x9C", "\xE2\x84\xA2", "\xE2\x80\xBA", "\xE2\x82\xAC", "\xC2\x9C", "\xC2\x9A", "\xC2\x9B", "\xC2\x9F",
    // 0xA0 -> 0xAF
    "\xC2\xA0", "\xD0\x8E", "\xD1\x9E", "\xD0\x88", "\xC2\xA4", "\xD2\x90", "\xC2\xA6", "\xC2\xA7",
    "\xD0\x81", "\xC2\xA9", "\xD0\x84", "\xC2\xAB", "\xC2\xAC", "\xC2\xAD", "\xC2\xAE", "\xD0\x87",
    // 0xB0 -> 0xBF
    "\xC2\xB0", "\xC2\xB1", "\xD0\x86", "\xD1\x96", "\xD2\x91", "\xC2\xB5", "\xC2\xB6", "\xC2\xB7",
    "\xD1\x91", "\xE2\x84\x96", "\xD1\x94", "\xC2\xBB", "\xD1\x98", "\xD0\x85", "\xD1\x95", "\xD1\x97",
    // 0xC0 -> 0xCF
    "\xD0\x90", "\xD0\x91", "\xD0\x92", "\xD0\x93", "\xD0\x94", "\xD0\x95", "\xD0\x96", "\xD0\x97",
    "\xD0\x98", "\xD0\x99", "\xD0\x9A", "\xD0\x9B", "\xD0\x9C", "\xD0\x9D", "\xD0\x9E", "\xD0\x9F",
    // 0xD0 -> 0xDF
    "\xD0\xA0", "\xD0\xA1", "\xD0\xA2", "\xD0\xA3", "\xD0\xA4", "\xD0\xA5", "\xD0\xA6", "\xD0\xA7",
    "\xD0\xA8", "\xD0\xA9", "\xD0\xAA", "\xD0\xAB", "\xD0\xAC", "\xD0\xAD", "\xD0\xAE", "\xD0\xAF",
    // 0xE0 -> 0xEF
    "\xD0\xB0", "\xD0\xB1", "\xD0\xB2", "\xD0\xB3", "\xD0\xB4", "\xD0\xB5", "\xD0\xB6", "\xD0\xB7",
    "\xD0\xB8", "\xD0\xB9", "\xD0\xBA", "\xD0\xBB", "\xD0\xBC", "\xD0\xBD", "\xD0\xBE", "\xD0\xBF",
    // 0xF0 -> 0xFF
    "\xD1\x80", "\xD1\x81", "\xD1\x82", "\xD1\x83", "\xD1\x84", "\xD1\x85", "\xD1\x86", "\xD1\x87",
    "\xD1\x88", "\xD1\x89", "\xD1\x8A", "\xD1\x8B", "\xD1\x8C", "\xD1\x8D", "\xD1\x8E", "\xD1\x8F"
};

   unsigned char c;
   while ((c = *input++) != 0) {
       if (c < 0x80) {
           *output++ = c;
       } else {
           const char* utf8_seq = cp1251_utf8[c - 0x80];
           while (*utf8_seq) {
               *output++ = *utf8_seq++;
           }
       }
   }
   *output++ = 0;
}

// реализация convert_koi8r_to_utf8
void convert_koi8r_to_utf8(const unsigned char *input, char *output) {
   static const char *koi8r_utf8[256] = {
    "\x00", "\x01", "\x02", "\x03", "\x04", "\x05", "\x06", "\x07",
    "\x08", "\x09", "\x0a", "\x0b", "\x0c", "\x0d", "\x0e", "\x0f",
    "\x10", "\x11", "\x12", "\x13", "\x14", "\x15", "\x16", "\x17",
    "\x18", "\x19", "\x1a", "\x1b", "\x1c", "\x1d", "\x1e", "\x1f",
    "\x20", "\x21", "\x22", "\x23", "\x24", "\x25", "\x26", "\x27",
    "\x28", "\x29", "\x2a", "\x2b", "\x2c", "\x2d", "\x2e", "\x2f",
    "\x30", "\x31", "\x32", "\x33", "\x34", "\x35", "\x36", "\x37",
    "\x38", "\x39", "\x3a", "\x3b", "\x3c", "\x3d", "\x3e", "\x3f",
    "\x40", "\x41", "\x42", "\x43", "\x44", "\x45", "\x46", "\x47",
    "\x48", "\x49", "\x4a", "\x4b", "\x4c", "\x4d", "\x4e", "\x4f",
    "\x50", "\x51", "\x52", "\x53", "\x54", "\x55", "\x56", "\x57",
    "\x58", "\x59", "\x5a", "\x5b", "\x5c", "\x5d", "\x5e", "\x5f",
    "\x60", "\x61", "\x62", "\x63", "\x64", "\x65", "\x66", "\x67",
    "\x68", "\x69", "\x6a", "\x6b", "\x6c", "\x6d", "\x6e", "\x6f",
    "\x70", "\x71", "\x72", "\x73", "\x74", "\x75", "\x76", "\x77",
    "\x78", "\x79", "\x7a", "\x7b", "\x7c", "\x7d", "\x7e", "\x7f",
    // Кириллическая часть KOI8-R [0x80, 0xFF]
"\xE2\x94\x80", "\xE2\x94\x82", "\xE2\x94\x8C", "\xE2\x94\x90", "\xE2\x94\x94", "\xE2\x94\x98", "\xE2\x94\x9C", "\xE2\x94\xA4",
"\xE2\x94\xAC", "\xE2\x94\xB4", "\xE2\x94\xBC", "\xE2\x96\x80", "\xE2\x96\x84", "\xE2\x96\x88", "\xE2\x96\x8C", "\xE2\x96\x90",
"\xE2\x96\x91", "\xE2\x96\x92", "\xE2\x96\x93", "\xE2\x88\xA0", "\xE2\x96\xA0", "\xE2\x88\x99", "\xE2\x88\x9A", "\xE2\x89\x88",
"\xE2\x89\xA4", "\xE2\x89\xA5", "\xC2\xA0", "\xE2\x88\xA1", "\xC2\xB0", "\xC2\xB2", "\xC2\xB7", "\xC3\xB7",
"\xE2\x95\x90", "\xE2\x95\x91", "\xE2\x95\x92", "\xD1\x91", "\xE2\x95\x93", "\xE2\x95\x94", "\xE2\x95\x95", "\xE2\x95\x96",
"\xE2\x95\x97", "\xE2\x95\x98", "\xE2\x95\x99", "\xE2\x95\x9A", "\xE2\x95\x9B", "\xE2\x95\x9C", "\xE2\x95\x9D", "\xE2\x95\x9E",
"\xE2\x95\x9F", "\xE2\x95\xA0", "\xE2\x95\xA1", "\xE2\x95\xA2", "\xE2\x95\xA3", "\xE2\x95\xA4", "\xE2\x95\xA5", "\xE2\x95\xA6",
"\xE2\x95\xA7", "\xE2\x95\xA8", "\xE2\x95\xA9", "\xE2\x95\xAA", "\xE2\x95\xAB", "\xE2\x95\xAC", "\xE2\x95\xAD", "\xC2\xA9",
"\xD1\x8E", "\xD0\xB0", "\xD0\xB1", "\xD1\x86", "\xD0\xB4", "\xD0\xB5", "\xD1\x84", "\xD0\xB3",
"\xD1\x85", "\xD0\xB8", "\xD0\xB9", "\xD0\xBA", "\xD0\xBB", "\xD0\xBC", "\xD0\xBD", "\xD0\xBE",
"\xD0\xBF", "\xD1\x8F", "\xD1\x80", "\xD1\x81", "\xD1\x82", "\xD1\x83", "\xD0\xB6", "\xD0\xB2",
"\xD1\x8C", "\xD1\x8B", "\xD0\xB7", "\xD1\x88", "\xD1\x8D", "\xD1\x89", "\xD1\x87", "\xD1\x8A",
"\xD0\xAE", "\xD0\x90", "\xD0\x91", "\xD0\xA6", "\xD0\x94", "\xD0\x95", "\xD0\xA4", "\xD0\x93",
"\xD0\xA5", "\xD0\x98", "\xD0\x99", "\xD0\x9A", "\xD0\x9B", "\xD0\x9C", "\xD0\x9D", "\xD0\x9E",
"\xD0\x9F", "\xD0\xAF", "\xD0\xA0", "\xD0\xA1", "\xD0\xA2", "\xD0\xA3", "\xD0\x96", "\xD0\x92",
"\xD0\xAC", "\xD0\xAB", "\xD0\x97", "\xD0\xA8", "\xD0\xAD", "\xD0\xA9", "\xD0\xA7", "\xD0\xAA"
};

  while (*input) { // Перебираем входной массив пока не встретим нулевой символ
        const char *utf8_seq = koi8r_utf8[*input]; // Замена на правильный доступ к элементу массива

        // Копируем последовательность UTF-8 в выходной массив
        while (*utf8_seq) {
            *output++ = *utf8_seq++;
        }

        input++; // Переходим к следующему символу
    }

    // Добавляем нулевой символ к концу выходной строки
    *output = '\0';
}

// Полная реализация convert_iso8859_5_to_utf8
void convert_iso8859_5_to_utf8(const char* input, char* output) {
unsigned short iso_8859_5_to_utf8[256] = {
    // 0x00-0x7F: Basic Latin (identical to Unicode)
    0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
    0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F,
    0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017,
    0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F,
    0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
    0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
    0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,
    0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
    0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
    0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
    0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F,
    0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
    0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
    0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
    0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x007F,

    // 0x80-0x9F: Control characters (unused in ISO-8859-5)
    0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087,
    0x0088, 0x0089, 0x008A, 0x008B, 0x008C, 0x008D, 0x008E, 0x008F,
    0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097,
    0x0098, 0x0099, 0x009A, 0x009B, 0x009C, 0x009D, 0x009E, 0x009F,

    // 0xA0-0xFF: Extended characters specific to ISO-8859-5
    0x00A0, 0x0401, 0x0402, 0x0403, 0x0404, 0x0405, 0x0406, 0x0407,
    0x0408, 0x0409, 0x040A, 0x040B, 0x040C, 0x00AD, 0x040E, 0x040F,
    0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417,
    0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F,
    0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427,
    0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F,
    0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
    0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F,
    0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
    0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F,
    0x2116, 0x0451, 0x0452, 0x0453, 0x0454, 0x0455, 0x0456, 0x0457,
    0x0458, 0x0459, 0x045A, 0x045B, 0x045C, 0x00A7, 0x045E, 0x045F
};

    size_t j = 0;
    for (size_t i = 0; input[i] != '\0'; i++) {
        unsigned short unicode = iso_8859_5_to_utf8[(unsigned char)input[i]];
       j += unicode_to_utf8(unicode, (unsigned char *)&output[j]);
    }
}

// Полная реализация unicode_to_utf8
int unicode_to_utf8(unsigned int codepoint, unsigned char *output) {
    if (output == NULL) {
        return 0;
    }

    if (codepoint <= 0x7F) {
        // 1 байт
        output[0] = (uint8_t)codepoint;
        return 1;
    } else if (codepoint <= 0x7FF) {
        // 2 байта
        output[0] = 0xC0 | ((codepoint >> 6) & 0x1F);
        output[1] = 0x80 | (codepoint & 0x3F);
        return 2;
    } else if (codepoint <= 0xFFFF) {
        // 3 байта
        output[0] = 0xE0 | ((codepoint >> 12) & 0x0F);
        output[1] = 0x80 | ((codepoint >> 6) & 0x3F);
        output[2] = 0x80 | (codepoint & 0x3F);
        return 3;
    } else if (codepoint <= 0x10FFFF) {
        // 4 байта
        output[0] = 0xF0 | ((codepoint >> 18) & 0x07);
        output[1] = 0x80 | ((codepoint >> 12) & 0x3F);
        output[2] = 0x80 | ((codepoint >> 6) & 0x3F);
        output[3] = 0x80 | (codepoint & 0x3F);
        return 4;
    } else {
        // Кодовая точка за пределами диапазона Unicode
        return 0;
    }
}

// На основе указанной кодировки выбирает и вызывает соответствующую функцию конвертации
void convert(const char* encoding, const char* input, char* output) {
    if (strcmp(encoding, "CP1251") == 0) {
        convert_cp1251_to_utf8(input, output);
    } else if (strcmp(encoding, "KOI8-R") == 0) {
        convert_koi8r_to_utf8((const unsigned char *)input, output);
    } else if (strcmp(encoding, "ISO-8859-5") == 0) {
        convert_iso8859_5_to_utf8(input, output);
    } else {
        fprintf(stderr, "Неизвестная кодировка: %s\n", encoding);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Использование: %s <кодировка> <входной файл> <выходной файл>\n", argv[0]);
        return 1;
    }

    FILE* input_file = fopen(argv[2], "rb");
    if (!input_file) {
        perror("Не удалось открыть входной файл");
        return 1;
    }

    FILE* output_file = fopen(argv[3], "wb");
    if (!output_file) {
        perror("Не удалось открыть выходной файл");
        fclose(input_file);
        return 1;
    }

    // Считывание входного файла в строку
    fseek(input_file, 0, SEEK_END);
    long file_size = ftell(input_file);
    fseek(input_file, 0, SEEK_SET);

    char* input_text = (char*)malloc(file_size + 1);
    if(input_text == NULL) {
        fprintf(stderr, "Не удалось выделить память для входного текста\n");
        fclose(input_file);
        fclose(output_file);
        return 1;
    }

    fread(input_text, 1, file_size, input_file);
    input_text[file_size] = '\0';

    // Выделяем память для выходного текста, предполагаем худший случай, что каждый символ будет заменен на 2 байта
    char* output_text = (char*)malloc(file_size * 4 + 1); // Каждый символ может взять до 4 байт
    if(output_text == NULL) {
        fprintf(stderr, "Не удалось выделить память для выходного текста\n");
        free(input_text);
        fclose(input_file);
        fclose(output_file);
        return 1;
    }

    // Конвертация текста и запись в выходной файл
    convert(argv[1], input_text, output_text);
    fwrite(output_text, 1, strlen(output_text), output_file);

    // Очистка и закрытие ресурсов
    free(input_text);
    free(output_text);
    fclose(input_file);
    fclose(output_file);

    return 0;
}

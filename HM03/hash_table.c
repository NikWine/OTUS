#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TABLE_SIZE 10007  // Простое число для размера хеш-таблицы

// Узел для хранения пар ключ-значение
typedef struct HashNode {
    char *key;
    int value;
} HashNode;

// Дубликат строки
char* my_strdup(const char *src) {
    size_t len = strlen(src) + 1; 
    char *dst = malloc(len);
    if (dst == NULL) return NULL;
    memcpy(dst, src, len); 
    return dst;
}

// Создание хеш-таблицы
HashNode* create_hashtable() {
    HashNode *hashtable = (HashNode *)calloc(TABLE_SIZE, sizeof(HashNode)); // Выделение памяти для хеш-таблицы и инициализация нулями
    return hashtable;
}

// Хеш-функция
unsigned int hash(const char *key) {
    unsigned int value = 0;
    for (; *key; key++) {
        value = value * 37 + *key; // Простое умножение и сложение для хеширования
    }
    return value % TABLE_SIZE; // Простая функция хеширования, сокращает хеш до размера таблицы
}

// Вставка в хеш-таблицу
void insert(HashNode *hashtable, const char *key) {
    unsigned int index = hash(key);  // Вычисление индекса
    HashNode *node = &hashtable[index];

    // Проверяем, существует ли уже такой ключ
    while (node->key != NULL) {
        if (strcmp(node->key, key) == 0) {  // Если ключи совпадают, увеличиваем значение
            node->value++;
            return;
        }
        index = (index + 1) % TABLE_SIZE;  // Иначе, переходим к следующему ячейку
        node = &hashtable[index];
    }

    // Если такого ключа нет, создаем новый узел
    node->key = my_strdup(key);
    node->value = 1;
}

// Освобождение памяти для хеш-таблицы
void free_hashtable(HashNode *hashtable) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        HashNode *node = &hashtable[i];
        free(node->key); // Освобождение памяти для ключей
    }
    free(hashtable);
}

// Функция подсчета слов
void count_words(FILE *file, HashNode *hashtable) {
    char buffer[256];
    while (fscanf(file, "%s", buffer) == 1) {
        for (char *p = buffer; *p; ++p) *p = tolower((unsigned char)*p); // Приведение в нижний регистр
        insert(hashtable, buffer); // Вставка слова в хеш-таблицу
    }
}

// Вывод результата
void display(HashNode *hashtable) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (hashtable[i].key != NULL) {
            printf("%s: %d\n", hashtable[i].key, hashtable[i].value);
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <файл>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Ошибка при открытии файла");
        return 1;
    }

    HashNode *hashtable = create_hashtable();

    count_words(file, hashtable);

    display(hashtable);

    free_hashtable(hashtable);

    fclose(file);
    return 0;
}
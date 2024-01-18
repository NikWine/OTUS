#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TABLE_SIZE 10007 // Простое число для размера хеш-таблицы

/* Структура для хранения пары ключ (слово) - значение (частота) */
typedef struct HashNode {
    char *key;
    int value;
    struct HashNode *next;
} HashNode;

/* Функция для создания нового узла */
char* my_strdup(const char *src) {
    size_t len = strlen(src) + 1; // Длина строки + 1 для нуль-терминатора
    char *dst = malloc(len);
    if (dst == NULL) return NULL;
    memcpy(dst, src, len); // Используем memcpy, чтобы скопировать len символов
    return dst;
}
HashNode* new_hashnode(const char *key, int value) {
    HashNode *node = (HashNode *)malloc(sizeof(HashNode));
    if (node == NULL) {
        // Не забывайте проверять, удалось ли выделить память для узла.
        fprintf(stderr, "Ошибка при выделении памяти для нового узла.\n");
        return NULL;
    }
    node->key = my_strdup(key); // Здесь использована функция my_strdup вместо strdup
    if (node->key == NULL) {
        // Если strdup вернул NULL, нужно освободить уже выделенную память для узла.
        fprintf(stderr, "Ошибка при копировании ключа.\n");
        free(node);
        return NULL;
    }
    node->value = value;
    node->next = NULL;
    return node;
}

/* Функция для создания хеш-таблицы */
HashNode** create_hashtable() {
    HashNode **hashtable = (HashNode **)calloc(TABLE_SIZE, sizeof(HashNode *));
    return hashtable;
}

/* Хеш-функция */
unsigned int hash(const char *key) {
    unsigned int value = 0;
    for (; *key; key++) {
        value = value * 37 + *key;
    }
    return value % TABLE_SIZE;
}

/* Функция для вставки в хеш-таблицу */
void insert(HashNode **hashtable, const char *key) {
    unsigned int index = hash(key);
    HashNode *node = hashtable[index];

    // Проверяем, существует ли уже такой ключ
    while (node) {
        if (strcmp(node->key, key) == 0) {
            node->value++;
            return;
        }
        node = node->next;
    }

    // Если такого ключа не было, создаем новый узел
    node = new_hashnode(key, 1);
    node->next = hashtable[index];
    hashtable[index] = node;
}

/* Функция для освобождения памяти хеш-таблицы */
void free_hashtable(HashNode **hashtable) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        HashNode *node = hashtable[i];
        while (node) {
            HashNode *temp = node;
            node = node->next;
            free(temp->key);
            free(temp);
        }
    }
    free(hashtable);
}

/* Функция подсчета слов */
void count_words(FILE *file, HashNode **hashtable) {
    char buffer[256];
    while (fscanf(file, "%s", buffer) == 1) {
        // Приведение слова к нижнему регистру
        for (char *p = buffer; *p; ++p) *p = tolower((unsigned char)*p);
        insert(hashtable, buffer);
    }
}

/* Функция вывода результата */
void display(HashNode **hashtable) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        for (HashNode *node = hashtable[i]; node; node = node->next) {
            printf("%s: %d\n", node->key, node->value);
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

    HashNode **hashtable = create_hashtable();

    count_words(file, hashtable);

    display(hashtable);

    free_hashtable(hashtable);

    fclose(file);
    return 0;
}


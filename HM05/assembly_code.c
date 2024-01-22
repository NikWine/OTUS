#include <stdio.h>
#include <stdlib.h>

// Определение структуры для элемента списка
typedef struct Element {
    long num;
    struct Element *nextElem;
} Element;

// Функция для создания нового элемента списка
Element *createElem(long num, Element *nextElem) {
    // Выделение памяти для нового элемента
    Element *elem = (Element *) malloc(sizeof(Element));
    // Проверка успешности выделения памяти
    if (!elem) {
        fprintf(stderr, "Failed to allocate memory\n");
        exit(EXIT_FAILURE);
    }
    // Установка значений для элемента
    elem->num = num;
    elem->nextElem = nextElem;
    return elem;
}

// Функция для добавления нового элемента в начало списка
Element *addElement(long num, Element *list) {
    return createElem(num, list);
}

// Функция, применяющая функцию ко всем элементам списка
void applyToEach(Element *list, void (*func)(long)) {
    // Проходим по списку и применяем функцию к каждому элементу
    while (list != NULL) {
        func(list->num);
        list = list->nextElem;
    }
    printf("\n");
}

// Функция для проверки, является ли число нечетным
long check(long num) {
    return num & 1;
}

// Функция для создания нового списка, в который включены только элементы, удовлетворяющие условию проверки
Element *filter(Element *list, long (*func)(long)) {
    Element *result = NULL;
    Element *temp, *tail = NULL;

    // Проходим по списку и проверяем каждый элемент
    while (list != NULL) {
        // Если элемент проходит проверку, добавляем его в новый список
        if (func(list->num)) {
            temp = createElem(list->num, NULL);

            if (result == NULL) {
                result = temp;
            }
            else {
                tail->nextElem = temp;
            }

            tail = temp;
        }

        list = list->nextElem;
    }

    return result;
}

// Функция для вывода числа
void print(long num) {
    printf("%ld ", num);
}

// Главная функция программы
int main() {
    // Определение начального списка данных
    long data[] = {4, 8, 15, 16, 23, 42};
    size_t dataNum = sizeof(data) / sizeof(data[0]);

    // Создание нового списка
    Element *list = NULL;
    for (size_t i = 0; i < dataNum; i++) {
        list = addElement(data[i], list);
    }

    // Применение функции print ко всем элементам списка
    applyToEach(list, print);

    // Создание нового списка, содержащего только нечетные элементы
    Element *filteredList = filter(list, check);

    // Применение функции print ко всем элементам отфильтрованного списка
    applyToEach(filteredList, print);

    // Очистка памяти
    while (list != NULL) {
        Element *temp = list;
        list = list->nextElem;
        free(temp);
    }

    while (filteredList != NULL) {
        Element *temp = filteredList;
        filteredList = filteredList->nextElem;
        free(temp);
    }

    return EXIT_SUCCESS;
}
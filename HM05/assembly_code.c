#include <stdio.h>
#include <stdlib.h>

typedef struct Element {
    long num;
    struct Element *nextElem;
} Element;

// Сформировать новый элемент списка.
Element *createElem(long num, Element *nextElem) {
    Element *elem = (Element *)malloc(sizeof(Element));
    if (!elem) {
        exit(0);
    }
    elem->num = num;
    elem->nextElem = nextElem;
    return elem;
}

// Функция, применяющая функцию печати ко всем элементам списка.
void iterateAndApply(Element *elem, void (*print)(long)) {
    while (elem) {
        print(elem->num);
        elem = elem->nextElem;
    }
    printf("\n");
}

// Функция проверки, является ли число нечетным.
int oddCheck(long num) {
    return num & 1;
}

// Функция для создания нового списка, в который включены только элементы, удовлетворяющие условию проверки.
Element *refineList(Element *elem, int (*check)(long)) {
    Element *newlist = NULL;
    Element **end = &newlist;

    while (elem) {
        if (check(elem->num)) {
            *end = createElem(elem->num, NULL);
            end = &((*end)->nextElem);
        }
        elem = elem->nextElem;
    }
    return newlist;
}

// Функция вывода длинного целого числа в консоль.
void displayLong(long num) {
    printf("%ld ", num);
}

long dataset[] = {4, 8, 15, 16, 23, 42};
size_t dataset_length = sizeof(dataset) / sizeof(dataset[0]);

int main() {
    Element *list = NULL;

    // Вставка элементов в список (в начале).
    for (size_t i = dataset_length; i-- > 0; ) {
        list = createElem(dataset[i], list);
    }

    // Применить функцию displayLong к каждому элементу списка.
    iterateAndApply(list, displayLong);

    // Фильтровать список с условием oddCheck и создать новый список только с нечетными числами.
    Element *refinedList = refineList(list, oddCheck);

    // Применить функцию displayLong к каждому элементу отфильтрованного списка.
    iterateAndApply(refinedList, displayLong);

    // Освободить память, выделенную под список.
    while (list) {
        Element *temp = list;
        list = list->nextElem;
        free(temp);
    }

    // Освободить память, выделенную под отфильтрованный список.
    while (refinedList) {
        Element *temp = refinedList;
        refinedList = refinedList->nextElem;
        free(temp);
    }

    return 0;
}
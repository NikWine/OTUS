#include <stdio.h>
#include <stdlib.h>
#include "libpq-fe.h"
#include "postgres_ext.h"
#include "pg_config_ext.h"

// Функция для проверки, является ли строка числом
int is_number(const char *str) {
    if (str == NULL) return 0;
    char *endptr;
    strtod(str, &endptr);
    return *endptr == '\0';
}

// Функция для вычисления статистических данных выбранного столбца в таблице
void calculate_statistics(PGconn *conn, const char *table, const char *column) {
    char query[256];
    snprintf(query, sizeof(query), "SELECT %s FROM %s", column, table);

    // Выполняем SQL запрос
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    // Инициализация переменных для статистики
    double sum = 0.0, min = 0.0, max = 0.0, value;
    int count = 0;
    double mean = 0.0, M2 = 0.0, delta;

    // Итерируем по всем строкам результата запроса
    int nrows = PQntuples(res);
    for (int i = 0; i < nrows; i++) {
        const char *val_str = PQgetvalue(res, i, 0);
        if (val_str && is_number(val_str)) {
            value = atof(val_str);
            sum += value;
            if (count == 0) {
                min = max = value;
            } else {
                if (value < min) min = value;
                if (value > max) max = value;
            }
            count++;
            delta = value - mean;
            mean += delta / count;
            M2 += delta * (value - mean);
        }
    }

    PQclear(res);

    // Если были найдены числовые данные, выводим статистику
    if (count > 0) {
        double variance = count > 1 ? M2 / (count - 1) : 0.0;
        printf("Count: %d\n", count);
        printf("Sum: %f\n", sum);
        printf("Mean: %f\n", mean);
        printf("Min: %f\n", min);
        printf("Max: %f\n", max);
        printf("Variance: %f\n", variance);
    } else {
        printf("В столбце не найдено действительных числовых данных '%s'.\n", column);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <dbname> <user> <password> <table> <column>\n", argv[0]);
        return 1;
    }

    const char *conninfo = argv[1];
    const char *table_name = argv[2];
    const char *column_name = argv[3];

    // Соединение с БД
    PGconn *conn = PQconnectdb(conninfo);
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Не удалось подключиться к базе данных: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }

    // Вычисляем статистику для выбранного столбца в таблице
    calculate_statistics(conn, table_name, column_name);
    PQfinish(conn);
    return 0;
}

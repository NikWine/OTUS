#include <stdio.h>
#include <stdlib.h>
#include "libpq-fe.h"
#include "postgres_ext.h"
#include "pg_config_ext.h"

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

    // Выполняем SQL-запрос для вычисления статистики
    char query[256];
    snprintf(query, sizeof(query), "SELECT COUNT(%s), SUM(%s), AVG(%s), MIN(%s), MAX(%s), VAR_SAMP(%s) FROM %s",
            column_name, column_name, column_name, column_name, column_name, column_name, table_name);

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return 1;
    }

    // Получаем значения статистики из результата запроса
    int count = atoi(PQgetvalue(res, 0, 0));
    double sum = atof(PQgetvalue(res, 0, 1));
    double avg = atof(PQgetvalue(res, 0, 2));
    double min = atof(PQgetvalue(res, 0, 3));
    double max = atof(PQgetvalue(res, 0, 4));
    double variance = atof(PQgetvalue(res, 0, 5));

    PQclear(res);

    // Выводим статистику
    printf("Count: %d\n", count);
    printf("Sum: %f\n", sum);
    printf("Mean: %f\n", avg);
    printf("Min: %f\n", min);
    printf("Max: %f\n", max);
    printf("Variance: %f\n", variance);

    PQfinish(conn);
    return 0;
}

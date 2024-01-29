#include "logger.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <execinfo.h>

static FILE *log_file_ptr = NULL;

void logger_init(const char *log_file) {
    log_file_ptr = fopen(log_file, "a");
    if (log_file_ptr == NULL) {
        fprintf(stderr, "Ошибка открытия файла лога: %s\n", log_file);
        exit(EXIT_FAILURE);
    }
}

void log_message(LogLevel level, const char *file, int line, const char *function, const char *format, ...) {
    if (log_file_ptr == NULL) {
        fprintf(stderr, "Логгер не инициализирован!\n");
        return;
    }

    // Получаем текущее время.
    time_t now;
    time(&now);
    struct tm *local_time = localtime(&now);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local_time);

    // Печатаем заголовок сообщения.
    fprintf(log_file_ptr, "[%s] [%s] %s:%d %s() - ", time_str, (level == DEBUG ? "DEBUG" : (level == INFO ? "INFO" : (level == WARNING ? "WARNING" : "ERROR"))), file, line, function);

    // Печатаем сообщение.
    va_list args;
    va_start(args, format);
    vfprintf(log_file_ptr, format, args);
    va_end(args);
    fprintf(log_file_ptr, "\n");

    // Для уровня ERROR выводим стек вызовов.
    if (level == ERROR) {
        fprintf(log_file_ptr, "Стек вызовов:\n");
        
        void *call_stack[256];
        int count = backtrace(call_stack, sizeof(call_stack) / sizeof(void *));
        char **symbols = backtrace_symbols(call_stack, count);
        if (symbols != NULL) {
            for (int i = 0; i < count; i++) {
                fprintf(log_file_ptr, "%s\n", symbols[i]);
            }
            fprintf(log_file_ptr, "\n");
            free(symbols);
        } else {
            fprintf(log_file_ptr, "Ошибка при получении информации о стеке вызовов.\n");
        }
    }

    fflush(log_file_ptr);
}

void logger_close(void) {
    if (log_file_ptr != NULL) {
        fclose(log_file_ptr);
        log_file_ptr = NULL;
    }
}

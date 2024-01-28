#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

// Уровни важности сообщений лога.
typedef enum LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
} LogLevel;

// Инициализация логгера.
void logger_init(const char *log_file);

// Функция журналирования.
void log_message(LogLevel level, const char *file, int line, const char *function, const char *format, ...);

// Макросы для упрощения журналирования, добавляющие файл, строку и функцию.
#define LOG_DEBUG(format, ...) log_message(DEBUG, __FILE__, __LINE__, __func__, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) log_message(INFO, __FILE__, __LINE__, __func__, format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...) log_message(WARNING, __FILE__, __LINE__, __func__, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) log_message(ERROR, __FILE__, __LINE__, __func__, format, ##__VA_ARGS__)


// Закрытие логгера.
void logger_close(void);

#endif // LOGGER_H

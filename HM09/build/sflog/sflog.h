#ifndef SFLOG_H
#define SFLOG_H

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#define SFL_FATAL 1
#define SFL_ERROR 2
#define SFL_WARNING 4
#define SFL_INFO 8
#define SFL_NOTICE 16
#define SFL_DEBUG 32
#define SFL_ALL 0xFF

#define SFLOG_BACKTRACE_SIZE 128

/**
 * @brief Инициализация с указанием имени лог-файла
 * @return false, если ошибка открытия файла на запись */
bool sfl_init_fpath(const char *path, bool append_mode);

/**
 * @brief Инициализация файловым дискриптором (дескриптор должен быть готов для записи) */
void sfl_init_fstream(FILE *fstream);

/* Очистка ресурсов. */
void sfl_release();

/**
* @brief Задает фильтр обработки по типу сообщения
* @note По умолчанию выводятся все сообщения */
void sfl_set_verbosility(int level_flags);


/**
* @brief Вывод сообщения в лог-файл (служебная функция)
* @param level уровень/тип сообщения
* @param level_name строка для отображения названия уровня
* @param sync если true, то сбрасывает буфер в файл
* @param format printf format string
* @param ... args
*/
void sfl_print(int level, const char *level_name, bool sync, const char *format, ...);

/** Вывод стека функци
* для вывода символьных имен функциий ликуйте программу с ключем -rdynamic
*/
void sfl_print_backtrace();


/// Базовые макрос-функции для вывода в лог
#define sfl_fatal(...)                                                                             \
 do {                                                                                             \
   sfl_print(SFL_ERROR, "FATAL", 0, __VA_ARGS__);                                                 \
   sfl_print_backtrace();                                                                         \
 } while (0);
#define sfl_error(...) sfl_print(SFL_ERROR, "ERROR", 0, __VA_ARGS__)
#define sfl_warn(...) sfl_print(SFL_WARNING, "WARN", 0, __VA_ARGS__)
#define sfl_info(...) sfl_print(SFL_INFO, "INFO", 0, __VA_ARGS__)
#define sfl_note(...) sfl_print(SFL_NOTICE, "NOTE", 0, __VA_ARGS__)
#define sfl_debug(...) sfl_print(SFL_DEBUG, "DEBUG", 0, __VA_ARGS__)


/// Макрос-функции для синхронного вывода в лог.
#define sfl_fatal_sync(...)                                                                        \
 do {                                                                                             \
   sfl_print(SFL_ERROR, "FATAL", 1, __VA_ARGS__);                                                 \
   sfl_print_backtrace();                                                                         \
 } while (0);
#define sfl_error_sync(...) sfl_print(SFL_ERROR, "ERROR", 1, __VA_ARGS__)
#define sfl_warn_sync(...) sfl_print(SFL_WARNING, "WARN", 1, __VA_ARGS__)
#define sfl_info_sync(...) sfl_print(SFL_INFO, "INFO", 1, __VA_ARGS__)
#define sfl_note_sync(...) sfl_print(SFL_NOTICE, "NOTE", 1, __VA_ARGS__)
#define sfl_debug_sync(...) sfl_print(SFL_DEBUG, "DEBUG", 1, __VA_ARGS__)

#endif // SFLOG_H

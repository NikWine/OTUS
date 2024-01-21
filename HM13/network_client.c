#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

#define SERVER_PORT "23" // Port for Telnet protocol
#define MAX_TEXT_LENGTH 1024
#define BUFFER_SIZE 2048

// Вывод инструкций использования программы
void print_usage(const char* program_name) {
    printf("Usage: %s font text...\n", program_name);
}

// Попытка отправить все данные из буфера
ssize_t send_all(int socket, const char *buf, size_t len, int flags) {
    ssize_t total = 0;
    ssize_t n;

    // Продолжить отправку данных пока весь буфер не будет отправлен
    while ((size_t)total < len) {
        n = send(socket, buf + total, len - total, flags);
        if (n == -1) { break; }
        total += n;
    }

    return (n == -1) ? -1 : total;
}

int main(int argc, char *argv[]) {
    // Если количество аргументов меньше 3 (название программы, шрифт и текст), вывести инструкции использования программы и завершить ее
    if (argc < 3) {
        print_usage(argv[0]);
        return 1;
    }

    // Скопировать аргументы в переменные
    const char* font = argv[1];
    char text[MAX_TEXT_LENGTH] = {0};

    // Собирает все аргументы после шрифта в одну строку
    strcpy(text, argv[2]);
    for (int i = 3; i < argc; ++i) {
        strncat(text, " ", MAX_TEXT_LENGTH - strlen(text) - 1);
        strncat(text, argv[i], MAX_TEXT_LENGTH - strlen(text) - 1);
    }

    // Создает подключение к серверу "telehack.com"
    int sock;
    struct addrinfo hints, *servinfo, *p;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo("telehack.com", SERVER_PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }
        if (connect(sock, p->ai_addr, p->ai_addrlen) == -1) {
            close(sock);
            perror("client: connect");
            continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        freeaddrinfo(servinfo);
        return 2;
    }

    freeaddrinfo(servinfo);

    char buffer[BUFFER_SIZE];
    ssize_t numbytes;

    // прочитать приветственное сообщение
    numbytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (numbytes <= 0) {
        if (numbytes == 0) {
            fprintf(stderr, "Connection closed by server.\n");
        } else {
            perror("recv");
        }
        close(sock);
        return 3;
    }
    buffer[numbytes] = '\0';

// пропустить до запроса (это упрощенный метод, ненадежный для общих случаев)    
    while (!strstr(buffer, "Hacker>")) {
        numbytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (numbytes <= 0) {
            if (numbytes == 0) {
                fprintf(stderr, "Connection closed by server.\n");
            } else {
                perror("recv");
            }
            close(sock);
            return 4;
        }
        buffer[numbytes] = '\0';
    }

    // Отправка figlet команды на сервер
    char command[BUFFER_SIZE];
    int command_length = snprintf(command, BUFFER_SIZE, "figlet -f %s %s\r\n", font, text);

    // Обработка ситуации, когда команда слишком длинная или возникла ошибка кодирования
    if (command_length >= BUFFER_SIZE || command_length < 0) {
        fprintf(stderr, "Error: command string too long or encoding error occurred.\n");
        close(sock);
        return 5;
    }

    // Если отправка не удалась, закрыть соединение и завершить программу
    if (send_all(sock, command, strlen(command), 0) == -1) {
        perror("send");
        close(sock);
        return 6;
    }

    // Получение figlet ответа с сервера и вывод его на экран
    while ((numbytes = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[numbytes] = '\0';
        printf("%s", buffer);
        if (strstr(buffer, "Hacker>")) {
            // Приглашение снова получено, вывод завершен
            break;
        }
    }

    if (numbytes == -1) {
        perror("recv");
        close(sock);
        return 7;
    }

    close(sock); // Закрытие соединения
    return 0;    // Успешное завершение программы
}


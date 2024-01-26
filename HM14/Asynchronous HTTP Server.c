#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#define MAX_EVENTS 64
#define BUFFER_SIZE 1024

// Объявление функций
int set_nonblocking(int sockfd);
void send_http_response(int sockfd, const char *status, const char *content_type, const char *body, int body_length);
void send_file_contents(int client_fd, int file_fd);
void handle_http_request(int sockfd, const char *dir);

// Функция для установки сокета в неблокирующий режим
int set_nonblocking(int sockfd) {
    int flags;
    if ((flags = fcntl(sockfd, F_GETFL, 0)) == -1) {
        flags = 0;
    }
    return fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
}

// Функция для отправки содержимого файла
void send_file_contents(int client_fd, int file_fd) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = read(file_fd, buffer, BUFFER_SIZE)) > 0) {
        if (send(client_fd, buffer, bytes_read, 0) == -1) {
            perror("send");
            break;
        }
    }   
}

// Функция для отправки HTTP ответа
void send_http_response(int sockfd, const char *status, const char *content_type, const char *body, int body_length) {
    char header[BUFFER_SIZE];

    if (body == NULL) body = "";
    if (content_type == NULL) content_type = "text/html; charset=utf-8";
    if (body_length < 0) body_length = strlen(body);

    snprintf(header, sizeof(header),
        "HTTP/1.1 %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %d\r\n"
        "\r\n",
        status, content_type, body_length);
    if (send(sockfd, header, strlen(header), 0) == -1) {
        perror("send");
        return;
    }   

    if (body_length > 0 && send(sockfd, body, body_length, 0) == -1) {
    perror("send");
    }   
}

// Функция для обработки HTTP запроса
void handle_http_request(int sockfd, const char *dir) {
    char buffer[BUFFER_SIZE];
    ssize_t nbytes = recv(sockfd, buffer, sizeof(buffer), 0);

    // Проверяем получено ли что-то от sockfd
    if (nbytes <= 0) {
        if (nbytes < 0) {
            perror("recv");
        }
    } else {
        buffer[nbytes] = '\0';

        // Если начальные символы буфера - это 'GET '
        if (strncmp(buffer, "GET ", 4) == 0) {
            char *path = buffer + 4;
            char *end_path = strchr(path, ' ');
        
            if (end_path) {
                *end_path = '\0';
                char full_path[BUFFER_SIZE];
                snprintf(full_path, sizeof(full_path), "%s%s", dir, path);

                // Пытаемся открыть файл
                int file_fd = open(full_path, O_RDONLY);
                if (file_fd == -1) {
                    if (errno == EACCES) {
                        send_http_response(sockfd, "403 Forbidden", "text/plain", "403 Forbidden: Доступ запрещен.", -1);
                    } else {
                        send_http_response(sockfd, "404 Not Found", "text/plain", "404 Not Found: Запрошенный ресурс не найден на этом сервере.", -1);
                    }
                } else {
                    send_http_response(sockfd, "200 OK", "text/plain", NULL, 0);
                    send_file_contents(sockfd, file_fd);
                    close(file_fd);
                }
            }
        } else {
            send_http_response(sockfd, "400 Bad Request", "text/plain", "400 Bad Request: Запрос не может быть выполнен из-за неправильного синтаксиса.", -1);
        }
    }
    close(sockfd);
}

int main(int argc, char *argv[]) {

    // Если не заданы необходимые аргументы, выводим сообщение и завершаем выполнение
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <каталог> <адресс> <порт>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *dir = argv[1];
    const char *address = argv[2];
    int port = atoi(argv[3]);

    // Создаем сокет
    int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    set_nonblocking(listen_sock);

    // Задаем адрес и порт для сокета
    struct sockaddr_in srv_addr;
    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = inet_addr(address);
    srv_addr.sin_port = htons(port);

    // Привязываем сокет к определенному адресу и порту
    if (bind(listen_sock, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    //Если ошибка слушания, выводим сообщение об ошибке и выходим
    if (listen(listen_sock, SOMAXCONN) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Создаем epoll файловый дескриптор
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    struct epoll_event event;
    event.data.fd = listen_sock;
    event.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_sock, &event) == -1) {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }

    struct epoll_event *events = calloc(MAX_EVENTS, sizeof(event));
    if (!events) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    // Основной цикл обработки событий
    while (1) {
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < n; i++) {
            if (events[i].data.fd == listen_sock) {
                while (1) {
                    struct sockaddr in_addr;
                    socklen_t in_len = sizeof(in_addr);
                    int conn_sock = accept(listen_sock, &in_addr, &in_len);
                    if (conn_sock == -1) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            break; // Обработали все соединения
                        } else {
                            perror("accept");
                            break;
                        }
                    }

                    //Устанавливаем неблокирующий флаг для сокета
                    set_nonblocking(conn_sock);
                    event.data.fd = conn_sock;
                    event.events = EPOLLIN | EPOLLET;
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_sock, &event) == -1) {
                        perror("epoll_ctl");
                        close(conn_sock);
                    }
                }    
            } else {

                // Обработка HTTP-запроса 
                handle_http_request(events[i].data.fd, dir);
            }
        }
    }

    close(epoll_fd);
    free(events);
    close(listen_sock);

    return EXIT_SUCCESS;
}

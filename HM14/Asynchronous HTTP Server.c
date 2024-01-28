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

#define MAXEVENTS 64
#define BUFFSIZE 1024

// Сделать сокет неблокирующим
int make_socket_non_blocking(int sock) {
    int flags = fcntl(sock, F_GETFL, 0);
    if (flags == -1) {
        return -1;
    }
    flags |= O_NONBLOCK;
    return fcntl(sock, F_SETFL, flags);
}

// Доставить данные файла
void deliver_file_data(int client, int file) {
    char buffer[BUFFSIZE];
    ssize_t bytes_read;
    while ((bytes_read = read(file, buffer, sizeof(buffer))) > 0) {
        if (send(client, buffer, bytes_read, 0) == -1) {
            perror("Не удалось отправить данные файла");
            break;
        }
    }
}

// Отправить HTTP-ответ
void send_http_response(int fd, const char *status_code, const char *content_type, const char *body, int body_length) {
    char response_header[BUFFSIZE];
    const char *body_content = body ? body : "";
    const char *mime_type = content_type ? content_type : "text/html; charset=utf-8";
    
    int content_length = body_length >= 0 ? body_length : (int)strlen(body_content);

    snprintf(response_header, sizeof(response_header), "HTTP/1.1 %s\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n",
             status_code, mime_type, content_length);
    send(fd, response_header, strlen(response_header), 0);
    if (content_length > 0) {
        send(fd, body_content, content_length, 0);
    }
}

// Функция для обработки HTTP запроса
void handle_http_request(int client_fd, const char *web_root) {
    char req_buffer[BUFFSIZE];
    ssize_t bytes_received = recv(client_fd, req_buffer, sizeof(req_buffer) - 1, 0);
    

    if (bytes_received > 0) {
        req_buffer[bytes_received] = '\0';
        if (strncmp(req_buffer, "GET ", 4) == 0) {
            char *path_start = req_buffer + 4;
            char *path_end = strchr(path_start, ' ');
            if (path_end) {
                *path_end = '\0';
                char full_file_path[BUFFSIZE];
                snprintf(full_file_path, sizeof(full_file_path), "%s%s", web_root, path_start);
                int file_desc = open(full_file_path, O_RDONLY);
                if (file_desc != -1) {
                    send_http_response(client_fd, "200 OK", "text/plain", NULL, -1);
                    deliver_file_data(client_fd, file_desc);
                    close(file_desc);
                } else {
                    // Отправляем ответ 403 Forbidden в случае ошибки доступа или 404 Not Found для остальных ошибок
                    if (errno == EACCES) {
                        send_http_response(client_fd, "403 Forbidden", "text/plain", "403 Forbidden: Доступ запрещен.", -1);
                    } else {
                        send_http_response(client_fd, "404 Not Found", "text/plain", NULL, -1);
                    }
                }
            }
        }
    }
    close(client_fd);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s [port] [web root]\n", argv[0]);
        return EXIT_FAILURE;
    }

    int server_fd, new_client, epoll_fd;
    struct sockaddr_in server_addr;
    struct epoll_event event, events[MAXEVENTS];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Не удалось создать сокет");
        return EXIT_FAILURE;
    }

    int port = atoi(argv[1]);
    const char *web_root = argv[2];

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Could not bind socket");
        return EXIT_FAILURE;
    }

    if (listen(server_fd, SOMAXCONN) == -1) {
        perror("Could not listen on socket");
        return EXIT_FAILURE;
    }

    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("Could not create epoll instance");
        return EXIT_FAILURE;
    }

    event.data.fd = server_fd;
    event.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1) {
        perror("Не удалось добавить серверный сокет в epoll");
        return EXIT_FAILURE;
    }

    // Основной цикл обработки событий
    while (1) {
        int n = epoll_wait(epoll_fd, events, MAXEVENTS, -1);
        for (int i = 0; i < n; i++) {
            if (events[i].data.fd == server_fd) {
                struct sockaddr in_addr;
                socklen_t in_len = sizeof(in_addr);
                new_client = accept(server_fd, &in_addr, &in_len);
                if (new_client == -1) {
                    if ((errno != EAGAIN) && (errno != EWOULDBLOCK)) {
                        perror("Could not accept client connection");
                    }
                    continue;
                }

                if (make_socket_non_blocking(new_client) == -1) {
                    perror("Could not make client socket non-blocking");
                    close(new_client);
                    continue;
                }

                event.data.fd = new_client;
                event.events = EPOLLIN | EPOLLET;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_client, &event) == -1) {
                    perror("Не удалось добавить клиентский сокет в epoll");
                    close(new_client);
                    continue;
                }
            } else {
                handle_http_request(events[i].data.fd, web_root);
            }
        }
    }

    close(server_fd);
    return EXIT_SUCCESS;
}

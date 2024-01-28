#include "logger.h"

void test_function() {
    LOG_ERROR("Пример сообщения об ошибке %s", "");
}

int main() {
    logger_init("app.log");

    LOG_DEBUG("Пример отладочного сообщения %s", "");
    LOG_INFO("Пример информационного сообщения %s", "");
    LOG_WARNING("Пример предупреждения %s", "");

    test_function();

    logger_close();
    return 0;
}

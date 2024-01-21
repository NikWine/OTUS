#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

typedef struct {
    char *data;
    size_t length;
} Buffer;

static size_t BufferCallback(void *received, size_t size, size_t nmemb, void *userp) {
    size_t totalSize = size * nmemb;
    Buffer *mem = (Buffer *)userp;

    char *temp = realloc(mem->data, mem->length + totalSize + 1);
    if (temp == NULL) {
        printf("не хватает памяти (realloc вернул NULL)\n");
        return 0;
    }

    mem->data = temp;
    memcpy(&(mem->data[mem->length]), received, totalSize);
    mem->length += totalSize;
    mem->data[mem->length] = 0; 

    return totalSize;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Корректное использование: %s <city>\n", argv[0]);
        return 1;
    }

    int urlSize = strlen("http://wttr.in/?format=j1") + strlen(argv[1]) + 1;
    char *url = malloc(sizeof(char) * urlSize);
    if (url == NULL) {
        fprintf(stderr, "Память под URL не выделена\n");
        return 1;
    }
    snprintf(url, urlSize, "http://wttr.in/%s?format=j1", argv[1]);

    CURL *curl;
    CURLcode result;

    Buffer response;
    response.data = malloc(1);
    response.length = 0;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, BufferCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    result = curl_easy_perform(curl);

    if (result != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() не удалось: %s\n", curl_easy_strerror(result));
    } else {
        cJSON *jsonData = cJSON_Parse(response.data);
        if (jsonData == NULL) {
            const char *error = cJSON_GetErrorPtr();
            if (error != NULL) {
                fprintf(stderr, "Ошибка при парсинге JSON: %s\n", error);
            }
        } else {
            cJSON *currentConditionArr = cJSON_GetObjectItemCaseSensitive(jsonData, "current_condition");
            if (currentConditionArr != NULL && cJSON_IsArray(currentConditionArr)) {
                cJSON *currentCondition = cJSON_GetArrayItem(currentConditionArr, 0);
                if (currentCondition != NULL) {
                    cJSON *temp_C = cJSON_GetObjectItemCaseSensitive(currentCondition, "temp_C");
                    cJSON *weatherDesc = cJSON_GetObjectItemCaseSensitive(currentCondition, "weatherDesc");
                    cJSON *winddir16Point = cJSON_GetObjectItemCaseSensitive(currentCondition, "winddir16Point");
                    cJSON *windspeedKmph = cJSON_GetObjectItemCaseSensitive(currentCondition, "windspeedKmph");
                    cJSON *desc = NULL;
                    if (weatherDesc != NULL && cJSON_IsArray(weatherDesc)) {
                        desc = cJSON_GetArrayItem(weatherDesc, 0);
                    }
                    if (desc != NULL) {
                        desc = cJSON_GetObjectItemCaseSensitive(desc, "value");
                    }

                    if (temp_C != NULL && desc != NULL && winddir16Point != NULL && windspeedKmph != NULL) {
                        fprintf(stdout, "Температура сейчас: %s°C\n", temp_C->valuestring);
                        fprintf(stdout, "Описание погоды: %s\n", desc->valuestring);
                        fprintf(stdout, "Направление ветра: %s\n", winddir16Point->valuestring);
                        fprintf(stdout, "Скорость ветра: %s Км/ч\n", windspeedKmph->valuestring);
                    }
                }
            }
            cJSON_Delete(jsonData);
        }

        curl_easy_cleanup(curl);
        free(response.data);
        curl_global_cleanup();

        free(url);

        return 0;
    }
}
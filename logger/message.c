// Подключаем заголовочый файл с фукнциями 
#include "message.h"

// Заголовчный файл для работы с malloc, free и т.д.
#include <malloc.h>

// Заголовочный файл для работы со строчками
#include <string.h>

// Подключаем заголовочный файл с НАШИМИ кодами ошибок
#include "errors.h"

// Функция для инициализации структуры Message
int message_init(struct Message* message, const enum Importance importance, const char* text)
{
    // Приравниваем importance
    message->importance = importance;

    /*
        Функция strlen находится в заголовочном файле string.h
        strlen вычисляет длину строчки БЕЗ null-terminated символ
        strlen работает за O(n)
        По этой причине я делаю +1, чтобы выделить место еще и null-terminated символ
    */
    const size_t text_length = strlen(text) + 1;
    // Выделяем память на куче
    message->text = malloc(text_length * sizeof(char));
    // ОБЯЗАТЕЛЬНО проверяем, что память выделилась
    if (message->text == NULL) {
        return -NOT_ENOUGH_MEMORY;
    }

    /*
        Функция strcpy также находится в заголовочном файле string.h
        Она копирует одну строчку в другую

        Аналогично данную функцию можно переписать с помощью цикла for:

        for (size_t i = 0; i < text_length; i++) {
            message->text[i] = text[i];
        }
    */
    strcpy(message->text, text);

    return 0;
}

// Функция для очистки структуры Message
void message_free(struct Message* message)
{
    // Очистка message->text (не забываем, что он указывает на кучу)
    free(message->text);
}

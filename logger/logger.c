// Подключем заголовочный файл для работы с Logger и его функциями
#include "logger.h"

// Заголовчный файл для работы с malloc, free и т.д.
#include <malloc.h>

// Заголовочный файл для работы с вводом и выводом
#include <stdio.h>

// Заголовочный файл для #define bool
#include <stdbool.h>

// Заголовочный файл для работы с функциями для строчки
#include <string.h>

// Подключаем заголовочный файл с НАШИМИ кодами ошибок
#include "errors.h"

// Подключаем заголовочый файл с фукнциями 
#include "message.h"

// Функция, которая инициализирует logger
int logger_init(struct Logger* logger, FILE* file, const enum Importance importance, const size_t message_capacity, const size_t time_flush)
{
    // Устанавливаем кол-во message в 0 (т.к. никто еще не записал в файл)
    logger->message_amount = 0;
    // Устанавливем capacity
    logger->message_capacity = message_capacity;
    // Устанавливаем importance
    logger->importance = importance;
    // Устанавливаем time_flush
    logger->time_flush = time_flush;
    // Устанавливаем указатель на сообщение
    logger->message_pointer = 0;
    // Устанавливаем файл
    logger->file = file;

    /*
        Выделяем память под сообщения
        Заметьте, что т.к. мы выделяем message_capacity сообщений (речь про кол-во), а sizeof(struct Message) - сколько байт занимает struct Message,
        то формула получется message_capacity * sizeof(struct Message)
    */
    logger->messages = malloc(message_capacity * sizeof(struct Message));
    // Проверяем, выделилась ли нужная память
    if (logger->messages == NULL) {
        return -NOT_ENOUGH_MEMORY;
    }
    
    return 0;
}

/*
    Функция, которая показывает, нужно ли увеличить выделенную память по формуле
    static - т.к. функция нужна только в данном файле, т.е. пользователь (другой программист) не будет знать про данную функцию, когда использует наш логгер
*/
static bool need_to_grow(struct Logger* logger)
{
    return logger->message_amount * CAPACITY_GROW_COEF >= logger->message_capacity;
}

/*
    Функция для перевыделения памяти под message
    static - т.к. функция нужна только в данном файле, т.е. пользователь (другой программист) не будет знать про данную функцию, когда использует наш логгер
*/
static int grow_messages(struct Logger* logger, const size_t new_capacity)
{
    // Приравниваем новое capacity
    logger->message_capacity = new_capacity;
    
    // Перевыделяем память, обратите внимание на sizeof()
    struct Message* new_messages = realloc(logger->messages, new_capacity * sizeof(struct Message));
    // Делаем проверку
    if (new_messages == NULL) {
        return -NOT_ENOUGH_MEMORY;
    }
    
    /* 
        ВАЖНОЕ УТОЧНЕНИЕ: заметьте, что я не писал сразу logger->messages = realloc(...)
        Почему?
        Т.к. realloc может вернуть NULL, то при такой записи мы можем потерять указатель на старые данные, что приведет к утечке данных

        P.s. старую память не надо очищать, т.к. это делает realloc
    */
    logger->messages = new_messages;

    return 0;
}

// Функция записи в logger
int logger_write(struct Logger* logger, const enum Importance importance, const char* text)
{
    // Увеличиваем messages, если требуется
    if (need_to_grow(logger) == true) {
        int err = grow_messages(logger, logger->message_capacity * CAPACITY_GROW_COEF);
        if (err != 0) {
            return err;
        }
    }

    // Инициализируем новый message. Заметьте, что память под Message уже выделена ранее, надо ее просто "заполнить" нужными данными
    int err = message_init(&logger->messages[logger->message_amount++], importance, text);
    if (err != 0) {
        return err;
    }

    // Пишем в файл раз в какое-то время
    if (logger->message_amount % logger->time_flush == 0) {
        logger_flush(logger);
    }

    return 0;
}

/*  
    Функция, которая пишет сообщение внутрь файла
    static - т.к. функция нужна только в данном файле, т.е. пользователь (другой программист) не будет знать про данную функцию, когда использует наш логгер
*/
static int logger_write_message(const struct Logger* logger, const struct Message* message)
{
    // Если сообщение по важности меньше, чем важность, которую мы задали, то просто выходим из функции
    if (logger->importance > message->importance) {
        return 0;
    }

    // Запись в файл. Используется функция fprintf. Она такая же, как и printf, просто нужно еще указать КУДА будет запись
    int res;
    switch (message->importance) {
        case LOW:
            res = fprintf(logger->file, "LOW: %s\n", message->text);
            break;
        case MEDIUM:
            res = fprintf(logger->file, "MEDIUM: %s\n", message->text);
            break;
        case HIGH:
            res = fprintf(logger->file, "HIGH: %s\n", message->text);
            break;
    }

    // Проверяем, была ли ошибка при записи
    if (res < 0) {
        return -FILE_WRITING_ERROR;
    }

    return 0;
}

// Функция для записи данных в файл из logger
int logger_flush(struct Logger* logger)
{
    // Идем по каждому сообщению и записываем в файл
    for (; logger->message_pointer < logger->message_amount; logger->message_pointer++) {
        struct Message* msg = &logger->messages[logger->message_pointer];
        int err = logger_write_message(logger, msg);
        if (err != 0) {
            return err;
        }
    }

    return 0;
}

// Функция для очистки ресурсов logger'а
void logger_free(struct Logger* logger)
{
    /* 
        Заметьте, что мы идем до MESSAGE_AMOUNT, а не до MESSAGE_CAPACITY
        Почему?
        Т.к. мы выдлелили "заполнили" только message_amount message
    */
    for (size_t i = 0; i < logger->message_amount; i++) {
        message_free(&logger->messages[i]);
    }

    // Не забываем почистить память из-под messages
    free(logger->messages);
}
// Конструкция для предотвращения повторного включения. Аналог #pragma once
#ifndef LOGGER_H
#define LOGGER_H

// Заголовчный файл для работы с enum Importance, struct Message и их функциями
#include "message.h"

// Заголовочный файл для работы с FILE
#include <stdio.h>

// Коэф. возрастания capacity
#define CAPACITY_GROW_COEF 2

// Структура Logger
struct Logger
{
    // Если сообщения будут иметь более низкий приоритет, чем данный, то сообщение игнорируется
    enum Importance importance;

    // Длина нашего "массива" на куче
    size_t message_amount;

    // Количество выделленых message на куче
    size_t message_capacity;

    // Указатель на текущий Message (чтобы мы не писали одно и тоже несколько раз)
    size_t message_pointer;

    // Указатель на struct Message
    struct Message* messages;

    // Файл, куда мы будет писать
    FILE* file;

    // Переменная, отвещающая раз в сколько записей будет происходить запись в файлов
    size_t time_flush;
};

// Функция, которая инициализирует logger
int logger_init(struct Logger* logger, FILE* file, const enum Importance importance, const size_t message_capacity, const size_t time_flush);

// Функция записи в logger
int logger_write(struct Logger* logger, const enum Importance importance, const char* text);

// Функция для записи данных в файл из logger
int logger_flush(struct Logger* logger);

// Функция для очистки ресурсов logger'а
void logger_free(struct Logger* logger);

#endif
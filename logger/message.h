// Конструкция для предотвращения повторного включения. Аналог #pragma once
#ifndef MESSAGE_H
#define MESSAGE_H

/* 
    Тип данных ПЕРЕЧИСЛЕНИЕ, которое может принимать только значения - LOW, MEDIUM и HIGH
    Числа нужны для того, чтобы мы могли использовать операторы <, > и др.
*/
enum Importance
{
    LOW = 0,
    MEDIUM = 1,
    HIGH = 2,
};

// Структура данных, которая будет описывать сообщение пользователя
struct Message
{
    // Переменная типа enum Importance
    enum Importance importance;

    // Т.к. мы не знаем длины сообщения, то нам остается сделать только указатель, который будет указывать на память в КУЧЕ
    char* text;
};

// Функция для инициализации структуры Message
int message_init(struct Message* message, const enum Importance importance, const char* text);

// Функция для очистки структуры Message
void message_free(struct Message* message);

#endif
// Заголовочный файл для работы с вводом и выводом
#include <stdio.h>

// Заголовочный файл для работы с функциями для строчки
#include <string.h>

// Заголовочный файл для #define bool
#include <stdbool.h>

// Подключаем наш Message 
#include "logger/message.h"

// Подключаем наш Logger
#include "logger/logger.h"

// Создаем "синоним" для числа 256
#define BUFFER_LENGTH 256

// Создаем "синоним" для строчки "q"
#define EXIT_COMMAND "q"

// Функция, которая переводит const char* в enum Importance
static int str_to_importance(const char* text, enum Importance* importance)
{
    /* 
        Функция strcmp находится в <string.h>
        Если данная функция возвращает 0, то строчки равны
    */
    if (strcmp(text, "LOW") == 0) {
        *importance = LOW;
        return 0;
    // Сравниваем buffer c MEDIUM, и если это правда, то завершаем цикл
    } else if (strcmp(text, "MEDIUM") == 0) {
        *importance = MEDIUM;
        return 0;
    // Сравниваем buffer c HIGH, и если это правда, то завершаем цикл
    } else if (strcmp(text, "HIGH") == 0) {
        *importance = HIGH;
        return 0;
    }

    // Если у нас невалидное значние, то возвращаем -1, показывая, что функция завершилась с ошибкой
    return -1;
}

/*
    Функция проверяет находится ли в buffer команда для выхода
    Функции, которые не будут использованы в других файлах принято объявлять, как static
    static - "делает вашу функцию видимой только в данном файле"
*/
static bool check_exist_command(const char* buffer)
{
    // Смотрим, является ли buffer командой для выхода
    if (strcmp(buffer, EXIT_COMMAND) == 0) 
    {
        return true;
    }

    return false;
}

/* 
    Функция читает из консоли, пока пользователь не введет LOW, MEDIUM, HIGH или EXIT_COMMAND ("q")
    Т.к. функция может возвращать только одно значение, а мы условились на том, что функция будет возвращать 
    появилась ли ошибка в ходе выполнения фукнции (так было на лекции, я слышал).
    ПОЭТОМУ наша функция будет также принимать УКАЗАТЕЛЬ на enum Importance и УКАЗАТЕЛЬ на bool (была ли введена команда выхода из программы)
*/
static int read_importance(char* buffer, enum Importance* importance, bool* need_to_exit)
{
    // Запускаем бесконечный цикл
    while (1) {
        // Считываем с консоли
        int res = scanf("%s", buffer);
        if (res != 1) {
            return -1;
        }

        // Проверяем, является ли ввод командой для выхода из программы
        if (check_exist_command(buffer) == true) {
            // Помечаем, что пользователь ввел команду для выхода
            *need_to_exit = true;
            return 0;
        }

        // Сравниваем buffer c LOW, и если это правда, то завершаем цикл
        if (strcmp(buffer, "LOW") == 0) {
            *importance = LOW;
            break;
        // Сравниваем buffer c MEDIUM, и если это правда, то завершаем цикл
        } else if (strcmp(buffer, "MEDIUM") == 0) {
            *importance = MEDIUM;
            break;
        // Сравниваем buffer c HIGH, и если это правда, то завершаем цикл
        } else if (strcmp(buffer, "HIGH") == 0) {
            *importance = HIGH;
            break;
        } else {
            // Пользователь ввел что-то не то, поэтому мы просим его повторить ввод
            puts("Unknown importance, please repeat input");
        }
    }

    return 0;
}

/*
    Функция для чтения пользовательского ввода (при запуске программы)
    static т.к. больше нигде эта функция не нужна
*/
static int read_input(const int argc, const char* str_importance, enum Importance* importance)
{
    // Проверяем кол-во аргументов
    if (argc != 3) {
        puts("Invalid argument amount. Example: ./app <file> [LOW|MEDIUM|HIGH]");
        return -1;
    }

    // Пытаемся прочитать importance, которая пойдем в logger
    int err = str_to_importance(str_importance, importance);
    if (err != 0) {
        puts("Invalid importance, example: [LOW|MEDIUM|HIGH]");
        return err;
    }

    return 0;
}

static int work(const enum Importance importance, const char* path_to_file)
{
    int status_code = 0;
    // Выделяем массив длины BUFFER_LENGTH под ввод пользователя
    char buffer[BUFFER_LENGTH];

    FILE* file = fopen(path_to_file, "wa");
    if (file == NULL) {
        // Функция puts работает точно также, как и printf только без формата
        puts("Error while opening input file");
        return -1;
    }

    struct Logger logger;
    int err = logger_init(&logger, file, importance, 10, 5);
    if (err != 0) {
        status_code = err;
        goto close_file;
    }

    // Запускаем бесоконечный цикл, пока пользователь не введет q
    // Да, можно поставить любое число, которое не равно 0
    while (1) {
        // Создаем флаг, по которому мы будем смотреть, ввел ли пользователь EXIT_COMMAND
        bool need_to_exit;
        puts("Please, write importance..");
        // Создаем переменную, в которую запишем importance, которую ввел пользователь
        enum Importance importance;
        err = read_importance(buffer, &importance, &need_to_exit);
        if (err != 0) {
            puts("Error while reading from console");
            status_code = err;
            goto free_logger_and_close_file;
        }

        // Нужно завершить цикл, если пользователь ввел COMMAND_EXIT
        if (need_to_exit == true) {
            break;
        }

        puts("Please, write message without whitespaces..");
        // Читаем строчку c консоли
        int res = scanf("%s", buffer);
        if (res != 1) {
            // Функция puts работает точно также, как и printf только без формата
            puts("Error while reading from console");
            status_code = -1;
            goto free_logger_and_close_file;
        }

        // Ввёл ли пользователь EXIT_COMMAND
        if (check_exist_command(buffer) == true) {
            break;
        }

        err = logger_write(&logger, importance, buffer);
        if (err != 0) {
            status_code = err;
            goto free_logger_and_close_file;
        }
    }

    // Не забываем сбросить на файл, то что осталось
    logger_flush(&logger);

free_logger_and_close_file:
    logger_free(&logger);

close_file:
    err = fclose(file);
    if (err != 0) {
        // Т.к. мы ничего сделать не можем, то просто сообщим пользователю, что произошла ошибка, и работаем дальше
        puts("Error while closing file");
    }

    return status_code;
}

/*
    Программа на языке C/C++ также может иметь вот такую сигнатуру функции main
    argc указывает на кол-во входных аргументов. Пример: ./app text1 text2 text3 => argc = 4 (заметьте, что не 3!)
    argv - это указатель на аргументы. Пример: ./app text1 text2 text3 => argv[0] = app, argv[1] = text1, argv[2] = text2, argv[3] = 3
 */
int main(const int argc, const char** argv)
{
    // Переменная, которую мы будем использовать создания логгера
    enum Importance importance;

    int err = read_input(argc, argv[2], &importance);
    if (err != 0) {
        return err;
    }

    return work(importance, argv[1]);
}
# Logger

Пример простого логгера для любимых студентов ❤️

### Обновить репозиторий

```bash
git pull
```

### Cборка

```bash
clang -g -Wall -Wextra -Werror ./logger/logger.c ./logger/message.c main.c -o app
```

### Запуск

```bash
./app \<file\> [LOW|MEDIUM|HIGH]
```
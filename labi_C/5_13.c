/*
5.13. Напишите программу, которая через аргументы командной строки получает
имена и аргументы для запуска внешних программ (произвольное их количество),
разделённые параметром, состоящим из двух символов «;»; чтобы при запуске
командный интерпретатор не считал параметр «;;» имеющим особый смысл, заключайте
его в апострофы, например:
`./prog ls -1 / ';;' sleep 10 ';;' cat file1.txt file2.txt`
Ваша программа должна запустить на одновременное (параллельное) исполнение все
указанные программы с заданными аргументами и напечатать имена тех из них,
которые завершились успешно, то есть вызовом _exit с параметром 0. Печатать
имена следует по мере завершения запущенных программ. Закончить работу следует
сразу после завершения последней из запущенных программ, но не раньше.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

// Функция для запуска команды
void run_command(char **command) {
    pid_t pid = fork(); // Создаем новый процесс
    if (pid == 0) { // Дочерний процесс
        execvp(command[0], command); // Заменяем текущий процесс на команду
        // Если execvp вернул управление, значит произошла ошибка
        perror("execvp");
        exit(EXIT_FAILURE);
    } else if (pid < 0) { // Ошибка при создании процесса
        perror("fork");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s command1 arg1 ';;' command2 arg2 ';;' ...\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Массив для хранения PID запущенных процессов
    pid_t pids[argc];
    int pid_count = 0;

    // Разделяем аргументы на команды по разделителю ';;'
    for (int i = 1; i < argc; ) {
        if (strcmp(argv[i], ";;") == 0) {
            i++; // Пропускаем разделитель
            continue;
        }

        // Собираем команду
        char **command = malloc((argc + 1) * sizeof(char *));
        int j = 0;
        while (i < argc && strcmp(argv[i], ";;") != 0) {
            command[j++] = argv[i++];
        }
        command[j] = NULL; // Завершаем массив NULL

        // Запускаем команду
        pid_t pid = fork();
        if (pid == 0) { // Дочерний процесс
            execvp(command[0], command);
            perror("execvp");
            exit(EXIT_FAILURE);
        } else if (pid < 0) { // Ошибка при создании процесса
            perror("fork");
            exit(EXIT_FAILURE);
        } else { // Родительский процесс
            pids[pid_count++] = pid; // Сохраняем PID дочернего процесса
        }

        free(command); // Освобождаем память
    }

    // Ожидаем завершения всех дочерних процессов
    for (int i = 0; i < pid_count; i++) {
        int status;
        waitpid(pids[i], &status, 0); // Ждем завершения процесса
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            printf("Command with PID %d completed successfully.\n", pids[i]);
        }
    }

    return EXIT_SUCCESS;
}
/*
5.26. Напишите программу, которая через аргументы командной строки получает
имена и аргументы для запуска двух внешних программ, разделённые параметром,
состоящим из двух символов «;», как в задаче 5.13, и запускает эти программы на
одновременное (параллельное) выполнение, связав их конвейером, т. е. стандартный
вывод первой программы должен идти на стандартный ввод второй программы.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

// Разделяем аргументы на две команды по разделителю ;;
void split_args(char **argv, char ***cmd1, char ***cmd2) {
    int i = 1;
    while (argv[i] != NULL && strcmp(argv[i], ";;") != 0) i++;
    
    if (argv[i] == NULL || i == 1 || argv[i+1] == NULL) {
        fprintf(stderr, "Invalid arguments. Usage: %s cmd1 args... ;; cmd2 args...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Первая команда (до ;;)
    *cmd1 = &argv[1];
    argv[i] = NULL;

    // Вторая команда (после ;;)
    *cmd2 = &argv[i+1];
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s cmd1 args... ;; cmd2 args...\n", argv[0]);
        return EXIT_FAILURE;
    }

    char **cmd1, **cmd2;
    split_args(argv, &cmd1, &cmd2);

    int pipefd[2];
    if (pipe(pipefd)) {
        perror("pipe");
        return EXIT_FAILURE;
    }

    // Первый процесс (cmd1)
    pid_t pid1 = fork();
    if (pid1 == 0) {
        close(pipefd[0]);       // Закрываем читающий конец
        dup2(pipefd[1], 1);     // Перенаправляем stdout в pipe
        close(pipefd[1]);

        execvp(cmd1[0], cmd1);
        perror("execvp cmd1");
        exit(EXIT_FAILURE);
    } else if (pid1 < 0) {
        perror("fork");
        return EXIT_FAILURE;
    }

    // Второй процесс (cmd2)
    pid_t pid2 = fork();
    if (pid2 == 0) {
        close(pipefd[1]);       // Закрываем записывающий конец
        dup2(pipefd[0], 0);     // Перенаправляем stdin из pipe
        close(pipefd[0]);

        execvp(cmd2[0], cmd2);
        perror("execvp cmd2");
        exit(EXIT_FAILURE);
    } else if (pid2 < 0) {
        perror("fork");
        return EXIT_FAILURE;
    }

    // Родительский процесс
    close(pipefd[0]);
    close(pipefd[1]);

    // Ждем завершения обоих процессов
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return EXIT_SUCCESS;
}
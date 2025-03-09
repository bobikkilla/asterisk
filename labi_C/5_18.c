/*
5.18. Напишите программу, которая принимает не менее двух аргументов командной
строки и рассматривает первый аргумент - как имя файла, остальные - как имя и
аргументы для запуска внешней программы; например, если ваша программа
называется prog и пользователь запустил её командой
`./prog file1.txt 1s -1 -R`
то здесь имеется в виду файл file1.txt, внешняя программа ls и аргументы -1 -R.
Запустите указанную внешнюю программу с указанными аргументами так, чтобы:

a) её стандартный вывод был перенаправлен в указанный файл (если такого файла
нет, он должен быть создан, если он уже есть - перезаписан; если открыть файл не
удалось, должна быть выдана диагностика, соответствующая возникшей ситуации);

b) её стандартный ввод шёл из указанного файла (если такого файла нет, должна
быть зафиксирована ошибка и выдана соответствующая диагностика);

c) стандартный вывод был перенаправлен в указанный файл на добавление в конец
(если файла нет, создайте его, но если он есть, новое содержимое должно быть
добавлено к старому);

d) стандартный вывод был перенаправлен в указанный файл на добавление
в конец, причём если такого файла нет, должна быть зафиксирована ошибка.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>

// Функция для запуска внешней программы с перенаправлением ввода/вывода
void run_program(const char *file, char *const argv[], int task) {
    pid_t pid = fork(); // Создаем новый процесс
    if (pid == 0) { // Дочерний процесс
        int fd;

        // Обработка задачи a: стандартный вывод в файл (перезапись или создание)
        if (task == 'a') {
            fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644); // Создаем или перезаписываем файл
            if (fd == -1) {
                perror("open (task a)");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO); // Перенаправляем stdout в файл
            close(fd);
        }

        // Обработка задачи b: стандартный ввод из файла (файл должен существовать)
        if (task == 'b') {
            fd = open(file, O_RDONLY); // Открываем файл только для чтения
            if (fd == -1) {
                perror("open (task b)");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDIN_FILENO); // Перенаправляем stdin из файла
            close(fd);
        }

        // Обработка задачи c: стандартный вывод в файл (добавление или создание)
        if (task == 'c') {
            fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0644); // Создаем или добавляем в файл
            if (fd == -1) {
                perror("open (task c)");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO); // Перенаправляем stdout в файл
            close(fd);
        }

        // Обработка задачи d: стандартный вывод в файл (добавление, файл должен существовать)
        if (task == 'd') {
            fd = open(file, O_WRONLY | O_APPEND); // Открываем файл только для добавления
            if (fd == -1) {
                perror("open (task d)");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO); // Перенаправляем stdout в файл
            close(fd);
        }

        // Запускаем внешнюю программу
        execvp(argv[0], argv);
        perror("execvp"); // Если execvp вернул управление, значит произошла ошибка
        exit(EXIT_FAILURE);
    } else if (pid < 0) { // Ошибка при создании процесса
        perror("fork");
        exit(EXIT_FAILURE);
    } else { // Родительский процесс
        waitpid(pid, NULL, 0); // Ждем завершения дочернего процесса
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <file> <program> [args...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *file = argv[1]; // Имя файла
    char * program_args[argc - 1]; // Аргументы для внешней программы

    // Формируем массив аргументов для внешней программы
    for (int i = 2; i < argc; i++) {
        program_args[i - 2] = argv[i];
    }
    program_args[argc - 2] = NULL; // Завершаем массив NULL

    // Запуск задач
    printf("Task a: Redirect stdout to file (overwrite or create)\n");
    run_program(file, program_args, 'a');

    printf("Task b: Redirect stdin from file (file must exist)\n");
    run_program(file, program_args, 'b');

    printf("Task c: Redirect stdout to file (append or create)\n");
    run_program(file, program_args, 'c');

    printf("Task d: Redirect stdout to file (append, file must exist)\n");
    run_program(file, program_args, 'd');

    return EXIT_SUCCESS;
}
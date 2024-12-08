/*
5.12. Напишите программу, которая принимает произвольное количество (не менее
одного) аргументов командной строки и рассматривает их как имя и аргументы для
запуска внешней программы; например, если ваша программа называется prog и
пользователь запустил её командой ./prog ls -1 -R то ваша программа должна
запустить программу ls с аргументами -1 -R. Сделайте так, чтобы в зависимости от
того, как запущенная программа завершится, ваша программа печатала либо слово
exited и код завершения, либо слово killed и номер сигнала, которым запущенная
программа была убита.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if(argc == 1) {
        printf("program: too little args\n");
        exit(EXIT_FAILURE);
    } // checking if any args given
    
    /* size_t total_len = 0;
    for (int i = 1; i < argc; i++) {
        total_len += sizeof(argv[i]) + 1;
    } // counting total lenght of command */
    
    char *bobik = malloc(1024);
    if(!bobik) {
        printf("prog: error allocating memory\n");
        exit(EXIT_FAILURE);
    } // if error with malloc
    for(int i = 1; i < argc; i++) { 
        strcat(bobik, argv[i]);
        if(i != (argc-1)) strcat(bobik, " ");
    }
    
    int status = system(bobik);
    
    if(WEXITSTATUS(status) == 0) {
        printf("\nexited(%d)", WEXITSTATUS(status));
    }
    else {
        printf("\nkilled(%d)", WEXITSTATUS(status));
    } 
    return 0;
}
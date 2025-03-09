/*
5.20. Напишите программу, которая после запуска выдаёт сообщение «Press Ctrl-C
to quit» и после этого ничего не делает, но и не завершается; при нажатии Ctrl-С
выдаёт сообщение «Good bye» и завершается. Обязательно убедитесь с помощью
программы top, что ваша программа во время своего «ничегонеделания» не
потребляет процессорное время.
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handle_sigint(int sig) {
    printf("\nGood bye\n");
    exit(0); 
}

int main() {
    signal(SIGINT, handle_sigint);
    printf("press Ctrl + C to quit\n");
    while (1) {
        pause(); 
    }
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RULES 100
#define MAX_LINE 256
#define MAX_ANSWERS 10

// Структура правила
typedef struct {
    char condition[MAX_LINE];
    char recommendation[MAX_LINE];
} Rule;

// Функция для очистки буфера ввода
void clear_input() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Загрузка правил из файла
int load_rules(const char *filename, Rule rules[]) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Ошибка: не удалось открыть файл %s\n", filename);
        return 0;
    }

    int rule_count = 0;
    char line[MAX_LINE];

    while (fgets(line, sizeof(line), fp)) {
        // Пропуск комментариев и пустых строк
        if (line[0] == '#' || line[0] == '\n') continue;

        // Поиск стрелки "=>"
        char *arrow = strstr(line, "=>");
        if (!arrow) continue;

        // Разделение условия и рекомендации
        strncpy(rules[rule_count].condition, line, arrow - line);
        strcpy(rules[rule_count].recommendation, arrow + 2);

        // Удаление лишних пробелов и переносов строки
        strtok(rules[rule_count].condition, " \t\n");
        strtok(rules[rule_count].recommendation, "\n");

        rule_count++;
    }

    fclose(fp);
    return rule_count;
}

// Проверка соответствия условия и ответов
int match_condition(char *condition, int answers[]) {
    if (strlen(condition) == 0) return 1; // default правило

    char copy[MAX_LINE];
    strcpy(copy, condition);

    char *token = strtok(copy, " ");
    while (token != NULL) {
        if (strstr(token, "singleplayer=")) {
            int value = strstr(token, "=yes") ? 1 : 0;
            if (value != answers[2]) return 0;
        } else if (strstr(token, "mobile=")) {
            int value = strstr(token, "=yes") ? 1 : 0;
            if (value != answers[8]) return 0;
        } else if (strstr(token, "budget=")) {
            int value = strstr(token, "=no") ? 1 : 0;
            if (value != (answers[9] == 0)) return 0;
        } else {
            int matched = 0;
            for (int i = 0; i < MAX_ANSWERS; i++) {
                if (strstr(token, "yes") && i == atoi(token + 6)) {
                    if (answers[i]) {
                        matched = 1;
                        break;
                    }
                }
            }
            if (!matched) return 0;
        }
        token = strtok(NULL, " ");
    }
    return 1;
}

int main() {
    int answers[MAX_ANSWERS] = {0}; // 0 = no, 1 = yes
    Rule rules[MAX_RULES];
    int rule_count;

    // Загрузка правил
    rule_count = load_rules("rules.txt", rules);
    if (rule_count == 0) {
        printf("Нет доступных правил.\n");
        return 1;
    }

    printf("=== экспертная система по рекомендации компьютерных игр ===\n");

    char input[10];

    printf("1. вы любите экшн? (y/n): ");
    scanf("%s", input);
    clear_input();
    answers[0] = (strcmp(input, "y") == 0);

    printf("2. вам интересен глубокий сюжет? (y/n): ");
    scanf("%s", input);
    clear_input();
    answers[1] = (strcmp(input, "y") == 0);

    printf("3. вы предпочитаете одиночную игру? (y/n): ");
    scanf("%s", input);
    clear_input();
    answers[2] = (strcmp(input, "y") == 0);

    printf("4. вы любите головоломки? (y/n): ");
    scanf("%s", input);
    clear_input();
    answers[3] = (strcmp(input, "y") == 0);

    printf("5. вам нравятся хорроры? (y/n): ");
    scanf("%s", input);
    clear_input();
    answers[4] = (strcmp(input, "y") == 0);

    printf("6. вам нравятся гонки или аркады? (y/n): ");
    scanf("%s", input);
    clear_input();
    answers[5] = (strcmp(input, "y") == 0);

    printf("7. вас интересуют стратегии и тактики? (y/n): ");
    scanf("%s", input);
    clear_input();
    answers[6] = (strcmp(input, "y") == 0);

    printf("8. вы любите фэнтези-вселенные? (y/n): ");
    scanf("%s", input);
    clear_input();
    answers[7] = (strcmp(input, "y") == 0);

    printf("9. ввы предпочитаете мобильный гейминг? (y/n): ");
    scanf("%s", input);
    clear_input();
    answers[8] = (strcmp(input, "y") == 0);

    printf("10. есть ограничения по бюджету? (y/n): ");
    scanf("%s", input);
    clear_input();
    answers[9] = (strcmp(input, "y") == 0);

    // Поиск подходящего правила
    char final_recommendation[MAX_LINE] = "Minecraft (универсальная рекомендация)";
    for (int i = 0; i < rule_count; i++) {
        if (match_condition(rules[i].condition, answers)) {
            strcpy(final_recommendation, rules[i].recommendation);
            break;
        }
    }

    printf("\nРекомендуем: %s\n", final_recommendation);
    return 0;
}
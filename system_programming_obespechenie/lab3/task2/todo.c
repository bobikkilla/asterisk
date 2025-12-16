#define WIN32_LEAN_AND_MEAN
#define _WIN32_IE 0x0501
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define ID_LIST     1001
#define ID_INPUT    1002
#define ID_ADD      1003
#define ID_DELETE   1004
#define ID_TOGGLE   1005
#define ID_TIME     1006

#define ID_FILE_OPEN    2001
#define ID_FILE_SAVE    2002
#define ID_FILE_EXIT    2003
#define ID_HELP_ABOUT   3001

#define MAX_TASKS 1000

HWND hList, hInput, hTimeInput;
HINSTANCE hInst;
char currentFile[MAX_PATH] = {0};

typedef struct {
    char text[256];      // текст задачи
    char timeStr[9];     // "HH:MM" или пусто
    BOOL isDone;
} Task;

Task tasks[MAX_TASKS];
int taskCount = 0;
UINT_PTR timerId = 0;
HWND hWndMain = NULL;  // Главное окно


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void CreateControls(HWND hwnd);
void CreateMainMenu(HWND hwnd);
BOOL SaveTasksToFile(const char* filename);
BOOL LoadTasksFromFile(const char* filename);
void ShowAbout(HWND hwnd);
void AddTask();
void DeleteSelectedTask();
void ToggleTask();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    hInst = hInstance;
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "TodoApp";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, "Cannot register window class", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    HWND hwnd = CreateWindowEx(
        0, "TodoApp", "TODO List Manager",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 400,
        NULL, NULL, hInstance, NULL
    );

    hWndMain = hwnd;  // ← сохраняем
    
    if (!hwnd) {
        MessageBox(NULL, "Cannot create window", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_TIMER:
            if (wParam == 1 && hWndMain) {
                time_t now = time(NULL);
                struct tm lt = *localtime(&now); 
                char nowStr[9];
                strftime(nowStr, sizeof(nowStr), "%H:%M", &lt);

                // Отладка: можно временно раскомментировать
                // char dbg[64];
                // snprintf(dbg, sizeof(dbg), "Now: %s, tasks: %d", nowStr, taskCount);
                // OutputDebugStringA(dbg);

                for (int i = 0; i < taskCount; i++) {
                    if (tasks[i].isDone) continue;

                    char *t = tasks[i].timeStr;
                    // Очистим от пробелов (защита от " 12:00 ")
                    while (*t == ' ') t++;
                    if (*t == '\0') continue;

                    // Проверим формат: HH:MM
                    if (strlen(t) >= 5 && t[2] == ':') {
                        char hh[3] = {t[0], t[1], '\0'};
                        char mm[3] = {t[3], t[4], '\0'};
                        int h = atoi(hh), m = atoi(mm);
                        if (h == lt.tm_hour && m == lt.tm_min) {
                            // Точное совпадение по числам
                            char msg[1024];
                            snprintf(msg, sizeof(msg),
                                "Time to do:\n\n%.255s\n\nTime: %02d:%02d",
                                tasks[i].text, h, m);
                            MessageBox(hWndMain, msg, "TODO Reminder",
                                MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND | MB_TOPMOST);
                            MessageBeep(MB_ICONEXCLAMATION);
                            // PlaySound(TEXT("SystemExclamation"), NULL, ILD_ASYNC); // или SND_ASYNC
                        }
                    }
                }
            }
            break;
        case WM_CREATE:
            CreateControls(hwnd);
            CreateMainMenu(hwnd);
            break;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_ADD:
                    AddTask();
                    break;
                case ID_DELETE:
                    DeleteSelectedTask();
                    break;
                case ID_TOGGLE:
                    ToggleTask();
                    break;
                case ID_FILE_OPEN:
                    {
                        OPENFILENAME ofn = {0};
                        char szFile[MAX_PATH] = {0};
                        ofn.lStructSize = sizeof(ofn);
                        ofn.hwndOwner = hwnd;
                        ofn.lpstrFile = szFile;
                        ofn.nMaxFile = sizeof(szFile);
                        ofn.lpstrFilter = "TODO Files (*.todo)\0*.todo\0Text Files (*.txt)\0*.txt\0All Files\0*.*\0";
                        ofn.nFilterIndex = 1;
                        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                        if (GetOpenFileName(&ofn)) {
                            if (LoadTasksFromFile(ofn.lpstrFile)) {
                                strncpy(currentFile, ofn.lpstrFile, MAX_PATH - 1);
                                currentFile[MAX_PATH - 1] = '\0';
                            }
                        }
                    }
                    break;
                case ID_FILE_SAVE:
                    {
                        OPENFILENAME ofn = {0};
                        char szFile[MAX_PATH] = {0};
                        if (currentFile[0]) {
                            strcpy(szFile, currentFile);
                        }
                        ofn.lStructSize = sizeof(ofn);
                        ofn.hwndOwner = hwnd;
                        ofn.lpstrFile = szFile;
                        ofn.nMaxFile = sizeof(szFile);
                        ofn.lpstrFilter = "TODO Files (*.todo)\0*.todo\0Text Files (*.txt)\0*.txt\0All Files\0*.*\0";
                        ofn.nFilterIndex = 1;
                        ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

                        if (GetSaveFileName(&ofn)) {
                            if (SaveTasksToFile(ofn.lpstrFile)) {
                                strncpy(currentFile, ofn.lpstrFile, MAX_PATH - 1);
                                currentFile[MAX_PATH - 1] = '\0';
                            }
                        }
                    }
                    break;
                case ID_FILE_EXIT:
                    PostMessage(hwnd, WM_CLOSE, 0, 0);
                    break;
                case ID_HELP_ABOUT:
                    ShowAbout(hwnd);
                    break;
                default:
                    return DefWindowProc(hwnd, msg, wParam, lParam);
            }
            break;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;

        case WM_DESTROY:
            if (timerId != 0) {
                KillTimer(hwnd, timerId);
                timerId = 0;
            }
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void CreateControls(HWND hwnd) {
    // Поле ввода текста
    hInput = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        10, 10, 200, 25,
        hwnd, (HMENU)ID_INPUT, hInst, NULL);
    SendMessage(hInput, WM_SETTEXT, 0, (LPARAM)"Enter task...");

    // Поле ввода времени (HH:MM) — без "at" рядом
    hTimeInput = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER,
        220, 10, 60, 25,
        hwnd, (HMENU)ID_TIME, hInst, NULL);
    SendMessage(hTimeInput, WM_SETTEXT, 0, (LPARAM)"12:00");

    // Кнопки
    CreateWindow("BUTTON", "Add", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        290, 10, 70, 25, hwnd, (HMENU)ID_ADD, hInst, NULL);
    CreateWindow("BUTTON", "Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        10, 40, 80, 25, hwnd, (HMENU)ID_DELETE, hInst, NULL);
    CreateWindow("BUTTON", "Status", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        100, 40, 80, 25, hwnd, (HMENU)ID_TOGGLE, hInst, NULL);

    // Список задач
    hList = CreateWindow("LISTBOX", "",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY | LBS_STANDARD,
        10, 75, 470, 300,
        hwnd, (HMENU)ID_LIST, hInst, NULL);
}

void CreateMainMenu(HWND hwnd) {
    HMENU hMenu = CreateMenu();
    HMENU hFileMenu = CreatePopupMenu();
    HMENU hHelpMenu = CreatePopupMenu();

    AppendMenu(hFileMenu, MF_STRING, ID_FILE_OPEN, "Open...\tCtrl+O");
    AppendMenu(hFileMenu, MF_STRING, ID_FILE_SAVE, "Save...\tCtrl+S");
    AppendMenu(hFileMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hFileMenu, MF_STRING, ID_FILE_EXIT, "Exit");

    AppendMenu(hHelpMenu, MF_STRING, ID_HELP_ABOUT, "About");

    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, "File");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hHelpMenu, "Help");

    SetMenu(hwnd, hMenu);
}

void AddTask() {
    char textBuf[256] = {0};
    char timeBuf[10] = {0};

    GetWindowText(hInput, textBuf, sizeof(textBuf));
    if (strlen(textBuf) == 0 || strcmp(textBuf, "Enter task...") == 0) {
        MessageBox(NULL, "Task description cannot be empty!", "Warning", MB_OK | MB_ICONWARNING);
        return;
    }

    if (!hTimeInput) {
        MessageBox(NULL, "Time input not initialized!", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Получаем время
    GetWindowText(hTimeInput, timeBuf, sizeof(timeBuf));

    // Валидация времени (простая)
    if (strlen(timeBuf) > 0) {
        int h, m;
        if (sscanf(timeBuf, "%d:%d", &h, &m) != 2 || h < 0 || h > 23 || m < 0 || m > 59) {
            MessageBox(NULL, "Invalid time format! Use HH:MM (24h).", "Warning", MB_OK | MB_ICONWARNING);
            return;
        }
    }

    if (taskCount >= MAX_TASKS) {
        MessageBox(NULL, "Too many tasks!", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Сохраняем в массив
    strncpy(tasks[taskCount].text, textBuf, sizeof(tasks[taskCount].text) - 1);
    tasks[taskCount].text[sizeof(tasks[taskCount].text) - 1] = '\0';
    strncpy(tasks[taskCount].timeStr, timeBuf, sizeof(tasks[taskCount].timeStr) - 1);
    tasks[taskCount].timeStr[sizeof(tasks[taskCount].timeStr) - 1] = '\0';
    tasks[taskCount].isDone = FALSE;

    // Добавляем в список: "[HH:MM] - Task" или " - Task"
    char item[300];
    if (strlen(timeBuf) > 0) {
        snprintf(item, sizeof(item), "[%s] - %s", timeBuf, textBuf);
    } else {
        snprintf(item, sizeof(item), " - %s", textBuf);
    }
    SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)item);

    taskCount++;

    // Очищаем поля
    SetWindowText(hInput, "");
    SetWindowText(hTimeInput, "12:00");

    // Устанавливаем таймер, если ещё не установлен
    if (timerId == 0 && hWndMain) {
        timerId = SetTimer(hWndMain, 1, 5000, NULL); // каждые 5 сек
    }
}

void DeleteSelectedTask() {
    int index = SendMessage(hList, LB_GETCURSEL, 0, 0);
    if (index == LB_ERR || index >= taskCount) {
        MessageBox(NULL, "No task selected!", "Warning", MB_OK | MB_ICONWARNING);
        return;
    }

    // Сдвигаем задачи в массиве
    for (int i = index; i < taskCount - 1; i++) {
        tasks[i] = tasks[i + 1];
    }
    taskCount--;

    SendMessage(hList, LB_DELETESTRING, index, 0);

    // Если задач не осталось — убираем таймер
    if (taskCount == 0 && timerId != 0) {
        KillTimer(GetParent(hList), timerId);
        timerId = 0;
    }
}

void ToggleTask() {
    int index = SendMessage(hList, LB_GETCURSEL, 0, 0);
    if (index == LB_ERR || index >= taskCount) {
        MessageBox(NULL, "No task selected!", "Warning", MB_OK | MB_ICONWARNING);
        return;
    }

    tasks[index].isDone = !tasks[index].isDone;

    char item[300];
    SendMessage(hList, LB_GETTEXT, index, (LPARAM)item);

    // Пересобираем строку с учётом времени
    char timePart[20] = "";
    char* p = strchr(item, ']');
    if (p && item[0] == '[') {
        int len = p - item + 2; // "[HH:MM] "
        if (len < sizeof(timePart)) {
            strncpy(timePart, item, len);
            timePart[len] = '\0';
        }
    }

    char newText[300];
    if (tasks[index].isDone) {
        snprintf(newText, sizeof(newText), "%s+ %s", timePart, tasks[index].text);
    } else {
        snprintf(newText, sizeof(newText), "%s- %s", timePart, tasks[index].text);
    }

    SendMessage(hList, LB_DELETESTRING, index, 0);
    SendMessage(hList, LB_INSERTSTRING, index, (LPARAM)newText);
    SendMessage(hList, LB_SETCURSEL, index, 0);
}

BOOL SaveTasksToFile(const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        MessageBox(NULL, "Cannot create file!", "Error", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    fprintf(fp, "# TODO file (format: time|done|text)\n");
    for (int i = 0; i < taskCount; i++) {
        fprintf(fp, "%s|%d|%s\n",
            tasks[i].timeStr,
            tasks[i].isDone ? 1 : 0,
            tasks[i].text);
    }
    fclose(fp);
    MessageBox(NULL, "Tasks saved successfully!", "Info", MB_OK | MB_ICONINFORMATION);
    return TRUE;
}

BOOL LoadTasksFromFile(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        MessageBox(NULL, "Cannot open file!", "Error", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    SendMessage(hList, LB_RESETCONTENT, 0, 0);
    taskCount = 0;

    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#' || strlen(line) < 3) continue;
        line[strcspn(line, "\r\n")] = 0;

        char timeBuf[20], textBuf[256];
        int done = 0;
        if (sscanf(line, "%19[^|]|%d|%255[^\n]", timeBuf, &done, textBuf) == 3) {
            if (taskCount < MAX_TASKS) {
                strncpy(tasks[taskCount].timeStr, timeBuf, sizeof(tasks[taskCount].timeStr) - 1);
                tasks[taskCount].timeStr[sizeof(tasks[taskCount].timeStr) - 1] = '\0';
                tasks[taskCount].isDone = (done != 0);
                strncpy(tasks[taskCount].text, textBuf, sizeof(tasks[taskCount].text) - 1);
                tasks[taskCount].text[sizeof(tasks[taskCount].text) - 1] = '\0';

                char item[300];
                if (strlen(timeBuf) > 0) {
                    snprintf(item, sizeof(item), "[%s] %c %s",
                        timeBuf, done ? '+' : '-', textBuf);
                } else {
                    snprintf(item, sizeof(item), " %c %s",
                        done ? '+' : '-', textBuf);
                }
                SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)item);
                taskCount++;
            }
        }
    }
    fclose(fp);

    if (taskCount > 0 && timerId == 0 && hWndMain) {
        timerId = SetTimer(hWndMain, 1, 5000, NULL);
    }

    MessageBox(NULL, "Tasks loaded successfully!", "Info", MB_OK | MB_ICONINFORMATION);
    return TRUE;
}

void ShowAbout(HWND hwnd) {
    MessageBox(hwnd,
        "TODO List Manager\n"
        "Simple task tracker with file support\n"
        "Built with WinAPI and MinGW",
        "About TODO App", MB_OK | MB_ICONINFORMATION);
}
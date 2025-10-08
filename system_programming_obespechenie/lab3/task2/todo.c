#define WIN32_LEAN_AND_MEAN
#define _WIN32_IE 0x0501
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ID_LIST     1001
#define ID_INPUT    1002
#define ID_ADD      1003
#define ID_DELETE   1004
#define ID_TOGGLE   1005

#define ID_FILE_OPEN    2001
#define ID_FILE_SAVE    2002
#define ID_FILE_EXIT    2003
#define ID_HELP_ABOUT   3001

HWND hList, hInput;
HINSTANCE hInst;
char currentFile[MAX_PATH] = {0};

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
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void CreateControls(HWND hwnd) {
    // Поле ввода
    hInput = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        10, 10, 300, 25,
        hwnd, (HMENU)ID_INPUT, hInst, NULL);

    // Кнопки
    CreateWindow("BUTTON", "Add", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        320, 10, 70, 25, hwnd, (HMENU)ID_ADD, hInst, NULL);
    CreateWindow("BUTTON", "Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        10, 40, 80, 25, hwnd, (HMENU)ID_DELETE, hInst, NULL);
    CreateWindow("BUTTON", "Toggle", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
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
    char buffer[256];
    if (GetWindowText(hInput, buffer, sizeof(buffer)) == 0) {
        MessageBox(NULL, "Task description cannot be empty!", "Warning", MB_OK | MB_ICONWARNING);
        return;
    }
    // Добавляем как " - [текст]"
    char item[300];
    snprintf(item, sizeof(item), " - %s", buffer);
    SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)item);
    SetWindowText(hInput, ""); // очистить поле
}

void DeleteSelectedTask() {
    int index = SendMessage(hList, LB_GETCURSEL, 0, 0);
    if (index == LB_ERR) {
        MessageBox(NULL, "No task selected!", "Warning", MB_OK | MB_ICONWARNING);
        return;
    }
    SendMessage(hList, LB_DELETESTRING, index, 0);
}

void ToggleTask() {
    int index = SendMessage(hList, LB_GETCURSEL, 0, 0);
    if (index == LB_ERR) {
        MessageBox(NULL, "No task selected!", "Warning", MB_OK | MB_ICONWARNING);
        return;
    }

    char item[300];
    SendMessage(hList, LB_GETTEXT, index, (LPARAM)item);
    if (strncmp(item, " - ", 3) == 0) {
        // Не выполнено → выполнено
        memmove(item + 1, item + 3, strlen(item + 3) + 1);
        item[0] = '+';
        item[1] = ' ';
        SendMessage(hList, LB_DELETESTRING, index, 0);
        SendMessage(hList, LB_INSERTSTRING, index, (LPARAM)item);
        SendMessage(hList, LB_SETCURSEL, index, 0);
    } else if (strncmp(item, " + ", 3) == 0) {
        // Выполнено → не выполнено
        memmove(item + 1, item + 3, strlen(item + 3) + 1);
        item[0] = '-';
        item[1] = ' ';
        SendMessage(hList, LB_DELETESTRING, index, 0);
        SendMessage(hList, LB_INSERTSTRING, index, (LPARAM)item);
        SendMessage(hList, LB_SETCURSEL, index, 0);
    }
}

BOOL SaveTasksToFile(const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        MessageBox(NULL, "Cannot create file!", "Error", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    int count = SendMessage(hList, LB_GETCOUNT, 0, 0);
    for (int i = 0; i < count; i++) {
        char item[300];
        SendMessage(hList, LB_GETTEXT, i, (LPARAM)item);
        // Формат: "- Task" или "+ Task"
        char prefix = item[1]; // ' ' после знака
        char sign = item[0];
        if (sign == '-' || sign == '+') {
            fprintf(fp, "%c %s\n", sign, item + 3);
        } else {
            fprintf(fp, "- %s\n", item);
        }
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
    char line[300];
    while (fgets(line, sizeof(line), fp)) {
        // Убираем \n
        line[strcspn(line, "\r\n")] = 0;
        if (strlen(line) == 0) continue;

        char item[300];
        if (line[0] == '+' || line[0] == '-') {
            snprintf(item, sizeof(item), " %c %s", line[0], line + 2);
            item[0] = line[0]; // исправляем первый символ
            SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)item);
        } else {
            snprintf(item, sizeof(item), " - %s", line);
            SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)item);
        }
    }
    fclose(fp);
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
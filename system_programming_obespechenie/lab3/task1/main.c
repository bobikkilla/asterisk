#define WIN32_LEAN_AND_MEAN
#define _WIN32_IE 0x0501
#include <windows.h>
#include <commctrl.h>   // Обязательно после windows.h
#include <commdlg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ID_EDIT 1001
#define ID_FILE_OPEN 101
#define ID_FILE_SAVE 102
#define ID_FILE_EXIT 103
#define ID_EDIT_CUT 201
#define ID_EDIT_COPY 202
#define ID_EDIT_PASTE 203
#define ID_HELP_ABOUT 301

HWND hEdit;
HINSTANCE hInst;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void CreateMainMenu(HWND hwnd);
void CreateContextMenu(HWND hwnd, int x, int y);
BOOL OpenTextFile(HWND hEdit);
BOOL SaveTextFile(HWND hEdit);
void ShowAbout(HWND hwnd);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    hInst = hInstance;
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "SimpleTextEditor";
    wc.hCursor = LoadCursor(NULL, IDC_IBEAM);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, "Cannot register class", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    HWND hwnd = CreateWindowEx(
        0, "SimpleTextEditor", "Simple Text Editor",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
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
            hEdit = CreateWindowEx(
                WS_EX_CLIENTEDGE,
                "EDIT", "",
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL |
                ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
                0, 0, 0, 0,
                hwnd, (HMENU)ID_EDIT, hInst, NULL
            );
            CreateMainMenu(hwnd);
            break;

        case WM_SIZE:
            if (hEdit) {
                MoveWindow(hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
            }
            break;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_FILE_OPEN:
                    OpenTextFile(hEdit);  // ← ИЗМЕНЕНО
                    break;
                case ID_FILE_SAVE:
                    SaveTextFile(hEdit);  // ← ИЗМЕНЕНО
                    break;
                case ID_FILE_EXIT:
                    PostMessage(hwnd, WM_CLOSE, 0, 0);
                    break;
                case ID_EDIT_CUT:
                    SendMessage(hEdit, WM_CUT, 0, 0);
                    break;
                case ID_EDIT_COPY:
                    SendMessage(hEdit, DM_COPY, 0, 0);
                    break;
                case ID_EDIT_PASTE:
                    SendMessage(hEdit, WM_PASTE, 0, 0);
                    break;
                case ID_HELP_ABOUT:
                    ShowAbout(hwnd);
                    break;
            }
            break;

        case WM_CONTEXTMENU:
            if ((HWND)wParam == hEdit) {
                int x = (short)LOWORD(lParam);
                int y = (short)HIWORD(lParam);
                if (x == -1 && y == -1) {
                    RECT rc;
                    GetClientRect(hEdit, &rc);
                    POINT pt = {rc.left, rc.bottom};
                    ClientToScreen(hEdit, &pt);
                    x = pt.x;
                    y = pt.y;
                }
                CreateContextMenu(hwnd, x, y);
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

void CreateMainMenu(HWND hwnd) {
    HMENU hMenu = CreateMenu();
    HMENU hFileMenu = CreatePopupMenu();
    HMENU hEditMenu = CreatePopupMenu();
    HMENU hHelpMenu = CreatePopupMenu();

    AppendMenu(hFileMenu, MF_STRING, ID_FILE_OPEN, "Open\tCtrl+O");
    AppendMenu(hFileMenu, MF_STRING, ID_FILE_SAVE, "Save\tCtrl+S");
    AppendMenu(hFileMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hFileMenu, MF_STRING, ID_FILE_EXIT, "Exit");

    AppendMenu(hEditMenu, MF_STRING, ID_EDIT_CUT, "Cut\tCtrl+X");
    AppendMenu(hEditMenu, MF_STRING, ID_EDIT_COPY, "Copy\tCtrl+C");
    AppendMenu(hEditMenu, MF_STRING, ID_EDIT_PASTE, "Paste\tCtrl+V");

    AppendMenu(hHelpMenu, MF_STRING, ID_HELP_ABOUT, "About text_editor");

    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, "File");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hEditMenu, "Edit");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hHelpMenu, "Help");

    SetMenu(hwnd, hMenu);
}

void CreateContextMenu(HWND hwnd, int x, int y) {
    HMENU hMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_STRING, ID_EDIT_CUT, "Cut");
    AppendMenu(hMenu, MF_STRING, ID_EDIT_COPY, "Copy");
    AppendMenu(hMenu, MF_STRING, ID_EDIT_PASTE, "Paste");
    TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, x, y, 0, hwnd, NULL);
    DestroyMenu(hMenu);
}

BOOL OpenTextFile(HWND hEdit) {
    OPENFILENAME ofn = {0};
    char szFile[260] = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetParent(hEdit);
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

    if (GetOpenFileName(&ofn)) {
        HANDLE hFile = CreateFileA(ofn.lpstrFile, GENERIC_READ, FILE_SHARE_READ,
                                   NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            DWORD dwFileSize = GetFileSize(hFile, NULL);
            if (dwFileSize != INVALID_FILE_SIZE) {
                char* buffer = (char*)malloc(dwFileSize + 1);
                if (buffer) {
                    DWORD dwRead;
                    ReadFile(hFile, buffer, dwFileSize, &dwRead, NULL);
                    buffer[dwRead] = '\0';
                    SetWindowTextA(hEdit, buffer);
                    free(buffer);
                }
            } else {
                SetWindowTextA(hEdit, "");
            }
            CloseHandle(hFile);
            return TRUE;
        }
    }
    return FALSE;
}

BOOL SaveTextFile(HWND hEdit) {
    OPENFILENAME ofn = {0};
    char szFile[260] = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetParent(hEdit);
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&ofn)) {
        HANDLE hFile = CreateFileA(ofn.lpstrFile, GENERIC_WRITE, 0, NULL,
                                   CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            int len = GetWindowTextLengthA(hEdit);
            if (len > 0) {
                char* buffer = (char*)malloc(len + 1);
                if (buffer) {
                    GetWindowTextA(hEdit, buffer, len + 1);
                    DWORD dwWritten;
                    WriteFile(hFile, buffer, len, &dwWritten, NULL);
                    free(buffer);
                }
            }
            CloseHandle(hFile);
            return TRUE;
        }
    }
    return FALSE;
}

void ShowAbout(HWND hwnd) {
    MessageBox(hwnd,
        "Simple Text Editor\n"
        "Built with MinGW on Linux\n"
        "Runs on Windows and Wine",
        "About", MB_OK | MB_ICONINFORMATION);
}
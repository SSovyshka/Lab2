#include <windows.h>
#include <vector>
#include <map>
#include <ctime>
#include <iostream>
#include <string>
#include <cmath>

const int CELL_SIZE = 50;
const int GRID_SIZE = 10;

int grid[GRID_SIZE][GRID_SIZE];
std::map<int, int> frequencies;
POINT lastMousePos = { 0, 0 };

std::vector<std::pair<wchar_t, POINT>> typedChars;

using namespace std;

void GenerateGrid() {
    srand(time(nullptr));
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            int value = rand() % 10;
            grid[i][j] = value;
            frequencies[value]++;
        }
    }
}

void DrawGrid(HDC hdc) {
    for (int y = 0; y < GRID_SIZE; ++y) {
        for (int x = 0; x < GRID_SIZE; ++x) {
            int value = grid[y][x];
            int r = (255 * x) / (GRID_SIZE - 1);
            int g = 0;
            int b = (255 * (GRID_SIZE - 1 - y)) / (GRID_SIZE - 1);
            HBRUSH hBrush = CreateSolidBrush(RGB(r, g, b));
            RECT rect = { x * CELL_SIZE, y * CELL_SIZE, (x + 1) * CELL_SIZE, (y + 1) * CELL_SIZE };
            FillRect(hdc, &rect, hBrush);
            DeleteObject(hBrush);

            wstring text = to_wstring(value);
            SetTextColor(hdc, RGB(255, 255, 255));
            SetBkMode(hdc, TRANSPARENT);

            RECT textRect = rect;
            DrawText(hdc, text.c_str(), -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
    }
}

void DrawCharts(HDC hdc) {
    int total = 100;
    int startAngle = 0;
    int centerX = GRID_SIZE * CELL_SIZE + 150;
    int centerY = 200;
    int radius = 100;
    int innerRadius = 70;

    for (auto& pair : frequencies) {
        int percent = (pair.second * 360) / total;
        int endAngle = startAngle + percent;
        HBRUSH hBrush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
        SelectObject(hdc, hBrush);
        Pie(hdc, centerX - radius, centerY - radius, centerX + radius, centerY + radius,
            centerX + radius * cos(startAngle * 3.14 / 180),
            centerY - radius * sin(startAngle * 3.14 / 180),
            centerX + radius * cos(endAngle * 3.14 / 180),
            centerY - radius * sin(endAngle * 3.14 / 180));
        DeleteObject(hBrush);
        startAngle = endAngle;
    }

    int ringChartCenterY = centerY + 250;
    startAngle = 0;
    for (auto& pair : frequencies) {
        int percent = (pair.second * 360) / total;
        int endAngle = startAngle + percent;

        HBRUSH hBrush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
        SelectObject(hdc, hBrush);
        Pie(hdc, centerX - radius, ringChartCenterY - radius, centerX + radius, ringChartCenterY + radius,
            centerX + radius * cos(startAngle * 3.14 / 180),
            ringChartCenterY - radius * sin(startAngle * 3.14 / 180),
            centerX + radius * cos(endAngle * 3.14 / 180),
            ringChartCenterY - radius * sin(endAngle * 3.14 / 180));

        DeleteObject(hBrush);
        startAngle = endAngle;
    }

    HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
    SelectObject(hdc, whiteBrush);
    Ellipse(hdc, centerX - innerRadius, ringChartCenterY - innerRadius, centerX + innerRadius, ringChartCenterY + innerRadius);
    DeleteObject(whiteBrush);
}

void ShowDeviceInfo(HDC hdc) {
    int widthMM = GetDeviceCaps(hdc, HORZSIZE);
    int heightMM = GetDeviceCaps(hdc, VERTSIZE);

    int fontHeight = GetDeviceCaps(hdc, LOGPIXELSY);
    TEXTMETRIC tm;
    GetTextMetrics(hdc, &tm);

    bool hasMouse = GetSystemMetrics(SM_MOUSEPRESENT);
    int buttons = GetSystemMetrics(SM_CMOUSEBUTTONS);

    RECT infoRect = { 550, 10, 780, 300 };
    SetTextColor(hdc, RGB(0, 0, 0));
    SetBkMode(hdc, TRANSPARENT);

    wchar_t buffer[512];
    swprintf(buffer, 512, L"Монітор: %d x %d мм\nШрифт: висота %d, міжстрок %d\nМиша: є=%d, кнопок=%d",
        widthMM, heightMM, tm.tmHeight, tm.tmExternalLeading, hasMouse, buttons);
    DrawText(hdc, buffer, -1, &infoRect, DT_LEFT);
}

void DrawTypedChars(HDC hdc) {
    for (const auto& pair : typedChars) {
        wchar_t ch[2] = { pair.first, 0 };
        TextOut(hdc, pair.second.x, pair.second.y, ch, 1);
    }
}

void RandomCharAtMouse(HWND hwnd) {
    wchar_t ch = L'!' + rand() % 94;
    HDC hdc = GetDC(hwnd);
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hwnd, &pt);
    wchar_t s[2] = { ch, 0 };
    TextOut(hdc, pt.x, pt.y, s, 1);
    ReleaseDC(hwnd, hdc);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CHAR: {
        POINT pt = { rand() % 800, rand() % 600 };
        typedChars.push_back({ (wchar_t)wParam, pt });
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    }

    case WM_MOUSEMOVE: {
        POINT currentPos = { LOWORD(lParam), HIWORD(lParam) };
        int dx = currentPos.x - lastMousePos.x;
        int dy = currentPos.y - lastMousePos.y;
        if (sqrt(dx * dx + dy * dy) > 100) {
            RandomCharAtMouse(hwnd);
            lastMousePos = currentPos;
        }
    } break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        DrawGrid(hdc);
        DrawCharts(hdc);
        ShowDeviceInfo(hdc);
        DrawTypedChars(hdc);
        EndPaint(hwnd, &ps);
    } break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    GenerateGrid();

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"Lab2Full";
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, L"Lab2Full", L"Lab2 - Повна версiя", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1000, 700, NULL, NULL, hInstance, NULL);
    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

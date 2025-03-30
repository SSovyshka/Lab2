#include <windows.h>
#include <vector>
#include <map>
#include <ctime>
#include <iostream>
#include <string>

const int CELL_SIZE = 50;
const int GRID_SIZE = 10;

int grid[GRID_SIZE][GRID_SIZE];
std::map<int, int> frequencies;

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

            RECT textRect = { x * CELL_SIZE, y * CELL_SIZE, (x + 1) * CELL_SIZE, (y + 1) * CELL_SIZE };
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


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        DrawGrid(hdc);
        DrawCharts(hdc);
        EndPaint(hwnd, &ps);
    } break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    GenerateGrid();

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            cout << grid[i][j] << " ";
        }
        cout << "\n";
    }

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"Lab2";
    RegisterClass(&wc);
    HWND hwnd = CreateWindowEx(0, L"Lab2", L"Lab2", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);
    ShowWindow(hwnd, nCmdShow);
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

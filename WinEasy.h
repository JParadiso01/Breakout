#include <Windows.h>
#include <time.h>
#include <string.h>

typedef struct {
    int r;
    int g;
    int b;
} Color;

enum COLOR_NAMES {
    RED = 0,
    BLUE,
    GREEN,
    YELLOW,
    PINK,
    WHITE,
    LIGHT_BLUE,
    GRAY,
    LIGHT_GRAY,
    DARK_GRAY,
    BLACK,
    COLOR_COUNT,
};

Color colors[COLOR_COUNT] = {
    (Color){0xFF,0x00,0x00},
    (Color){0x00,0x00,0xFF},
    (Color){0x00,0xFF,0x00},
    (Color){0xFF,0xFF,0x00},
    (Color){0xFF,0x00,0xFF},
    (Color){0xFF,0xFF,0xFF},
    (Color){0x00,0xFF,0xFF},
    (Color){0x80,0x80,0x80},
    (Color){0xDC,0xDC,0xDC},
    (Color){0x18,0x18,0x18},
    (Color){0x00,0x00,0x00},
};


void WinEasyCreateWindowClass(WNDCLASSEX *wc, HINSTANCE hInstance,LRESULT CALLBACK (* WndProc)(HWND, UINT, WPARAM, LPARAM), char *ClassName, HBRUSH hbrBackground);
void WinEasyCreateWindow(HWND *hwnd, char *windowTitle, char *className, HINSTANCE hInstance, int w, int h, int x, int y);

void WinEasyStartBackBuffer(PAINTSTRUCT *ps, HWND *hwnd, HDC *frontHDC, HDC *backHDC, HBITMAP *backBuffer, RECT *windowRect);
void WinEasyCopyBackBuffer(HDC frontHDC, HDC backHDC, RECT windowRect);
void WinEasyEndBackBuffer(PAINTSTRUCT ps, HWND hwnd, HDC backHDC, HBITMAP backBuffer);

void WinEasyDrawBackground(HDC screen, Color color, RECT rect);
void WinEasyDrawRect(HDC screen, Color color, RECT rect);
void WinEasyDrawCircle(HDC screen, Color color, int x, int y, int radius);
void WinEasyDrawEllipse(HDC screen, Color color, RECT ellipse_rect);

void WinEasyDrawText(HDC screen, Color txtColor, char * string, int x, int y);


COLORREF WinEasyColorToCOLORREF(Color color);
HBRUSH WinEasyColorToHBRUSH(Color color);

void WinEasySleepForFrames(float frames, clock_t begTime, clock_t endTime);


void WinEasyCreateWindowClass(WNDCLASSEX *wc, HINSTANCE hInstance,LRESULT CALLBACK (* WndProc)(HWND, UINT, WPARAM, LPARAM), char *ClassName, HBRUSH hbrBackground){
    wc->cbSize        = sizeof(WNDCLASSEX);
    wc->style         = 0;
    wc->lpfnWndProc   = WndProc;
    wc->cbClsExtra    = 0;
    wc->cbWndExtra    = 0;
    wc->hInstance     = hInstance;
    wc->hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc->hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc->hbrBackground = hbrBackground;
    wc->lpszMenuName  = NULL;
    wc->lpszClassName = ClassName;
    wc->hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(wc)){
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        exit(1);
    }
}

void WinEasyCreateWindow(HWND *hwnd, char *windowTitle, char *className, HINSTANCE hInstance, int w, int h, int x, int y){
    RECT rect = {0, 0, w, h};
    AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_CLIENTEDGE);
    *hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        className,
        windowTitle,
        WS_OVERLAPPEDWINDOW,
        x, y, rect.right - rect.left, rect.bottom - rect.top,
        NULL, NULL, hInstance, NULL);
    if(hwnd == NULL){
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        exit(1);
    }
}


void WinEasyStartBackBuffer(PAINTSTRUCT *ps, HWND *hwnd, HDC *frontHDC, HDC *backHDC, HBITMAP *backBuffer, RECT *windowRect){
    *frontHDC = BeginPaint(*hwnd, ps);
    *backHDC = CreateCompatibleDC(*frontHDC);
    GetClientRect(*hwnd, windowRect);
    *backBuffer = CreateCompatibleBitmap(*frontHDC, windowRect->right, windowRect->bottom);
    SelectObject(*backHDC, *backBuffer);
}
void WinEasyCopyBackBuffer(HDC frontHDC, HDC backHDC, RECT windowRect){
    BitBlt(frontHDC, 0,0, windowRect.right, windowRect.bottom, backHDC,0,0,SRCCOPY);
}

void WinEasyEndBackBuffer(PAINTSTRUCT ps, HWND hwnd, HDC backHDC, HBITMAP backBuffer){
        DeleteDC(backHDC);
        DeleteObject(backBuffer);
        EndPaint(hwnd, &ps);
}

void WinEasyDrawBackground(HDC screen, Color color, RECT rect){
    HBRUSH brush = WinEasyColorToHBRUSH(color);
    SelectObject(screen, brush);
    Rectangle(screen,rect.left,rect.top,rect.right,rect.bottom);
    DeleteObject(brush);
}
void WinEasyDrawRect(HDC screen, Color color, RECT rect){
    HBRUSH brush = WinEasyColorToHBRUSH(color);
    SelectObject(screen, brush);
    Rectangle(screen,rect.left,rect.top,rect.right,rect.bottom);
    DeleteObject(brush);
}

void WinEasyDrawCircle(HDC screen, Color color, int x, int y, int radius){
    HBRUSH brush = WinEasyColorToHBRUSH(color);
    SelectObject(screen, brush);
    Ellipse(screen, x-radius, y-radius, x+radius, y+radius);
    DeleteObject(brush);
}

void WinEasyDrawEllipse(HDC screen, Color color, RECT ellipseRect){
    HBRUSH brush = WinEasyColorToHBRUSH(color);
    SelectObject(screen, brush);
    Ellipse(screen,ellipseRect.left,ellipseRect.top,ellipseRect.right,ellipseRect.bottom);
    DeleteObject(brush);
}

void WinEasyDrawText(HDC screen, Color txtColor, char *string, int x, int y){
    SetTextColor(screen, WinEasyColorToCOLORREF(txtColor));
    SetBkColor(screen, GetBkColor(screen));
    TextOutA(screen, x, y, string, strlen(string));
}



COLORREF WinEasyColorToCOLORREF(Color color){
    return RGB(color.r, color.g, color.b);
}

HBRUSH WinEasyColorToHBRUSH(Color color){
    return CreateSolidBrush(RGB(color.r, color.g, color.b));
}

void WinEasySleepForFrames(float frames, clock_t begTime, clock_t endTime){
    float frameTime = (float)(endTime - begTime)/CLOCKS_PER_SEC;
    if (frameTime < (1/frames)){
        Sleep((1.0/frames) - frameTime);
    }
}

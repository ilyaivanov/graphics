#include <windows.h>

#include "utils/all.c"

i32 isRunning = 1;
MyBitmap bitmap;
BITMAPINFO bitmapInfo;

LRESULT OnEvent(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_DESTROY)
        isRunning = 0;

    if (message == WM_SIZE)
    {
        bitmap.width = LOWORD(lParam);
        bitmap.height = HIWORD(lParam);
        bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
        bitmapInfo.bmiHeader.biBitCount = 32;
        bitmapInfo.bmiHeader.biWidth = bitmap.width;
        bitmapInfo.bmiHeader.biHeight = bitmap.height;
        bitmapInfo.bmiHeader.biPlanes = 1;

        if (bitmap.pixels)
            VirtualFreeMemory(bitmap.pixels);

        bitmap.pixels = VirtualAllocateMemory(bitmap.width * bitmap.height * 4);
    }

    if (message == WM_PAINT)
    {
        PAINTSTRUCT paint = {0};
        BeginPaint(window, &paint);
        EndPaint(window, &paint);
    }
    return DefWindowProc(window, message, wParam, lParam);
}

inline V2f WorldToScreen(V3f p)
{
    // ugly hack
    if (p.z == 0)
        p.z = 0.0001f;

    return (V2f){((p.x / p.z) + 1) / 2 * bitmap.width, ((p.y / p.z) + 1) / 2 * bitmap.height};
}

void FillTriangle(V3f p1, V3f p2, V3f p3, u32 color)
{
    V2f screenP1 = WorldToScreen(p1);
    V2f screenP2 = WorldToScreen(p2);
    V2f screenP3 = WorldToScreen(p3);

    f32 minX = Max2Int(Min3Int(screenP1.x, screenP2.x, screenP3.x), 0);
    f32 maxX = Min2Int(Max3Int(screenP1.x, screenP2.x, screenP3.x), bitmap.width);

    f32 minY = Max2Int(Min3Int(screenP1.y, screenP2.y, screenP3.y), 0);
    f32 maxY = Min2Int(Max3Int(screenP1.y, screenP2.y, screenP3.y), bitmap.height);

    V2f first = screenP1;
    V2f second = screenP2;
    V2f third = screenP3;

    // f32 minX = 0;
    // f32 maxX = bitmap.width;

    // f32 minY = 0;
    // f32 maxY = bitmap.height;

    for (i32 y = minY; y < maxY; y++)
        for (i32 x = minX; x < maxX; x++)
        {
            if (V2fCross(V2fDiff(second, first), V2fDiff((V2f){x + 0.5f, y + 0.5f}, first)) <= 0 &&
                V2fCross(V2fDiff(third, second), V2fDiff((V2f){x + 0.5f, y + 0.5f}, second)) <= 0 &&
                V2fCross(V2fDiff(first, third), V2fDiff((V2f){x + 0.5f, y + 0.5f}, third)) <= 0)
                *(bitmap.pixels + y * bitmap.width + x) = color;
        }
}

void FillPolygon4(V3f p1, V3f p2, V3f p3, V3f p4, u32 color)
{
    V2f screenP1 = WorldToScreen(p1);
    V2f screenP2 = WorldToScreen(p2);
    V2f screenP3 = WorldToScreen(p3);
    V2f screenP4 = WorldToScreen(p4);

    f32 minX = Max2Int(Min4Int(screenP1.x, screenP2.x, screenP3.x, screenP4.x), 0);
    f32 maxX = Min2Int(Max4Int(screenP1.x, screenP2.x, screenP3.x, screenP4.x), bitmap.width);

    f32 minY = Max2Int(Min4Int(screenP1.y, screenP2.y, screenP3.y, screenP4.y), 0);
    f32 maxY = Min2Int(Max4Int(screenP1.y, screenP2.y, screenP3.y, screenP4.y), bitmap.height);

    V2f first = screenP1;
    V2f second = screenP2;
    V2f third = screenP3;
    V2f fourth = screenP4;

    // f32 minX = 0;
    // f32 maxX = bitmap.width;

    // f32 minY = 0;
    // f32 maxY = bitmap.height;

    for (i32 y = minY; y < maxY; y++)
        for (i32 x = minX; x < maxX; x++)
        {
            if (V2fCross(V2fDiff(second, first), V2fDiff((V2f){x + 0.5f, y + 0.5f}, first)) <= 0 &&
                V2fCross(V2fDiff(third, second), V2fDiff((V2f){x + 0.5f, y + 0.5f}, second)) <= 0 &&
                V2fCross(V2fDiff(fourth, third), V2fDiff((V2f){x + 0.5f, y + 0.5f}, third)) <= 0 &&
                V2fCross(V2fDiff(first, fourth), V2fDiff((V2f){x + 0.5f, y + 0.5f}, fourth)) <= 0)
                *(bitmap.pixels + y * bitmap.width + x) = color;
        }
}

// #define USE_POLYGON

void DrawLowerPlane(f32 leftX, f32 rightX, f32 nearZ, f32 farZ, u32 color)
{
    f32 y = -1;
    V3f nearLeft = {leftX, y, nearZ};
    V3f farRight = {rightX, y, farZ};
    V3f farLeft = {leftX, y, farZ};
    V3f nearRight = {rightX, y, nearZ};

#ifndef USE_POLYGON
    FillTriangle(nearLeft, farRight, nearRight, color);
    FillTriangle(nearLeft, farLeft, farRight, color);
#else
    FillPolygon4(nearLeft, farLeft, farRight, nearRight, color);
#endif
}

void DrawUpperPlane(f32 leftX, f32 rightX, f32 nearZ, f32 farZ, u32 color)
{
    f32 y = 1;
    V3f nearLeft = {leftX, y, nearZ};
    V3f farRight = {rightX, y, farZ};
    V3f farLeft = {leftX, y, farZ};
    V3f nearRight = {rightX, y, nearZ};
#ifndef USE_POLYGON
    FillTriangle(nearLeft, farRight, farLeft, color);
    FillTriangle(nearLeft, nearRight, farRight, color);
#else
    FillPolygon4(nearLeft, nearRight, farRight, farLeft, color);
#endif
}

void DrawLeftPlane(f32 upperY, f32 lowerY, f32 nearZ, f32 farZ, u32 color)
{
    f32 x = -1;
    V3f nearUp = {x, upperY, nearZ};
    V3f nearDown = {x, lowerY, nearZ};

    V3f farUp = {x, upperY, farZ};
    V3f farDown = {x, lowerY, farZ};

#ifndef USE_POLYGON
    FillTriangle(nearUp, farUp, nearDown, color);
    FillTriangle(nearDown, farUp, farDown, color);
#else
    FillPolygon4(nearUp, farUp, farDown, nearDown, color);
#endif
}

void DrawRightPlane(f32 upperY, f32 lowerY, f32 nearZ, f32 farZ, u32 color)
{
    f32 x = 1;
    V3f nearUp = {x, upperY, nearZ};
    V3f nearDown = {x, lowerY, nearZ};

    V3f farUp = {x, upperY, farZ};
    V3f farDown = {x, lowerY, farZ};

#ifndef USE_POLYGON
    FillTriangle(farDown, farUp, nearUp, color);
    FillTriangle(farDown, nearUp, nearDown, color);
#else
    FillPolygon4(farUp, nearUp, nearDown, farDown, color);
#endif
}

f32 appTimeSec;
RandomSeries series;
f32 speed = 3.0f;
#define PLANES_COUNT 400
f32 bordersZ[PLANES_COUNT];
u32 colors[4 * PLANES_COUNT];

u32 RandomColor()
{
    return RandomChoice(&series, 0xffffff);
}

void DrawBorder(f32 z, u32 upColor, u32 rightColor, u32 bottomColor, u32 leftColor)
{
    DrawUpperPlane(-1.0f, 1.0f, z, z + 1, upColor);

    DrawRightPlane(1.0f, -1.0f, z, z + 1, rightColor);

    DrawLowerPlane(-1.0f, 1.0f, z, z + 1, bottomColor);

    DrawLeftPlane(1.0f, -1.0f, z, z + 1, leftColor);
}

void WinMainCRTStartup()
{
    PreventWindowsDPIScaling();

    InitPerf();

    series = CreateSeries();

    for (int i = 0; i < ArrayLength(colors); i++)
        colors[i] = RandomColor();

    for (int i = 0; i < ArrayLength(bordersZ); i++)
        bordersZ[i] = i;

    HWND window = OpenAppWindowWithSize(GetModuleHandle(0), OnEvent, 1300, 1300);
    HDC dc = GetDC(window);
    MSG msg;
    f32 prevFrameTimeSec = 0;
    while (isRunning)
    {
        StartMetric(Overall);
        while (PeekMessageA(&msg, 0, 0, 0, 1))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        for (int i = 0; i < ArrayLength(bordersZ); i++)
        {
            bordersZ[i] -= prevFrameTimeSec * speed;
            if (bordersZ[i] < 0.01f)
            {
                bordersZ[i] = ArrayLength(bordersZ) - 1;
            }
        }

        memset(bitmap.pixels, 0x22, bitmap.height * bitmap.width * 4);

        for (int i = 0; i < ArrayLength(bordersZ); i++)
        {
            f32 z = bordersZ[i];
            DrawBorder(z, colors[i * 4], colors[i * 4 + 1], colors[i * 4 + 2], colors[i * 4 + 3]);
        }

        StretchDIBits(dc,
                      0, 0, bitmap.width, bitmap.height,
                      0, 0, bitmap.width, bitmap.height,
                      bitmap.pixels, &bitmapInfo, 0, SRCCOPY);

        StopMetric(Overall);
        u32 frameMicroseconds = GetMicrosecondsFor(Overall);
        prevFrameTimeSec = (f32)frameMicroseconds / 1000 / 1000;
        appTimeSec += prevFrameTimeSec;

        u8 ch[20];
        FormatNumber(frameMicroseconds, ch);
        OutputDebugStringA(ch);
        OutputDebugStringA("\n");
    }

    ExitProcess(0);
}
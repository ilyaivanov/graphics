#include <windows.h>

#include "utils/all.c"

#define F32_MAX 3.402823466e+38F

i32 isRunning = 1;
MyBitmap bitmap;
f32 *zBuffer;
u32 zBufferLength;

BITMAPINFO bitmapInfo;

V2f mouseScreenPos;

V3f RED = {1.0f, 0.0, 0.0};
V3f BLUE = {0.0f, 1.0, 0.0};
V3f GREEN = {0.0f, 0.0, 1.0};

V3f YELLOW = {1.0f, 1.0, 0.0};
V3f PURPLE = {1.0f, 0.0, 1.0};
V3f DARK_PURPLE = {0.5f, 0.0, 1.0};

V3f WHITE = {1.0f, 1.0, 1.0};

typedef struct Triangle
{
    V4f v1, v2, v3;
    V3f c1, c2, c3;
} Triangle;

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
        if (zBuffer)
            VirtualFreeMemory(zBuffer);

        bitmap.pixels = VirtualAllocateMemory(bitmap.width * bitmap.height * 4);

        zBufferLength = bitmap.width * bitmap.height * sizeof(f32);
        zBuffer = VirtualAllocateMemory(bitmap.width * bitmap.height * sizeof(f32));
    }

    if (message == WM_PAINT)
    {
        PAINTSTRUCT paint = {0};
        BeginPaint(window, &paint);
        EndPaint(window, &paint);
    }

    if (message == WM_MOUSEMOVE)
    {
        mouseScreenPos.x = (f32)LOWORD(lParam) / bitmap.width * 2 - 1;
        mouseScreenPos.y = (f32)(bitmap.height - HIWORD(lParam)) / bitmap.height * 2 - 1;
    }
    return DefWindowProc(window, message, wParam, lParam);
}

inline V2f WorldToScreen(V4f p)
{
    return (V2f){((p.x / p.z) + 1) / 2 * bitmap.width, ((p.y / p.z) + 1) / 2 * bitmap.height};
}

void FillTriangleInterpolatedColors(Triangle trig, Mat4 mat)
{
    V4f v1 = Mat4MultV4f(mat, trig.v1);
    V4f v2 = Mat4MultV4f(mat, trig.v2);
    V4f v3 = Mat4MultV4f(mat, trig.v3);
    V2f screenP1 = WorldToScreen(v1);
    V2f screenP2 = WorldToScreen(v2);
    V2f screenP3 = WorldToScreen(v3);

    f32 minX = Max2Int(Min3Int(screenP1.x, screenP2.x, screenP3.x), 0);
    f32 maxX = Min2Int(Max3Int(screenP1.x, screenP2.x, screenP3.x) + 1, bitmap.width);

    f32 minY = Max2Int(Min3Int(screenP1.y, screenP2.y, screenP3.y), 0);
    f32 maxY = Min2Int(Max3Int(screenP1.y, screenP2.y, screenP3.y) + 1, bitmap.height);

    V2f A = screenP1;
    V2f B = screenP2;
    V2f C = screenP3;

    // f32 minX = 0;
    // f32 maxX = bitmap.width;

    // f32 minY = 0;
    // f32 maxY = bitmap.height;

    f32 volume = -V2fCross(V2fDiff(A, B), V2fDiff(A, C));

    for (i32 y = minY; y < maxY; y++)
        for (i32 x = minX; x < maxX; x++)
        {
            V2f point = (V2f){x + 0.5f, y + 0.5f};
            if (V2fCross(V2fDiff(B, A), V2fDiff(point, A)) <= 0 &&
                V2fCross(V2fDiff(C, B), V2fDiff(point, B)) <= 0 &&
                V2fCross(V2fDiff(A, C), V2fDiff(point, C)) <= 0)
            {
                f32 t1 = V2fCross(V2fDiff(B, point), V2fDiff(B, C)) / volume;
                f32 t2 = V2fCross(V2fDiff(C, point), V2fDiff(C, A)) / volume;
                f32 t3 = 1.0f - t1 - t2;

                V3f res = LerpV3f(trig.c1, trig.c2, trig.c3, t1, t2, t3);

                f32 zInterpolated = Lerp3f(1 / v1.z, 1 / v2.z, 1 / v3.z, t1, t2, t3);
                zInterpolated = 1 / zInterpolated;

                if (zBuffer[y * bitmap.width + x] > zInterpolated)
                {
                    zBuffer[y * bitmap.width + x] = zInterpolated;
                    u32 color = ((u32)(res.x * 0xff) << 16) |
                                ((u32)(res.y * 0xff) << 8) |
                                ((u32)(res.z * 0xff) << 0);
                    bitmap.pixels[y * bitmap.width + x] = color;
                }
            }
        }
}

f32 appTimeSec;
RandomSeries series;

void WinMainCRTStartup()
{
    PreventWindowsDPIScaling();

    InitPerf();

    series = CreateSeries();
    HWND window = OpenAppWindowWithSize(GetModuleHandle(0), OnEvent, 1600, 1600);
    HDC dc = GetDC(window);
    MSG msg;
    f32 prevFrameTimeSec = 0;

    V4f vertices[] = {
        // front-face
        V4(-0.5, -0.5, -0.5),
        V4(-0.5, 0.5, -0.5),
        V4(0.5, 0.5, -0.5),
        V4(0.5, -0.5, -0.5),

        // back-face
        V4(-0.5, -0.5, 0.5),
        V4(-0.5, 0.5, 0.5),
        V4(0.5, 0.5, 0.5),
        V4(0.5, -0.5, 0.5),
    };

    V3f colors[] = {
        V3(0, 0, 0),
        V3(0, 0, 0),
        V3(0, 0, 0),
        V3(0, 0, 0),

        V3(1, 1, 1),
        V3(1, 1, 1),
        V3(1, 1, 1),
        V3(1, 1, 1),
    };

    u32 modelIndices[] = {
        0, 1, 2, // front
        2, 3, 0,

        6, 5, 4, // back
        4, 7, 6,

        4, 5, 1, // left
        1, 0, 4,

        3, 2, 6, // right
        6, 7, 3,

        1, 5, 6, // top
        6, 2, 1,

        4, 0, 3, // bottom
        3, 7, 4};

    while (isRunning)
    {
        StartMetric(Overall);
        while (PeekMessageA(&msg, 0, 0, 0, 1))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        memset(bitmap.pixels, 0x22, bitmap.height * bitmap.width * 4);

        // some big Z
        for (int i = 0; i < bitmap.width * bitmap.height; i++)
            zBuffer[i] = F32_MAX;

        f32 sin;
        f32 cos;
        MySinCos(appTimeSec, &sin, &cos);

        Mat4 mat =
            Mat4Mult(Mat4TranslateXYZ(0, 0, 4 + sin * 2),
                     Mat4RotateXYZ(appTimeSec / 3, appTimeSec / 4, appTimeSec / 2.5));

        for (int i = 0; i < ArrayLength(modelIndices); i += 3)
        {
            Triangle trig;

            trig.v1 = vertices[modelIndices[i]];
            trig.v2 = vertices[modelIndices[i + 1]];
            trig.v3 = vertices[modelIndices[i + 2]];

            trig.c1 = colors[modelIndices[i]];
            trig.c2 = colors[modelIndices[i + 1]];
            trig.c3 = colors[modelIndices[i + 2]];

            FillTriangleInterpolatedColors(trig, mat);
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
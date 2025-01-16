#include <stdio.h>
#include <windows.h>
#include <stdint.h>

#include "./include/definitions.h"
#include "./include/win32_window_dimensions.h"
#include "./include/Functions/GetWindowDimensions.h"

using namespace std;

// Method 2 of closing window
GLOBAL_VARIABLE bool GlobalRunning = true;

// TODO(ARNAV:DONE) note all answers in physical notes as well.
// QUESTION(SOLVED): What is the use of these three (BitmapInfo ,BitmapMemory, BitmapHandle)?
// ANSWER(BitmapInfo): Stores metadata about the bitmap being used, like color depth, compresstion method.
// ANSWER(BitmapMemory): Points to the raw memory where bitmap is present in the memory.
// ANSWER(BitmapHandle): Handle to the bitmap object, used by application to manage the bitmap.
struct win32_offscreen_buffer
{
    BITMAPINFO Info;
    void *Memory;
    int Width;
    int Height;
    int BytesPerPixel;
};

GLOBAL_VARIABLE win32_offscreen_buffer Bitmap = {0};

// INFO(ARNAV) DIB = Device Independent Bitmap

INTERNAL void
Win32RenderWeirdGradient(int XOffset, int YOffset, win32_offscreen_buffer *Bitmap)
{
    int Pitch = Bitmap->Width * Bitmap->BytesPerPixel;
    uint8_t *Row = (uint8_t *)Bitmap->Memory;
    for (int y = 0; y < Bitmap->Height; y++)
    {
        uint32_t *Pixel = (uint32_t *)Row;
        for (int x = 0; x < Bitmap->Width; x++)
        {
            // INFO(ARNAV): R is at last because the format is little endian.
            uint8_t Green = (((x + XOffset) * 255));
            uint8_t Blue = (((y + YOffset) * 255));
            // uint8_t Green = (((x + XOffset) * 255)) / Bitmap->Width;
            // uint8_t Blue = (((y + YOffset) * 255)) / Bitmap->Height;
            // *Pixel++ = (0xa1 | (Green << 16) | (Blue << 16));
            *Pixel++ = (0xa1 | (Green << 16) | (Blue << 8));
        }

        Row += Pitch;
    }
}

INTERNAL void Win32ResizeDIBSection(int width, int height, win32_offscreen_buffer *Bitmap)
{
    if (Bitmap->Memory)
    {
        VirtualFree(Bitmap->Memory, 0, MEM_RELEASE);
    }

    Bitmap->Height = height;
    Bitmap->Width = width;

    // INFO(ARNAV): bmiHeader structure defines the size, dimensions, and color format of a bitmap image.
    Bitmap->Info.bmiHeader.biSize = sizeof(Bitmap->Info.bmiHeader); // INFO(ARNAV): Size of the header
    Bitmap->Info.bmiHeader.biWidth = Bitmap->Width;                 // INFO(ARNAV): Client width that means actual drawing width.
    Bitmap->Info.bmiHeader.biHeight = -Bitmap->Height;              // INFO(ARNAV): Client height that means actual drawing height.
    Bitmap->Info.bmiHeader.biPlanes = 1;                            // INFO(ARNAV): Always 1 kept for historical reasons.
    Bitmap->Info.bmiHeader.biBitCount = 32;                         // INFO(ARNAV): Actually 24 bits are required 8 bits for each R, G, B but 32 bits are marked for it to be DWORD alligned which improves performance, The extra 8 bits are unused or can store an alpha channel for transparency.
    Bitmap->Info.bmiHeader.biCompression = BI_RGB;                  // INFO(ARNAV): This is used to store what is the compression method used for frame buffer

    // QUESTION(SOLVED): What does negative biHeight means?
    // ANSWER(-Height): When biHeight is negative, the bitmap is a top-down DIB This means that the origin of
    // ANSWER(CONTINUED) the bitmap is the upper-left corner, and the bitmap is stored in memory starting from
    // ANSWER(CONTINUED) the top row to the bottom row.
    // ANSWER(+Height): When biHeight is positive, the bitmap is a bottom-up DIB This means that the origin of the bitmap is the upper-left corner

    Bitmap->BytesPerPixel = 4;
    // INFO(lpAddress): 0 means we don't care where we get the memory.
    Bitmap->Memory = VirtualAlloc(0, Bitmap->BytesPerPixel * width * height, MEM_COMMIT, PAGE_READWRITE);
}

INTERNAL void Win32UpdateWindow(HDC DeviceContext, int WindowWidth, int WindowHeight, int x, int y, int width, int height, win32_offscreen_buffer *Bitmap)
{
    StretchDIBits(DeviceContext,
                  0, 0, WindowWidth, WindowHeight,
                  0, 0, Bitmap->Width, Bitmap->Height,
                  Bitmap->Memory, &Bitmap->Info, DIB_RGB_COLORS, SRCCOPY);
}

LRESULT MainWindowCallback(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    switch (Message)
    {
    case WM_CREATE:
    {
        OutputDebugStringA("WM_CREATE\n");
        break;
    }
    case WM_SIZE:
    {
        win32_window_dimensions Dimensions = GetWindowDimensions(Window);
        Win32ResizeDIBSection(1280, 720, &Bitmap);
        OutputDebugStringA("WM_SIZE\n");
        break;
    }
    case WM_DESTROY:
    {
        DestroyWindow(Window); // Method 1 for closing window
        GlobalRunning = false; // Method 2 for closing window
        OutputDebugStringA("WM_DESTROY\n");
        break;
    }
    case WM_CLOSE:
    {
        PostQuitMessage(0);    // Method 1 for closing window
        GlobalRunning = false; // Method 2 for closing window
        OutputDebugStringA("WM_CLOSE\n");
        break;
    }
    case WM_ACTIVATEAPP:
    {
        OutputDebugStringA("WM_ACTIVATEAPP\n");
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT Paint;
        HDC DeviceContext = BeginPaint(Window, &Paint);

        // HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));
        // FillRect(DeviceContext, &Paint.rcPaint, brush);
        // DeleteObject(brush);

        int X = Paint.rcPaint.left;
        int Y = Paint.rcPaint.top;
        int width = Paint.rcPaint.right - X;
        int height = Paint.rcPaint.bottom - Y;

        win32_window_dimensions Dimensions = GetWindowDimensions(Window);

        Win32UpdateWindow(DeviceContext, Dimensions.Width, Dimensions.Height, X, Y, width, height, &Bitmap);

        EndPaint(Window, &Paint);
    }
    break;
    default:
    {
        result = DefWindowProc(Window, Message, wParam, lParam);
        OutputDebugStringA("Default\n");
        break;
    }
    }
    return result;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR CommandLine, int SHhwCode)
{

    // AllocConsole();
    // freopen("CONOUT$", "w", stdout); // Redirect stdout to console
    // freopen("CONOUT$", "w", stderr); // Redirect stderr to console
    // printf("Console initialized\n");

    int test[1024 * 1024 * 2] = {0};
    WNDCLASS WindowClass = {0};

    // INFO(HREDRAW): This option says that if we resize the window 'horizontally' then
    // INFO(CONTINUED): whole window should repaint, instead of painting only the new
    // INFO(CONTINUED): part of the window.
    // INFO(VREDRAW): This option says that if we resize the window 'vertically' then
    // INFO(CONTINUED): whole window should repaint, instead of painting only the new
    // INFO(CONTINUED): part of the window.
    WindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW; // ?| is Used when you have to add multiple options
    WindowClass.lpfnWndProc = MainWindowCallback;
    WindowClass.hInstance = hInstance;
    WindowClass.lpszClassName = "Game Development";

    printf("Window class setup complete.\n");
    if (RegisterClass(&WindowClass))
    {
        printf("Window registeration successful\n");

        HWND WindowHandle = CreateWindowEx(
            0,
            WindowClass.lpszClassName,
            "Making Game",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SIZEBOX | WS_SYSMENU | WS_MAXIMIZEBOX,
            CW_USEDEFAULT, CW_USEDEFAULT,
            1024, 1024 * 9 / 16,
            NULL, NULL, hInstance, NULL);

        if (WindowHandle)
        {
            printf("Window handle created successfully");
            MSG Message;
            int x_offset = 0;
            int y_offset = 0;

            HDC DeviceContext = GetDC(WindowHandle);
            RECT ClientRect;
            BOOL success = GetClientRect(WindowHandle, &ClientRect);

            int WindowWidth = ClientRect.right - ClientRect.left;
            int WindowHeight = ClientRect.bottom - ClientRect.top;

            while (GlobalRunning)
            {
                while (PeekMessageA(&Message, 0, 0, 0, PM_REMOVE))
                {
                    if (Message.message == WM_QUIT)
                    {
                        GlobalRunning = false;
                    }
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }

                Win32RenderWeirdGradient(x_offset, y_offset, &Bitmap);
                win32_window_dimensions Dimensions = GetWindowDimensions(WindowHandle);
                Win32UpdateWindow(DeviceContext, Dimensions.Width, Dimensions.Height, 0, 0, WindowWidth, WindowHeight, &Bitmap);
                y_offset += 1;
                x_offset += 1;
            }
        }
        else
        {
            printf("Window creation failed.");
        }
    }
    else
    {
        printf("Failed Registering the class\n");
    }
    return 0;
}
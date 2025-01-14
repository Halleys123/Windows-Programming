#include <stdio.h>
#include <windows.h>

#define INTERNAL static
#define GLOBAL_VARIABLE static
#define PERSISTENT_VARIABLE static

using namespace std;

// Method 2 of closing window
GLOBAL_VARIABLE bool Running = true;

// TODO(ARNAV:DONE) note all answers in physical notes as well.
// QUESTION(SOLVED): What is the use of these three (BitmapInfo ,BitmapMemory, BitmapHandle)?
// ANSWER(BitmapInfo): Stores metadata about the bitmap being used, like color depth, compresstion method.
// ANSWER(BitmapMemory): Points to the raw memory where bitmap is present in the memory.
// ANSWER(BitmapHandle): Handle to the bitmap object, used by application to manage the bitmap.
GLOBAL_VARIABLE BITMAPINFO BitmapInfo;
GLOBAL_VARIABLE void *BitmapMemory;
GLOBAL_VARIABLE HBITMAP BitmapHandle;

// INFO(ARNAV) DIB = Device Independent Bitmap
INTERNAL void Win32ResizeDIBSection(int width, int height)
{
    if (BitmapHandle)
    {
        DeleteObject(BitmapHandle);
    }

    // INFO(ARNAV): bmiHeader structure defines the size, dimensions, and color format of a bitmap image.
    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader); // INFO(ARNAV): Size of the header
    BitmapInfo.bmiHeader.biWidth = width;                       // INFO(ARNAV): Client width that means actual drawing width.
    BitmapInfo.bmiHeader.biHeight = height;                     // INFO(ARNAV): Client height that means actual drawing height.
    BitmapInfo.bmiHeader.biPlanes = 1;                          // INFO(ARNAV): Always 1 kept for historical reasons.
    BitmapInfo.bmiHeader.biBitCount = 32;                       // INFO(ARNAV): Actually 24 bits are required 8 bits for each R, G, B but 32 bits are marked for it to be DWORD alligned which improves performance, The extra 8 bits are unused or can store an alpha channel for transparency.
    BitmapInfo.bmiHeader.biCompression = BI_RGB;                // INFO(ARNAV): This is used to store what is the compression method used for frame buffer

    // QUESTION(UNSOLVED): What exactly is device context?
    HDC DeviceContext = CreateCompatibleDC(0);

    BitmapHandle = CreateDIBSection(
        DeviceContext, &BitmapInfo,
        DIB_RGB_COLORS,
        &BitmapMemory,
        0, 0);
}

INTERNAL void Win32UpdateWindow(HDC DeviceContext, int x, int y, int width, int height)
{
    StretchDIBits(DeviceContext,
                  x, y, width, height,
                  x, y, width, height,
                  BitmapMemory, &BitmapInfo, DIB_RGB_COLORS, SRCCOPY);
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
        RECT ClientRect;
        BOOL success = GetClientRect(Window, &ClientRect);
        if (success > 0)
        {
            int width = ClientRect.right - ClientRect.left;
            int height = ClientRect.bottom - ClientRect.top;
            Win32ResizeDIBSection(width, height);
        }
        OutputDebugStringA("WM_SIZE\n");
        break;
    }
    case WM_DESTROY:
    {
        DestroyWindow(Window); // Method 1 for closing window
        Running = false;       // Method 2 for closing window
        OutputDebugStringA("WM_DESTROY\n");
        break;
    }
    case WM_CLOSE:
    {
        PostQuitMessage(0); // Method 1 for closing window
        Running = false;    // Method 2 for closing window
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

        Win32UpdateWindow(DeviceContext, X, Y, width, height);

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
    AllocConsole();
    freopen("CONOUT$", "w", stdout); // Redirect stdout to console
    freopen("CONOUT$", "w", stderr); // Redirect stderr to console
    printf("Console initialized\n");

    WNDCLASS WindowClass = {0};

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
            WS_OVERLAPPED | WS_VISIBLE | WS_SIZEBOX | WS_SYSMENU,
            CW_USEDEFAULT, CW_USEDEFAULT,
            1024, 1024 * 9 / 16,
            NULL, NULL, hInstance, NULL);

        if (WindowHandle)
        {
            printf("Window handle created successfully");
            MSG Message;
            while (true)
            {
                BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
                if (MessageResult > 0)
                {
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }
                else
                {
                    if (MessageResult == -1)
                        MessageBox(0, TEXT("There was some error in the windows"), TEXT("Error"), MB_RETRYCANCEL | MB_ICONERROR);
                    break;
                }
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
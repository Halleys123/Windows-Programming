#include <stdio.h>
#include <windows.h>
#include <stdint.h>
#include <Xinput.h>
#include <Dsound.h>

#include "./include/definitions.h"
#include "./include/win32_window_dimensions.h"
#include "./include/Functions/GetWindowDimensions.h"

using namespace std;

// XINPUT is a library that allows communication with Xbox controllers.
// It provides functions to handle input from Xbox 360 and Xbox One controllers.
// XINPUT supports features such as reading controller states, handling button presses,
// detecting connected controllers, and managing controller vibration (rumble).
// ---
// When you see someone saying "XInput shows in DirectInput,"
// it means that an XInput-compatible device (such as an Xbox controller) is recognized by the DirectInput API
// ---
// When you see "XInput shows in DirectInput," it means that an
// XInput-compatible device (such as an Xbox controller) is recognized by the DirectInput API.
// However, there are some limitations when using XInput devices through DirectInput
// ---
// It shows up in Direct Input but has limitations:
// - You can't send rumble (vibration) messages directly.
// - You can't get an audio handle to the controllers.

// There are two methods to receive messages from the controller:
// 1. Polling: Continuously check for changes from the controller at a fixed rate.
// 2. Interrupts: The controller sends signals to update the screen when there is a change.
// The XInput API requires polling for inputs rather than using interrupts.

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

// ! By using typedef and creating function pointers, this code achieves dynamic linking or runtime flexibility
// ! You can load the XInputGetState function from a DLL dynamically at runtime using LoadLibrary and GetProcAddress
// INFO(Stubs): Stubs are function that are like safegaurd when required function is not present.
// INFO(ARNAV): These lines create new types (x_input_get_state and x_input_set_state) that represent the function signatures of XInputGetState and XInputSetState.
// The parameters are the same as the original XInputGetState and XInputSetState functions.

#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub)
{
    return ERROR_DEVICE_NOT_CONNECTED;
}
GLOBAL_VARIABLE x_input_get_state *XInputGetState_ = XInputGetStateStub;

#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub)
{
    return ERROR_DEVICE_NOT_CONNECTED;
}
GLOBAL_VARIABLE x_input_set_state *XInputSetState_ = XInputSetStateStub;

INTERNAL void Win32LoadXInput()
{
    HMODULE XInputLibrary = LoadLibrary("xinput1_4.dll");
    if (!XInputLibrary)
        XInputLibrary = LoadLibrary("xinput1_3.dll");

    if (XInputLibrary)
    {
        XInputGetState_ = (x_input_get_state *)GetProcAddress(XInputLibrary, "XInputGetState");
        XInputSetState_ = (x_input_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");
    }
}

#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPGUID lpGuid, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE(direct_sound_create);
DIRECT_SOUND_CREATE(DirectSoundCreateStub)
{
    return ERROR_DEVICE_NOT_AVAILABLE;
}

INTERNAL void Win32LoadSound(HWND Window, int32_t buffer_size, int32_t SamplePerSec)
{
    direct_sound_create *DirectSoundCreate_ = DirectSoundCreateStub;
    HMODULE SoundLib = LoadLibrary("dsound.dll");
    if (SoundLib)
    {
        printf("SOUND: Sound Libarary found: Game will be able to play sound.\n");
        DirectSoundCreate_ = (direct_sound_create *)GetProcAddress(SoundLib, "DirectSoundCreate");
        // DIRECTSOUND *DirectSound;
        LPDIRECTSOUND DirectSound;
        if (DirectSoundCreate_ && SUCCEEDED(DirectSoundCreate_(0, &DirectSound, 0)))
        {
            printf("SOUND: SoundCreate working...\n");
            WAVEFORMATEX WaveFormat = {};
            WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
            WaveFormat.nChannels = 2;
            WaveFormat.nSamplesPerSec = SamplePerSec;
            WaveFormat.nBlockAlign = (WaveFormat.nChannels * WaveFormat.wBitsPerSample) / 8;
            WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec * WaveFormat.nBlockAlign;
            WaveFormat.wBitsPerSample = 16;
            WaveFormat.cbSize;

            if (SUCCEEDED(DirectSound->SetCooperativeLevel(Window, DSSCL_PRIORITY)))
            {
                printf("SOUND: SetCooperativeLevel complete.\n");
                DSBUFFERDESC BufferDescription = {};
                BufferDescription.dwSize = sizeof(BufferDescription);
                BufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;

                LPDIRECTSOUNDBUFFER PrimaryBuffer;
                if (SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription, &PrimaryBuffer, 0)))
                {
                    printf("SOUND: Sound Buffer created.\n");
                    if (SUCCEEDED(PrimaryBuffer->SetFormat(&WaveFormat)))
                    {
                        printf("SOUND: Format set succesfully.\n");
                    }
                    else
                    {
                    }
                }
                else
                {
                }
            }
            else
            {
            }

            DSBUFFERDESC BufferDescription = {};

            BufferDescription.dwSize = sizeof(BufferDescription);
            BufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;
            BufferDescription.dwBufferBytes = buffer_size;
            BufferDescription.lpwfxFormat = &WaveFormat;

            LPDIRECTSOUNDBUFFER SecondaryBuffer;
            if (SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription, &SecondaryBuffer, 0)))
            {
                printf("SOUND: Secondary sound buffer set succefully\n");
            }
        }
    }
}

// INFO(LoadLibrary): This is the function provided by the windows to load dll dynamically.

// INFO(XINPUTSTATE): (XInputGetState_ and XInputSetState_) are pointers to functions that match the x_input_get_state and x_input_set_state typedefs, respectively
// INFO(XINPUTSTATE): These global variables can later be assigned to point to the actual XInputGetState and XInputSetState functions or to alternative implementations.

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
            uint8_t Green = ((x + XOffset) * 255) / Bitmap->Width;
            uint8_t Blue = ((y + YOffset) * 255) / Bitmap->Height;
            // uint8_t Green = (((x + XOffset) * 255)) / Bitmap->Width;
            // uint8_t Blue = (((y + YOffset) * 255)) / Bitmap->Height;
            // *Pixel++ = (0xa1 | (Green <<  16) | (Blue << 16));
            *Pixel++ = (0x88 | (Green << 16) | (Blue << 8));
                }

        Row += Pitch;
    }
}
INTERNAL void Win32MirronImage(int XOffset, int YOffset, win32_offscreen_buffer *Bitmap)
{
    int Pitch = Bitmap->Width * Bitmap->BytesPerPixel;

    uint8_t *Row = (uint8_t *)Bitmap->Memory;
    for (int y = 0; y < Bitmap->Height; y++)
    {
        uint32_t *Pixel = (uint32_t *)Row;
        for (int x = 0; x < Bitmap->Width; x++)
        {
            // INFO(ARNAV): R is at last because the format is little endian.
            uint8_t Green = ((x + XOffset) * 255) / Bitmap->Width;
            uint8_t Blue = ((y + YOffset) * 255) / Bitmap->Height;
            // uint8_t Green = (((x + XOffset) * 255)) / Bitmap->Width;
            // uint8_t Blue = (((y + YOffset) * 255)) / Bitmap->Height;
            // *Pixel++ = (0xa1 | (Green <<  16) | (Blue << 16));
            *Pixel++ = (0x88 | (Green << 16) | (Blue << 8));
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

char keyPressed = 0b00000000;

LRESULT MainWindowCallback(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    static int time;

    static bool left = true;

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
    case WM_SYSKEYUP:
    {
        break;
    }
    case WM_SYSKEYDOWN:
    {
        uint32_t VKCode = wParam;
        if (VKCode == VK_F4)
        {
            // alt + f4 shortcut
            PostQuitMessage(0);
        }
    }
    break;
    case WM_KEYDOWN:
    {
        uint32_t VirtalKeyCode = wParam;

        if (VirtalKeyCode == 0x53) // o
        {
            keyPressed |= 0b00000100;
        }
        else if (VirtalKeyCode == 0x57)
        {
            keyPressed |= 0b00001000;
        }
        if (VirtalKeyCode == 0x41)
        {
            keyPressed |= 0b00000010;
        }
        else if (VirtalKeyCode == 0x44)
        {
            keyPressed |= 0b00000001;
        }

        break;
    }
    case WM_KEYUP:
    {
        uint32_t VirtalKeyCode = wParam;

        if (VirtalKeyCode == 0x53)
        {
            keyPressed &= !0b00000100;
        }
        else if (VirtalKeyCode == 0x57)
        {
            keyPressed &= !0b00001000;
        }
        if (VirtalKeyCode == 0x41)
        {
            keyPressed &= !0b0000010;
        }
        else if (VirtalKeyCode == 0x44)
        {
            keyPressed &= !0b00000001;
        }
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

    AllocConsole();
    freopen("CONOUT$", "w", stdout); // Redirect stdout to console
    freopen("CONOUT$", "w", stderr); // Redirect stderr to console
    printf("Console initialized\n");
    Win32LoadXInput();
    WNDCLASS WindowClass = {0};
    // ! In Simple terms steps for creating a window are:
    // 1. Make a window class
    // 1.1 Make and add window procedure to window class.
    // 2. Register that class
    // 3. Make window handle from that class
    // 4. If there is a handle present then present start message loop either with PeekMessage or GetMessage (not sure if it was called GetMessage)

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
            "Making Game in Windows",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SIZEBOX | WS_SYSMENU | WS_MAXIMIZEBOX,
            CW_USEDEFAULT, CW_USEDEFAULT,
            1024, 1024 * 9 / 16,
            NULL, NULL, hInstance, NULL);

        if (WindowHandle)
        {
            Win32LoadSound(WindowHandle, 48000, 48000);

            printf("Window handle created successfully");
            MSG Message;

            HDC DeviceContext = GetDC(WindowHandle);
            RECT ClientRect;
            BOOL success = GetClientRect(WindowHandle, &ClientRect);

            int WindowWidth = ClientRect.right - ClientRect.left;
            int WindowHeight = ClientRect.bottom - ClientRect.top;

            int x_offset = 0;
            int y_offset = 0;
            int x_acc = 0;
            int y_acc = 0;
            int acc_limit = 1000;

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

                for (DWORD ControllerIndex = 0; ControllerIndex < 4; ControllerIndex += 1)
                {
                    XINPUT_STATE ControllerState;
                    if (XInputGetState_(ControllerIndex, &ControllerState) == ERROR_SUCCESS)
                    {
                        bool DPadUp = (ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP);
                        bool DPadDown = (ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
                        bool DPadLeft = (ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
                        bool DPadRight = (ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
                        bool Start = (ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_START);
                        bool Back = (ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK);
                        bool LeftShoulder = (ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
                        bool RightShoulder = (ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
                        bool AButton = (ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_A);
                        bool BButton = (ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_B);
                        bool XButton = (ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_X);
                        bool YButton = (ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_Y);

                        int StickX = ControllerState.Gamepad.sThumbLX;
                        int StickY = ControllerState.Gamepad.sThumbLY;
                    }
                }

                if (keyPressed & (1 << 3))
                {
                    if (y_acc < acc_limit)
                        y_acc += 1;
                }
                else if (keyPressed & (1 << 2))
                {
                    if (y_acc > -acc_limit)
                        y_acc -= 1;
                }
                else
                {
                    if (y_acc > 0)
                        y_acc -= 1;
                    else if (y_acc < 0)
                        y_acc += 1;
                }

                if (keyPressed & (1 << 1))
                {
                    if (x_acc < acc_limit)
                        x_acc += 1;
                }
                else if (keyPressed & (1 << 0))
                {
                    if (x_acc > -acc_limit)
                        x_acc -= 1;
                }
                else
                {
                    if (x_acc > 0)
                        x_acc -= 1;
                    else if (x_acc < 0)
                        x_acc += 1;
                }

                x_offset += x_acc / 50;
                y_offset += y_acc / 50;

                // printf("x_offset: %d\ny_offset: %d\nx_acc: %d\ny_acc: %d\n", x_offset, y_offset, x_acc, y_acc);

                Win32RenderWeirdGradient(x_offset, y_offset, &Bitmap);
                win32_window_dimensions Dimensions = GetWindowDimensions(WindowHandle);
                Win32UpdateWindow(DeviceContext, Dimensions.Width, Dimensions.Height, 0, 0, WindowWidth, WindowHeight, &Bitmap);
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
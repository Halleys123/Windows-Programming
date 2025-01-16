#include <windows.h>
#include "../include/definitions.h"
#include "../include/win32_window_dimensions.h"

INTERNAL win32_window_dimensions GetWindowDimensions(HWND WindowHandle)
{
    RECT ClientRect;
    BOOL success = GetClientRect(WindowHandle, &ClientRect);

    win32_window_dimensions dimensions;
    dimensions.Height = ClientRect.bottom - ClientRect.top;
    dimensions.Width = ClientRect.right - ClientRect.left;

    return dimensions;
}

// main.cpp

#include <iostream>
#include <string>

#include <Windows.h>

void PrintLastError()
{
    //Get the error message ID, if any.
    std::cerr << "error happened" << std::endl;
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0)
    {
        return; //No error message has been recorded
    }
    LPSTR messageBuffer = nullptr;
    //Ask Win32 to give us the string version of that message ID.
    //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
    //Copy the error message into a std::string.
    std::string message(messageBuffer, size);
    //Free the Win32's string's buffer.
    LocalFree(messageBuffer);
    //Print message
    std::cerr << message << std::endl;
}

void registerHotkeys()
{
    UINT modifiers = /*MOD_ALT |*/ MOD_NOREPEAT;
    RegisterHotKey(NULL, 0, modifiers, VK_NUMPAD8);
    RegisterHotKey(NULL, 1, modifiers, VK_NUMPAD2);
    RegisterHotKey(NULL, 2, modifiers, VK_NUMPAD4);
    RegisterHotKey(NULL, 3, modifiers, VK_NUMPAD6);
    RegisterHotKey(NULL, 4, modifiers, VK_NUMPAD5);
    RegisterHotKey(NULL, 5, modifiers, VK_NUMPAD0);
}

void SendKey(HWND window, char key, bool activate = true)
{
    std::cout << "sending key" << std::endl;

    HWND currentWindow = NULL;

    if (activate)
    {
        currentWindow = GetForegroundWindow();
        if (currentWindow != window)
        {
            SetForegroundWindow(window);
        }
    }

    LRESULT status = 0;
    UINT message = WM_KEYDOWN;
    WPARAM wparam = key;
    LPARAM lparam = 1;

    status = SendMessage(window, message, wparam, lparam);
    if (status != 0) PrintLastError();
    if (key >= 'A' && key <= 'Z')
    {
        message = WM_CHAR;
        status = SendMessage(window, message, wparam, lparam);
        if (status != 0) PrintLastError();
    }
    message = WM_KEYUP;
    status = SendMessage(window, message, wparam, lparam);
    if (status != 0) PrintLastError();

    if (activate && currentWindow != window)
    {
        SetForegroundWindow(currentWindow);
    }
}

int main()
{
    registerHotkeys();

    HWND window = NULL;
    std::cout << "focus your target window now" << std::endl;
    Sleep(2000);
    window = GetForegroundWindow();
    std::cout << "target window is: " << window << std::endl;

    // message event loop
    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0) != 0)
    {
        if (msg.message == WM_HOTKEY)
        {
            switch (msg.wParam)
            {
            case 0: 
                SendKey(window, VK_UP); break;
            case 1:
                SendKey(window, VK_DOWN); break;
            case 2:
                SendKey(window, VK_LEFT); break;
            case 3:
                SendKey(window, VK_RIGHT); break;
            case 4:
                SendKey(window, ' '); break;
            case 5:
                SendKey(window, VK_F5); break;
            default:
                std::cerr << "error: unhandled hotkey index recieved" << std::endl; break;
            }
        }
    }
    return 0;
}
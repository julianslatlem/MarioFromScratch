#include "input.h"
#include "window.h"

namespace Input {
    bool GetKeyDown(int button) {
        for (int key = 0; key < 256; ++key) {
            if (GetAsyncKeyState(key) & 0x8000) {
                if (key == button) return true;
            }
        }

        return false;
    }

    bool GetKeyPressed(int button) {
        static bool keyStates[256] = { false };

        bool isPressed = (GetAsyncKeyState(button) & 0x8000) != 0;

        if (isPressed && !keyStates[button]) {
            keyStates[button] = true;
            return true;
        }
        else if (!isPressed) {
            keyStates[button] = false;
        }

        return false;
    }

    bool GetKeyReleased(int button) {
        static bool keyStates[256] = { false };

        bool isPressed = (GetAsyncKeyState(button) & 0x8000) != 0;

        if (!isPressed && keyStates[button]) {
            keyStates[button] = false;
            return true;
        }
        else if (isPressed) {
            keyStates[button] = true;
        }

        return false;
    }

    void GetWindowMousePosition(Window* window, int& mouseX, int& mouseY) {
        POINT cursorPos;
        if (GetCursorPos(&cursorPos)) {
            HDC hDC = GetDC(window->hwnd);
            int dpiX = GetDeviceCaps(hDC, LOGPIXELSX);
            int dpiY = GetDeviceCaps(hDC, LOGPIXELSY);
            ReleaseDC(window->hwnd, hDC);

            cursorPos.x = MulDiv(cursorPos.x, dpiX, 96);
            cursorPos.y = MulDiv(cursorPos.y, dpiY, 96);

            if (ScreenToClient(window->hwnd, &cursorPos)) {
                mouseX = cursorPos.x;
                mouseY = cursorPos.y;
                return;
            }
        }
    }

    int GetScrollDelta(Window* window) {
        int scrollDelta = window->scrollDelta;
        window->scrollDelta = 0;
        return scrollDelta;
    }
}
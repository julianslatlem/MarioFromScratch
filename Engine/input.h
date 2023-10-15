#include "pch.h"

#ifndef INPUT_H
#define INPUT_H

#include "window.h"

namespace KeyCode {
    enum KeyCode {
        LEFT_MOUSE_BUTTON = 1,
        RIGHT_MOUSE_BUTTON = 2,
        MIDDLE_MOUSE_BUTTON = 4,
        TAB = 9,
        SPACE = 32,
        LEFT = 37,
        UP = 38,
        RIGHT = 39,
        DOWN = 40,
        A = 65,
        B = 66,
        C = 67,
        D = 68,
        E = 69,
        F = 70,
        G = 71,
        H = 72,
        I = 73,
        J = 74,
        K = 75,
        L = 76,
        M = 77,
        N = 78,
        O = 79,
        P = 80,
        Q = 81,
        R = 82,
        S = 83,
        T = 84,
        U = 85,
        V = 86,
        W = 87,
        X = 88,
        Y = 89,
        Z = 90,
        CTRL = 162,
        RIGHT_CTRL = 163,
        ALT = 164,
    };
}

namespace Input {
    /*Checks if the specified key is currently pressed down.*/
    __declspec(dllexport) bool GetKeyDown(int button);

    /*Checks if the specified key was pressed this frame.*/
    __declspec(dllexport) bool GetKeyPressed(int button);

    /*Checks if the specified key was released this frame.*/
    __declspec(dllexport) bool GetKeyReleased(int button);

    __declspec(dllexport) void GetWindowMousePosition(Window* window, int& mouseX, int& mouseY);

    __declspec(dllexport) int GetScrollDelta(Window* window);
}

#endif
#pragma once

#include <windows.h>

class DisplayWin32
{
public:
    int ClientWidth;
    int ClientHeight;
    // HINSTANCE Ч идентификатор экземпл€ра приложени€.
    HINSTANCE hInstance;
    // ’ендл окна
    HWND hWnd;
    // WNDCLASSEX Ч это структура, содержаща€ настройки класса окна.
    // ќна нужна, чтобы зарегистрировать окно в Windows API.
    WNDCLASSEX wc;

    // Ётот код определ€ет оконную процедуру WndProc, котора€ обрабатывает сообщени€ операционной системы дл€ окна приложени€
    // CALLBACK Ч специальное ключевое слово, указывающее, что функци€ используетс€ в качестве обработчика событий Windows API.
    // hwnd Ч идентификатор окна, которому предназначено сообщение.
    // umessage Ч тип полученного сообщени€.
    // wparam Ч дополнительный параметр, часто содержит код клавиши или флаг.
    // lparam Ч дополнительный параметр, содержащий информацию о позиции мыши, размере окна и т.д.
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);

    DisplayWin32() {}

    // LPCWSTR Ч это указатель на строку в формате Unicode. (L перед строкой означает, что строка хранитс€ в широких символах)
    DisplayWin32(LPCWSTR applicationName, int width, int height);  
};

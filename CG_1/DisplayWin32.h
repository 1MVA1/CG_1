#pragma once

#include <windows.h>

class DisplayWin32
{
public:
    int ClientWidth;
    int ClientHeight;
    // HINSTANCE � ������������� ���������� ����������.
    HINSTANCE hInstance;
    // ����� ����
    HWND hWnd;
    // WNDCLASSEX � ��� ���������, ���������� ��������� ������ ����.
    // ��� �����, ����� ���������������� ���� � Windows API.
    WNDCLASSEX wc;

    // ���� ��� ���������� ������� ��������� WndProc, ������� ������������ ��������� ������������ ������� ��� ���� ����������
    // CALLBACK � ����������� �������� �����, �����������, ��� ������� ������������ � �������� ����������� ������� Windows API.
    // hwnd � ������������� ����, �������� ������������� ���������.
    // umessage � ��� ����������� ���������.
    // wparam � �������������� ��������, ����� �������� ��� ������� ��� ����.
    // lparam � �������������� ��������, ���������� ���������� � ������� ����, ������� ���� � �.�.
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);

    DisplayWin32() {}

    // LPCWSTR � ��� ��������� �� ������ � ������� Unicode. (L ����� ������� ��������, ��� ������ �������� � ������� ��������)
    DisplayWin32(LPCWSTR applicationName, int width, int height);  
};

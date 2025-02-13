#include "DisplayWin32.h"
#include <iostream>

LRESULT CALLBACK DisplayWin32::WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// WM_KEYDOWN вызывается, когда нажимается клавиша на клавиатуре.
	case WM_KEYDOWN:
	{
		// If a key is pressed send it to the input object so it can record that state.
		std::cout << "Key: " << static_cast<unsigned int>(wparam) << std::endl;

		//  Если нажата клавиша Esc (код 27), вызывается PostQuitMessage(0), что завершает работу приложения.
		if (static_cast<unsigned int>(wparam) == 27) PostQuitMessage(0);
		return 0;
	}
	default:
	{
		// Передача остальных сообщений в стандартный обработчик
		return DefWindowProc(hwnd, umessage, wparam, lparam);
	}
	}
}

DisplayWin32::DisplayWin32(LPCWSTR applicationName, int width, int height) : ClientWidth(width), ClientHeight(height)
{
    // GetModuleHandle(nullptr); — получает экземпляр текущего процесса, который нужен при регистрации окна.
    hInstance = GetModuleHandle(nullptr);
    
	// wc.style определяет стиль поведения окна:
	// CS_HREDRAW — перерисовывать окно при изменении ширины.
	// CS_VREDRAW — перерисовывать окно при изменении высоты.
	// CS_OWNDC — выделять уникальный контекст устройства (DC) для каждого окна (важно для 3D-графики).
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	// lpfnWndProc — это указатель на оконную процедуру.
	// WndProc — функция, обрабатывающая сообщения окна (например, нажатия клавиш, закрытие и т. д.).
	wc.lpfnWndProc = WndProc;
	// cbClsExtra — дополнительные байты для класса окна (обычно 0).
	wc.cbClsExtra = 0;
	// cbWndExtra — дополнительные байты для экземпляра окна (обычно 0).
	wc.cbWndExtra = 0;
	// hInstance — экземпляр приложения
	wc.hInstance = hInstance;
	// hIcon — главная иконка окна.
	// LoadIcon(nullptr, IDI_WINLOGO) — загружает иконку Windows по умолчанию.
	wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	// hIconSm = wc.hIcon; — маленькая иконка окна (например, в заголовке).
	wc.hIconSm = wc.hIcon;
	// hCursor — курсор по умолчанию.
	// LoadCursor(nullptr, IDC_ARROW) — загружает стандартный указатель-стрелку.
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	// hbrBackground — цвет фона окна.
	// GetStockObject(BLACK_BRUSH) — делает фон чёрным.
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	// lpszMenuName — указывает ресурс меню для окна.
	// nullptr означает, что меню нет.
	wc.lpszMenuName = nullptr;
	// lpszClassName — имя класса окна(используется при создании окна).
	wc.lpszClassName = applicationName;
	// cbSize — размер структуры (обязательно для WNDCLASSEX).
	wc.cbSize = sizeof(WNDCLASSEX);

	// RegisterClassEx(&wc); — регистрирует класс окна в системе.
		// После этого можно создавать окно на основе этого класса.
	RegisterClassEx(&wc);

	// RECT — структура, содержащая координаты границ окна:
	//		left = 0, top = 0 (левый верхний угол).
	//		right = ClientWidth, bottom = ClientHeight(правый нижний угол).
	//		static_cast<LONG>(...) — приведение типа int → LONG (так требует RECT).
	RECT windowRect = { 0, 0, static_cast<LONG>(ClientWidth), static_cast<LONG>(ClientHeight) };
	// AdjustWindowRect(...) изменяет RECT, чтобы учесть границы окна:
	//		WS_OVERLAPPEDWINDOW — стандартный стиль окна с рамкой, заголовком и кнопками управления.
	//		FALSE — у окна нет меню.
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	// WS_SYSMENU — кнопка закрытия и системное меню (Alt + Space).
	// WS_CAPTION — заголовок окна.
	// WS_MINIMIZEBOX — кнопка "Свернуть".
	// WS_THICKFRAME — изменяемый размер окна.
	auto dwStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME;

	// GetSystemMetrics(SM_CXSCREEN) — ширина экрана.
	// GetSystemMetrics(SM_CYSCREEN) — высота экрана.
	// (ширина/высота экрана - ширина/высота окна) / 2 — позиция X/Y по центру.
	auto posX = (GetSystemMetrics(SM_CXSCREEN) - ClientWidth) / 2;
	auto posY = (GetSystemMetrics(SM_CYSCREEN) - ClientHeight) / 2;

	// Возвращает HWND (хендл окна), который нужен для управления окном.
	hWnd = CreateWindowEx(
		// WS_EX_APPWINDOW — окно будет отдельным приложением (в Alt+Tab).
		WS_EX_APPWINDOW,       				    // Дополнительный стиль окна
		applicationName,        				// Имя класса окна
		applicationName,        				// Заголовок окна
		dwStyle,								// Стиль окна
		posX, posY,								// Координаты (X, Y)
		windowRect.right - windowRect.left,		// Ширина
		windowRect.bottom - windowRect.top,		// Высота
		nullptr, nullptr, hInstance, nullptr	// Родитель, меню, экземпляр, параметры
	);

	// SW_SHOW делает окно видимым.
	ShowWindow(hWnd, SW_SHOW);
	// Переводит окно в фокус, чтобы оно стало активным.
	SetForegroundWindow(hWnd);
	// Гарантирует, что окно получает ввод с клавиатуры.
	SetFocus(hWnd);
	// Показывает курсор (если он скрыт).
	ShowCursor(true);
}
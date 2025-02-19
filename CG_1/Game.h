#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <chrono>

#include "DisplayWin32.h"
#include "GameComponent.h"
#include "Shader.h"

class Game
{
public:
	Shader shader;
	CameComponent gameComp;
	DisplayWin32 display;

	IDXGISwapChain* swapChain;
	ID3D11DeviceContext* context;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	std::chrono::time_point<std::chrono::steady_clock> PrevTime;
	float totalTime;
	unsigned int frameCount;
	ID3D11Texture2D* backBuff;
	ID3D11RenderTargetView* rendTargView;

	void Initialization(LPCWSTR applicationName, int width, int height);
	void CreateBackBuff();
	void PrepareResources();
	void Run();
	void Draw();
	void MessageHandler();
	void PrepareFrame();
	void Update();

private:
	bool isExitRequested;
	MSG msg;
};
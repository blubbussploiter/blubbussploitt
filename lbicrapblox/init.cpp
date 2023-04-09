#include "ui.h"

#include "auth.h"
#include "files.h"

#include <format>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

/* uhh i dont really understand d3d or dx shit so like i took sum from this (rest is mine tho) https://github.com/milkteaaa/Open-Source-Lua-Wrapper/blob/master/Lua-Wrapper/drawing.cpp */

HWND base;
LONG WNDProc;

LONG SW;
LONG SH;

/* our appliances */

namespace UserInterface 
{

	ID3D11Device* UIDevice;
	ID3D11DeviceContext* UIContext;
	ID3D11RenderTargetView* UITargetView;
	ID3D11Texture2D* UITargetTexture;

}

bool UserInterface::shouldRender = true;
bool isInitialized = false;

std::uintptr_t vtable;
typedef HRESULT(__stdcall* PresentFunctionHook)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
PresentFunctionHook OldPresent;

#define IMGUI_INI_PATH std::format("{}//imgui.ini", getExploitPath(dll))

HRESULT WINAPI renderingHook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) 
{
	
	if (!isInitialized)
	{
		isInitialized = true;

		pSwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&UserInterface::UIDevice));
		UserInterface::UIDevice->GetImmediateContext(&UserInterface::UIContext);

		ImGui::CreateContext();
		ImGui_ImplWin32_Init(base);
		ImGui_ImplDX11_Init(UserInterface::UIDevice, UserInterface::UIContext);
		ImGui_ImplDX11_CreateDeviceObjects();

		ImGui::GetIO().IniFilename = NULL;

		ImGui::StyleColorsDark();
	}
	
	if (!UserInterface::UITargetView)
	{
		pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&UserInterface::UITargetTexture));
		UserInterface::UIDevice->CreateRenderTargetView(UserInterface::UITargetTexture, nullptr, &UserInterface::UITargetView);
		UserInterface::UITargetTexture->Release();

		ImGui::CreateContext();
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if(UserInterface::shouldRender)
		UserInterface::renderUi();

	if (ImGui::IsKeyPressed(ImGuiKey_F2))
		UserInterface::shouldRender = not UserInterface::shouldRender;

	ImGui::EndFrame();
	ImGui::Render();
	UserInterface::UIContext->OMSetRenderTargets(1, &UserInterface::UITargetView, nullptr);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return OldPresent(pSwapChain, SyncInterval, Flags);
}

LRESULT IMGUI_IMPL_API onResize(HWND hWnd, UINT msg, WPARAM w_param, LPARAM l_param) 
{
	if (isInitialized)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, msg, w_param, l_param);
		switch (msg)
		{
			case WM_SIZE:
			{
				if (UserInterface::UITargetView)
				{
					UserInterface::UITargetView->Release();
					UserInterface::UITargetView = nullptr;
				}
				
				RECT RCT;
				GetWindowRect(hWnd, &RCT);

				SW = RCT.right - RCT.left;
				SH = RCT.bottom - RCT.top;
				
				break;
			}
			case WM_CLOSE:
			{
				exit(0);
				break;
			}
			default: break;
		}
	}
	return CallWindowProc(reinterpret_cast<WNDPROC>(WNDProc), hWnd, msg, w_param, l_param);
}

void UserInterface::init()
{
	if(FindWindowW(NULL, WIN_NAME) == NULL) /* if not window, wait for it */
		do { std::this_thread::sleep_for(std::chrono::seconds(1)); } while (FindWindowW(NULL, WIN_NAME) == NULL);
	base = FindWindowW(NULL, WIN_NAME);

	DXGI_SWAP_CHAIN_DESC SwapChainDesc = { 0 };
	SwapChainDesc.BufferCount = 1;
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	SwapChainDesc.OutputWindow = base;
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	SwapChainDesc.Windowed = true;

	D3D_FEATURE_LEVEL RequestedLevels[] = { D3D_FEATURE_LEVEL_11_0 };

	IDXGISwapChain* SwapChain = nullptr;
	ID3D11Device* Device;
	ID3D11DeviceContext* Context;

	if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, RequestedLevels, 1, D3D11_SDK_VERSION, &SwapChainDesc, &SwapChain, &Device, nullptr, &Context)))
		return;

	vtable = *reinterpret_cast<std::uintptr_t*>(SwapChain);
	OldPresent = *reinterpret_cast<PresentFunctionHook*>(vtable + 32);

	DWORD old;

	if (!VirtualProtect(reinterpret_cast<LPVOID>(vtable), 64, PAGE_READWRITE, &old))
		return;

	*reinterpret_cast<void**>(vtable + 32) = &renderingHook;

	if (!VirtualProtect(reinterpret_cast<LPVOID>(vtable), 64, old, &old))
		return;

	WNDProc = GetWindowLong(base, GWLP_WNDPROC);
	SetWindowLong(base, GWLP_WNDPROC, (LONG)onResize);

	RECT RCT;
	GetWindowRect(base, &RCT);

	SW = RCT.right - RCT.left;
	SH = RCT.bottom - RCT.top;

}
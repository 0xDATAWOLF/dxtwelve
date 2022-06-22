/**
 *	dxtwelve
 *		A DX12 test application.
 */
#include "main.h"
#include <string>
#include <vector>

#include <windows.h>
#include <wrl.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <d3d12sdklayers.h>

#include <assert.h>


/**
 * A simple singleton fetcher for app_state.
 */
internal inline app_state&
getAppState() { lpersist app_state _appState = {}; return _appState; }

// ---------------------------------------------------------------------------------------------------------------------
// Windows Platform
// ---------------------------------------------------------------------------------------------------------------------

/**
 * The window procedure that is called periodically by Windows or through user-interaction.
 */
LRESULT CALLBACK
WindowProcedure(HWND windowInstance, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
		case WM_DESTROY:
		{
			// WM_DESTROY indicates that the window is being destroyed, but we will treat it as a request to quit.
			PostQuitMessage(0);
			break;
		}

		case WM_CLOSE:
		{
			getAppState().isRunning = false; // Close the application.
			break;
		}

		case WM_PAINT:
		{
			PAINTSTRUCT ps = {};
			HDC deviceContext = BeginPaint(windowInstance, &ps);
			FillRect(deviceContext, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW+1));
			EndPaint(windowInstance, &ps);
			break;
		}

		default:
		{
			break;
		}
	}

	return DefWindowProc(windowInstance, message, wParam, lParam);

}

/**
 * Application entry point.
 */
INT WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInstance, PSTR commandline, int commandShow)
{

	// Create the window class.
	WNDCLASSW windowClass = {};
	windowClass.lpfnWndProc = WindowProcedure;
	windowClass.lpszClassName = L"dxtwelve Window Class";
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	RegisterClassW(&windowClass);

	// Create a known client area size.
	RECT windowRectangle = {};
	int clientWidth = 1280;
	int clientHeight = 720;
	SetRect(&windowRectangle, 0, 0, clientWidth, clientHeight);
	AdjustWindowRect(&windowRectangle, WS_OVERLAPPEDWINDOW, FALSE);

	int windowWidth = windowRectangle.right - windowRectangle.left;
	int windowHeight = windowRectangle.bottom - windowRectangle.top;

	// Create the window instance.
	HWND windowInstance = CreateWindowExW(0, L"dxtwelve Window Class", L"dxtwelve", WS_OVERLAPPEDWINDOW,
										CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight, NULL,
										NULL, hInstance, NULL);

	// Ensure that the window is created.
	assert(windowInstance);

	// Initialize DX12.

	// -------------------------------------------------------------------------
	// Step 0 - enable the debug layer.
#if (defined(DEBUG) || defined(_DEBUG))
	{
		MSWRLComPtr<ID3D12Debug> debugController = {};
		HRESULT _debugIStatus = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
		assert(SUCCEEDED(_debugIStatus));
		debugController->EnableDebugLayer();
	}
#endif

	// -------------------------------------------------------------------------
	// Step 1 - Create the D3D12Device
	MSWRLComPtr<IDXGIFactory4> dxgiFactory = {};
	HRESULT _createDxgiFactoryStatus = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	assert(SUCCEEDED(_createDxgiFactoryStatus));

	// Enumerate over available adapters. This will show us all available adapters.
	std::vector<IDXGIAdapter1*> adapters;
	for (UINT i = 0;; ++i)
	{
		IDXGIAdapter1* currentAdapter = nullptr;
		HRESULT _enumStatus = dxgiFactory->EnumAdapters1(i, &currentAdapter);
		if (_enumStatus == DXGI_ERROR_NOT_FOUND) break;

		DXGI_ADAPTER_DESC1 adapterDescription = {};
		currentAdapter->GetDesc1(&adapterDescription);

		std::wstring descString = {};
		descString += L"***Adapter: ";
		descString += adapterDescription.Description;
		descString += L" Vendor: ";
		descString += std::to_wstring(adapterDescription.VendorId);
		descString += L" Subsys: ";
		descString += std::to_wstring(adapterDescription.SubSysId);
		descString += L" DVRAM: ";
		descString += std::to_wstring(adapterDescription.DedicatedVideoMemory);
		descString += L" Software?: ";
		b32 isSoftware = adapterDescription.Flags & DXGI_ADAPTER_FLAG_SOFTWARE;
		descString += (isSoftware == 0) ? L"No" : L"Yes";
		descString += L"\n";

		OutputDebugString(descString.c_str());

		adapters.push_back(currentAdapter);
	}

	// Attempt to get a hardware device.
	MSWRLComPtr<ID3D12Device> d3dDevice = {};
	HRESULT _createDxgiHWDevice = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3dDevice));
	if (FAILED(_createDxgiHWDevice))
	{
		// Fetch the WARP adapter.
		MSWRLComPtr<IDXGIAdapter> WARPAdapter = {};
		HRESULT _enumWarpAdStatus = dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&WARPAdapter));
		assert(SUCCEEDED(_enumWarpAdStatus));

		HRESULT _createDxgiSWDevice = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3dDevice));
		assert(SUCCEEDED(_createDxgiSWDevice));
	}

	// Execute the main loop.
	ShowWindow(windowInstance, commandShow); // Show the window.
	getAppState().isRunning = true;
	while (getAppState().isRunning)
	{

		// View and process the incoming messages on the queue.
		MSG currentMessage = {};
		while (PeekMessageW(&currentMessage, windowInstance, NULL, NULL, PM_REMOVE))
		{
			// If the application needs to quit, we will kill the loop and return from wWinMain.
			if (currentMessage.message == WM_QUIT) { getAppState().isRunning = false; return 0; }
			TranslateMessage(&currentMessage);
			DispatchMessage(&currentMessage);
		}



	}

	return 0;
}

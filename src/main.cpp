/**
 *	dxtwelve
 *		A DX12 test application.
 */
#include "main.h"
#include <windows.h>
#include <wrl.h>
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
wWinMain(HINSTANCE hInstance, HINSTANCE hPreviousInstance, PWSTR commandline, int commandShow)
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

	// Ensure that the window is created. Show the window if it is, otherwise explode.
	assert(windowInstance);
	ShowWindow(windowInstance, commandShow);

	// Execute the main loop.
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

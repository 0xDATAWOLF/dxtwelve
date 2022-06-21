# dxtwelve - A DirectX 12 Application From Scratch

This is a DirectX 12 application created from scratch. This project isn't meant
to be used for anything serious or permanent, just an exploratory project designed
to focus on the various features of modern DX12.

# Getting Started

This project requires [CMake](https://cmake.org) in order to configure and build
the project. [Visual Studio](https://visualstudio.microsoft.com/) is also necessary
to perform any level of debugging due to the coupling of the DX12 framework and Win32 APIs.
I recommend grabbing the latest version of both CMake and Visual Studio. Lastly,
this project is developed using [Visual Studio Code](https://code.visualstudio.com/) as
the editor of choice. Any editor will work, but this is what I recommend.

You will also find `pwshbuild.conf` within the root project directory. This contains
the configuration that I use with [pwshbuild](https://github.com/0xDATAWOLF/pwshbuild).
If you plan to use your own build scripts/tools, or wish to manually configure & build
CMake through the CLI (as most people tend to do), then feel free to ignore it.

### Note on Documentation

This project isn't anything more than a public project of mine, serving as future
point of reference should I ever need it. Therefore, I will not provide any working
documentation intended for the end user. While I may maintain some documentation,
it is purely for my own reference and shouldn't be taken as reliable source of
information regarding the Win32 API or DX12. I recommend [Frank D. Luna's Introduction
to 3D Game Programming with DirectX 12](https://www.amazon.com/Introduction-3D-Game-Programming-DirectX/dp/1942270062)
as good starting point for DX12. This repo is based primarily on this book and information
pulled from Microsoft's own documentation.

# Documentation

### Pre-Initialization Stage of DX12

Before we can even begin initializing DirectX 12, we will need to perform some pre-initialization
work with Windows. The process is relatively simple but there is a lot going on that goes unsaid
throughout the process. [Microsoft's documentation](https://docs.microsoft.com) is a great resource
to look deeper into the underlying functions and methodology and will be a great place to go if
anything below seems a bit obtuse. What we are effectively trying to accomplish in the pre-initialization
stage is creating a [Win32 Window](https://docs.microsoft.com/en-us/windows/win32/learnwin32/creating-a-window)
and defining a simple application loop that takes care of the required housekeeping needed to run a Win32 application.
Fortunately, the code isn't particularly dense when trimmed down to the absolute necessary components and
therefore doesn't require much prerequisite knowledge but I still highly recommend taking a look at Microsoft's
documentation for a complete crash course in Win32 applications.

Below, you will find a series of code snippets. It is assumed you are familiar with C++ enough to understand what
is going on. Anywhere you find an elipsis "..." in the code, it is meant to represent pieces of code that proceeds
or preceeds what is shown. Any snippet contained within a class, function, or class method will contain
the corresponding header such that you know where in the code I am referring to.

To begin, we will need to define some functions:

```C++
#include <windows.h>
#include <assert.h>

// Define your WindowProcedure here.
HRESULT CALLBACK WindowProcedure(HWND windowInstance, UINT message, WPARAM wParam, LPARAM lParam) { ... };

// Win32 Application entry point definition.
INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPreviousInstance, PWSTR commandline, int commandShow) { ... };
```

Somewhere in `wWinMain`, create your window class and register it.

```C++
INT WINAPI wWinMain(...)
{
...
WNDCLASS windowClass = {};
windowClass.lpfnWndProc = WindowProcedure;
windowClass.hInstance = hInstance; // Refer to the function parameters of wWinMain
windowClass.lpszClassName = L"dxtwelve Window Class";
assert(RegisterClass(&windowClass));
```

It may be useful to define a known client area viewport for DX12. For this example, we are creating a
1280x720p viewport. The reason we can't outright specify these dimensions through `CreateWindow` is because
the dimensions that you pass into that function define the entire window's size, which may or may not include
the frame, border, menus, and other styling things that take up room. Therefore, you need to call some helper
functions to create a `RECT` structure that is the exact fit to contain the style and desired client area.

```C++
INT WINAPI wWinMain(...)
{
...
RECT windowRectangle = {};
int clientWidth = 1280;
int clientHeight = 720;
SetRect(&windowRectangle, 0, 0, clientWidth, clientHeight);
AdjustWindowRect(&windowRectangle, WS_OVERLAPPEDWINDOW, FALSE);

// The useable window dimensions for CreateWindow(...).
int windowWidth = windowRectangle.right - windowRectangle.left;
int windowHeight = windowRectangle.bottom - windowRectangle.top;
```

Creating the window is a rather trivial process. Ensure that you get a valid window instance back
and then show the window once you confirm that Windows has successfully created and allocated a window
for you.

```C++
INT WINAPI wWinMain(...)
{
...
HWND windowInstance = CreateWindow(L"dxtwelve Window Class", L"dxtwelve",
	WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT
	windowWidth, windowHeight, NULL, NULL, hInstance, NULL);
assert(windowInstance);
ShowWindow(windowInstance, commandShow); // Refer to the function parameters of wWinMain
```

You will need some runtime loop to keep the application running. For the sake of simplicity,
I will demonstrate a global fetcher function that contains the app state. It's no different
than defining a global variable, but it wraps the global variable inside a function and makes
it much more explicit as to the intention of what we're doing. You may opt to do something
else or build your application within a class, but nonetheless we will need some way of
cross-communicating between `wWinMain` and the `WindowProcedure` functions. If you chose the
class route, you can simply specificify `WndProc` to be the pointer to a class method.

```C++
struct app_state
{
	bool isRunning;
}

static inline app_state& getAppState() { static app_state = {}; return app_state; }
```

```C++
INT WINAPI wWinMain(...)
{
...
getAppState().isRunning = true;
while (getAppState().isRunning) {}
```

Even though we have a loop to keep the application running, it fails to do two very
important things: respond to important window related events and process the message
queue that Windows expects us to handle. A consequence of this unresponsive behavior
is that Windows will detect it as unresponsive. The best case is that the close button
will not work, and the worst case is Windows flagging it as "unresponsive" and prompt
the user to kill it. Actually, the worst case is the best case because a window you can
not close is a problem for the user. Unless you want to go into task manager every time you
want to close your app, you will need some way of processing window events and the
message queue.

A small sidebar regarding message queues: the reason we create a "global variable"
to control the application runtime loop is actually quite simple: the standard method
for controlling a Win32 runtime loop is terrible for running realtime applications.
The `while(getAppState().isRunning)` would instead be `while(GetMessage(...) > 0)`.
This is fine for very simple applications where the behavior is entirely governed by
the response to events, but not for an application that continuously updates dynamically
multiple times per second. We will instead use `PeekMessage()` within our runtime
loop to dispatch our messages and respond to window events as they come in on a per-frame
basis.

```C++
INT WINAPI wWinMain(...)
{
...
while (getAppState().isRunning)
{
...
MSG currentMessage;
while (PeekMessageW(&currentMessage, windowInstance, NULL, NULL, PM_REMOVE))
{
	if (currentMessage.message == WM_QUIT) { getAppState().isRunning = false; return 0; }
	TranslateMessage(&currentMessage);
	DispatchMessage(&currentMessage);
}
```

```C++
HRESULT CALLBACK WindowProcedure(...)
{
...
switch(message)
{
	case WM_DESTROY:
	{
		// WM_DESTROY indicates that the window is being destroyed, but we will treat it as a request to quit.
		PostQuitMessage(0);
		break;
	}

	case WM_CLOSE:
	{
		// WM_CLOSE indicates that the application should terminate.
		getAppState().isRunning = false; // This is why we defined the global.
		break;
	}

	case WM_PAINT:
	{
		// WM_PAINT is called when the client area of the window changes and we need to update the
		// canvas with a fresh coat of paint.
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

return DefWindowProc(windowInstance, message, wParam, lParam); // For everything else, default.
```


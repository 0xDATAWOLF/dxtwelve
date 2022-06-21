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

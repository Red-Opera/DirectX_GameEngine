#pragma once

#define EXCEPT(hr) Window::HRException(__LINE__, __FILE__, hr)
#define LASTEXCEPT() Window::HRException(__LINE__, __FILE__, GetLastError())
#define NOGRAPHICEXEPT() Window::NoGraphicException(__LINE__, __FILE__)
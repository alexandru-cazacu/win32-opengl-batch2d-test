#include "stdio.h"
#include <windows.h>

#include "resources.h"

void __stdcall WinMainCRTStartup()
{
    HICON hIcon1 = LoadIcon(NULL, MAKEINTRESOURCE(101));
    HMODULE hModule = GetModuleHandle(0);
    HDC hdc = GetDC(NULL);
    
    DrawIcon(hdc, 10, 20, hIcon1);
    
    MessageBoxA(NULL, "Ciao", "TextNoCRT", MB_ICONSTOP);
    
    ExitProcess(0);
}

void main() {

}
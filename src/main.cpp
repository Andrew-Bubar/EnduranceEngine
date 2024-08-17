
// Platform Globals
static bool running = true;
#include <iostream>
#include "endurance_lib.h"

// Platform Fucntions
bool platform_create_window(int width, int height, char* title);
void platform_update_window();

// Windows platform
#ifdef _WIN32
//includes
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

// globals
static HWND window;

//platform implementations
LRESULT CALLBACK windows_window_callback(HWND window, UINT msg, 
                                     WPARAM wparam, LPARAM lparam){
        
    LRESULT result = 0;

    switch(msg){
        case WM_CLOSE:{
            running = false;
            break;
        }

        default: {
            result = DefWindowProcA(window, msg, wparam, lparam);
        }
    }
    return result;
}
bool platform_create_window(int width, int height, char* title){

    HINSTANCE instance = GetModuleHandleA(0);

    WNDCLASSA wc = {};
    wc.hInstance = instance;
    wc.hIcon = LoadIcon(instance, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = title; //not titling the window
    wc.lpfnWndProc = windows_window_callback;

    if(!RegisterClassA(&wc)){ return false; }

    //for styling the window
    int dwStyle = WS_OVERLAPPEDWINDOW;

    window = CreateWindowExA(
        0,
        title, //class name
        title, //title for window
        dwStyle,
        100,
        100,
        width,
        height,
        NULL,
        NULL,
        instance,
        NULL
    );

    if(window == NULL){ return false; }

    ShowWindow(window, SW_SHOW);
    return true;
}

void platform_update_window(){

    MSG msg;

    while(PeekMessageA(&msg, window, 0, 0, PM_REMOVE)){
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

#endif

int main(){

    platform_create_window(1280, 720, "The game I WILL MAKE");

    while(running){
        platform_update_window();
    }

    return 0;
}

// my file for windows

//connecting this to the engine
#include "platform.h"
#include "endurance_lib.h"

//includes
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include "wglext.h"
#include <glcoreab.h>

// globals
static HWND window;
static HDC dc;

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
bool platform_create_window(int width, int height, char* title){ //making the window

    HINSTANCE instance = GetModuleHandleA(0);

    WNDCLASSA wc = {}; //making the window class and modifying it
    wc.hInstance = instance;
    wc.hIcon = LoadIcon(instance, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = title; //not titling the window
    wc.lpfnWndProc = windows_window_callback;

    if(!RegisterClassA(&wc)){ return false; } 

    //for styling the window
    int dwStyle = WS_OVERLAPPEDWINDOW;

    //openGL functions
    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB =  nullptr;

    { //all openGL initialization 
        window = CreateWindowExA( //making the fake window
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

        if(window == NULL){ SM_ASSERT(false, "failed to make a windows window"); return false; }

        //open gl window
        HDC fakeDC = GetDC(window);
        if(!fakeDC){ SM_ASSERT(false, "failed to get HDC"); return false; }

        PIXELFORMATDESCRIPTOR pfd = {0}; //setting the format for the pixels
        pfd.nSize = sizeof(pfd);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; //telling it how 
                                                                                //to handle pixels
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32; //color settings
        pfd.cAlphaBits = 8;
        pfd.cDepthBits = 24;

        int pixelFormat = ChoosePixelFormat(fakeDC, &pfd); //the actual pixel format
        if(!pixelFormat){SM_ASSERT(false, "Failed to make the pixel format"); return false; }

        //applying the pixel format once
        if(!SetPixelFormat(fakeDC, pixelFormat, &pfd)){ SM_ASSERT(false, "failed to set pixel format"); return false; }

        HGLRC fakeRC = wglCreateContext(fakeDC); //making the fake rendoring conetext (rc)
        if(!fakeRC){ SM_ASSERT(false, "failed to make fake rendoring context"); return false; }
        
        //making all of the above the current context
        if(!wglMakeCurrent(fakeDC, fakeRC)){ SM_ASSERT(false, "failed to make current context"); return false; }

        //loading in some openGL functions
        wglChoosePixelFormatARB = 
            (PFNWGLCHOOSEPIXELFORMATARBPROC)platform_load_gl_function("wglChoosePixelFormatARB");
        wglCreateContextAttribsARB =
            (PFNWGLCREATECONTEXTATTRIBSARBPROC)platform_load_gl_function("wglCreateContextAttribsARB");
        if(!wglChoosePixelFormatARB || !wglCreateContextAttribsARB){ SM_ASSERT(false, "didnt load GL functions"); return false; }

        //clean up and ready to make real window
        wglMakeCurrent(fakeDC, 0);
        wglDeleteContext(fakeRC);
        ReleaseDC(window, fakeDC);
        DestroyWindow(window);
    }

    { //acutal openGL initalization!
        { //adding in border size
            RECT borderRect = {};
            AdjustWindowRectEx(&borderRect, dwStyle, 0, 0);

            width += borderRect.right - borderRect.left;
            height += borderRect.bottom - borderRect.top;
        }

            window = CreateWindowExA( //making the real window
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
        if(window == NULL){ SM_ASSERT(false, "failed to make a windows window"); return false; }

        //device context
        dc = GetDC(window);
        if(!dc){ SM_ASSERT(false, "failed to get HDC"); return false; }

        const int pixelAttribs[] = //all of the settings for drawing to the window with openGL
        {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
            WGL_SWAP_METHOD_ARB,    WGL_SWAP_COPY_ARB,
            WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
            WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
            WGL_COLOR_BITS_ARB,     32,
            WGL_ALPHA_BITS_ARB,     8,
            WGL_DEPTH_BITS_ARB,     24,
            0 //always close with 0 for open gl
        };

        UINT numPixelFormats;
        int pixelFormat = 0;
        if(!wglChoosePixelFormatARB( 
            dc,
            pixelAttribs,
            0,
            1,
            &pixelFormat,
            &numPixelFormats
        )){ SM_ASSERT(0, "failed to wglChoosePixelFormatARB"); return false; }

        PIXELFORMATDESCRIPTOR pfd = {0};
        DescribePixelFormat(dc, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd); //making pixel format based on DC
        //setting the pixel format
        if(!SetPixelFormat(dc, pixelFormat, &pfd)){ SM_ASSERT(0, "failed to set pixel format"); return false; }

        const int contextAttribs[] ={ //setting up the current context
            WGL_CONTEXT_MAJOR_VERSION_ARB, 4, //what version
            WGL_CONTEXT_MINOR_VERSION_ARB, 3,
            WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB, //set up the core
            WGL_CONTEXT_FLAGS_ARB,         WGL_CONTEXT_DEBUG_BIT_ARB,  //debug settings for when open gl fails
            0 //terminate array
        };

        HGLRC rc = wglCreateContextAttribsARB(dc, 0, contextAttribs); //make the rendering context
        if(!rc){ SM_ASSERT(0, "failed to get rendering context"); return false; }

        //make rc the primary rendering context
        if(!wglMakeCurrent(dc, rc)){ SM_ASSERT(0, "failed to wglMakeCurrent"); return false; }
    }

    ShowWindow(window, SW_SHOW);
    return true;
}

void platform_update_window(){ //handling windows inputs on the window

    MSG msg;

    while(PeekMessageA(&msg, window, 0, 0, PM_REMOVE)){
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

void* platform_load_gl_function(char* function){

    PROC proc = wglGetProcAddress(function);

    if (!proc){ //if the proc isnt found yet

        static HMODULE openglDLL = LoadLibraryA("opengl32.dll"); //look for the .dll file
        proc = GetProcAddress(openglDLL, function); //then try to find the function from the dll

        //if no proc ever found
        if(!proc){ SM_ASSERT(false, "failed to load gl function: %s", "glCreateProgram"); return nullptr;}
    }
    return (void*)proc; //return the found proc
}

void platform_swap_buffers(){
    SwapBuffers(dc);
}
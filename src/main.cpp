
#include "platform.h"
#include "endurance_lib.h"
#include "input.h"

#define APIENTRY
#define GL_GLEXT_PROTOTYPES
#include "glcoreab.h"

// Windows platform
#ifdef _WIN32
#include "win32_platform.cpp" //all my windows functions
#endif

//includes after handling what OS i'm on
#include "gl_renderer.cpp"

int main(){

    BumpAllocator ts = make_bump_allocator(MB(50));

    input.screenSizeX = 1280;
    input.screenSizeY = 720;

    platform_create_window(input.screenSizeX, input.screenSizeY, "The game I WILL MAKE");
    gl_init(&ts);

    while(running){
        platform_update_window();
        gl_render();

        platform_swap_buffers();
    }

    return 0;
}
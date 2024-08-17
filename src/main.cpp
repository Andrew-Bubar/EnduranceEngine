
#include "platform.h"

// Windows platform
#ifdef _WIN32
#include "win32_platform.cpp" //all my windows functions
#endif

int main(){

    platform_create_window(1280, 720, "The game I WILL MAKE");

    while(running){
        platform_update_window();
    }

    return 0;
}
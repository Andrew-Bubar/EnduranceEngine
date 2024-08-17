#pragma once
//My file for things regardless of OS

// Platform Globals
static bool running = true;
#include <iostream>
#include "endurance_lib.h"

// Platform Fucntions
bool platform_create_window(int width, int height, char* title);
void platform_update_window();
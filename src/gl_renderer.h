#pragma once

#include "glcoreab.h"
#include "endurance_lib.h"

// Open GL static Function pointers
static PFNGLCREATEPROGRAMPROC glCreateProgram_ptr;

void gl_load_function(){

    PROC proc = wglGetProcAddress("glCreateProgram");
    if (!proc){ SM_ASSERT(false, "failed to load gl function: %s", "glCreateProgram"); }

    glCreateProgram_ptr = (PFNGLCREATEPROGRAMPROC)proc;
}

GLAPI GLint APIENTRY glCreateProgram(void){ //sending the pointer to a varrible
    return glCreateProgram_ptr();
}
#include "gl_renderer.h"

// OpenGL Structs
struct GLContext{
    GLuint programID;
};


// OpenGL Globals
static GLContext glContext;

// OpenGL Functions
static void gl_debug_callback( //making a function to send openGL messages into my logging system
    GLenum source, GLenum type, GLuint id, 
    GLenum severity, GLsizei length, const GLchar* message, const void* user){

        if( //seeing if the message is an error
            severity == GL_DEBUG_SEVERITY_HIGH   ||
            severity == GL_DEBUG_SEVERITY_MEDIUM ||
            severity == GL_DEBUG_SEVERITY_LOW
        ){ SM_ASSERT(0, "OpenGL error: %s", message); }
        else{ SM_TRACE((char*)message); } //otherwise just trace it
    }

bool gl_init(BumpAllocator* transientStorage){ //start-up openGL
    
    load_gl_functions(); //get the functions for openGL

    glDebugMessageCallback(&gl_debug_callback, nullptr); //adding the debug callback

    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); //enabling debug output
    glEnable(GL_DEBUG_OUTPUT);

    GLuint vertShaderID = glCreateShader(GL_VERTEX_SHADER); //making shaders
    GLuint fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    
    int fileSize = 0; //loading in the vert shader
    char* vertShader = read_file("assets/shaders/quad.vert", &fileSize, transientStorage);

    int fileSize2 = 0; //loading in the frag shader
    char* fragShader = read_file("assets/shaders/quad.frag", &fileSize, transientStorage);
    
    if(!vertShader || !fragShader){ SM_ASSERT(0, "Couldnt load shaders"); return false; }

    glShaderSource(vertShaderID, 1, &vertShader, 0); //adding the shaders to openGL after
    glShaderSource(fragShaderID, 1, &fragShader, 0); //loading them in

    glCompileShader(vertShaderID); //compiling the shaders
    glCompileShader(fragShaderID);

    //testing the vert shader
    {
        int works;
        char shaderLog[2048] = {};

        glGetShaderiv(vertShaderID, GL_COMPILE_STATUS, &works);
        if(!works){ 
            glGetShaderInfoLog(vertShaderID, 2048, 0, shaderLog); 
            SM_ASSERT(0, "failed to compile vert shader: %s", shaderLog); 
        }
    }
    //testing the frag shader
    {
        int works;
        char shaderLog[2048] = {};

        glGetShaderiv(fragShaderID, GL_COMPILE_STATUS, &works);
        if(!works){ 
            glGetShaderInfoLog(fragShaderID, 2048, 0, shaderLog); 
            SM_ASSERT(0, "failed to compile vert shader: %s", shaderLog); 
        }
    }

    glContext.programID = glCreateProgram(); //making the program

    glAttachShader(glContext.programID, vertShaderID); // adding the shaders
    glAttachShader(glContext.programID, fragShaderID);

    glLinkProgram(glContext.programID); //linking everything to the program

    glDetachShader(glContext.programID, vertShaderID); //after they belong in program, unload
    glDetachShader(glContext.programID, fragShaderID);
    glDeleteShader(vertShaderID); //after unloading, delete them from memeory
    glDeleteShader(fragShaderID);

    GLuint VAO; //giving openGL vertext Arrays cause it needs them to run :(
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glEnable(GL_DEPTH_TEST); //Depth texting stuff
    glDepthFunc(GL_GREATER);

    glUseProgram(glContext.programID); //use the program I've made

    return true;
}

void gl_render(){

    // color for when nothing draw
    glClearColor(119.0f / 255.0f, 33.0f / 225.0f, 111.0f / 255.0f, 1.0f);
    glClearDepth(0.0f); //reset depth
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear the window

    glViewport(0, 0, input.screenSizeX, input.screenSizeY); //telling openGL where to draw

    glDrawArrays(GL_TRIANGLES, 0, 6); //draw the array from shaders
}
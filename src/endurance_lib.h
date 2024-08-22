#pragma once

#include <stdio.h> //for printing text

#include <stdlib.h> //for malloc
#include <string.h> //for memset
#include <sys/stat.h> //to get/edit timestamp data

// Defines
#ifdef _WIN32 //for system specific debugging
#define DEBUG_BREAK() __debugbreak()
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
#elif __linux__
#define DEBUG_BREAK() __builtin_debugtrap()
#elif __APPLE__
#define DEBUG_BREAK() __builtin_trap()
#endif

#define BIT(x) 1 << (x) //helps shift data on a bite
#define KB(x) ((unsigned long long)1024 * x) //defining what different storage sizes are to better sort memory
#define MB(x) ((unsigned long long)1024 * KB(x))
#define GB(x) ((unsigned long long)1024 * MB(x))

// Logging

enum TextColor{
  TEXT_COLOR_BLACK,
  TEXT_COLOR_RED,
  TEXT_COLOR_GREEN,
  TEXT_COLOR_YELLOW,
  TEXT_COLOR_BLUE,
  TEXT_COLOR_MAGENTA,
  TEXT_COLOR_CYAN,
  TEXT_COLOR_WHITE,
  TEXT_COLOR_BRIGHT_BLACK,
  TEXT_COLOR_BRIGHT_RED,
  TEXT_COLOR_BRIGHT_GREEN,
  TEXT_COLOR_BRIGHT_YELLOW,
  TEXT_COLOR_BRIGHT_BLUE,
  TEXT_COLOR_BRIGHT_MAGENTA,
  TEXT_COLOR_BRIGHT_CYAN,
  TEXT_COLOR_BRIGHT_WHITE,
  TEXT_COLOR_COUNT
};

template <typename ...Args>
void _log(char* prefix, char* msg, TextColor textColor, Args... args){ // formatting for messages to the log

    static char* TextColorTable[TEXT_COLOR_COUNT] = {
    "\x1b[30m", // TEXT_COLOR_BLACK
    "\x1b[31m", // TEXT_COLOR_RED
    "\x1b[32m", // TEXT_COLOR_GREEN
    "\x1b[33m", // TEXT_COLOR_YELLOW
    "\x1b[34m", // TEXT_COLOR_BLUE
    "\x1b[35m", // TEXT_COLOR_MAGENTA
    "\x1b[36m", // TEXT_COLOR_CYAN
    "\x1b[37m", // TEXT_COLOR_WHITE
    "\x1b[90m", // TEXT_COLOR_BRIGHT_BLACK
    "\x1b[91m", // TEXT_COLOR_BRIGHT_RED
    "\x1b[92m", // TEXT_COLOR_BRIGHT_GREEN
    "\x1b[93m", // TEXT_COLOR_BRIGHT_YELLOW
    "\x1b[94m", // TEXT_COLOR_BRIGHT_BLUE
    "\x1b[95m", // TEXT_COLOR_BRIGHT_MAGENTA
    "\x1b[96m", // TEXT_COLOR_BRIGHT_CYAN
    "\x1b[97m", // TEXT_COLOR_BRIGHT_WHITE
    };

    char formatBuffer[8192] = {};
    sprintf(formatBuffer, "%s %s %s \033[0m", TextColorTable[textColor], prefix, msg);

    char textBuffer[8192] = {};
    sprintf(textBuffer, formatBuffer, args...);

    puts(textBuffer);
}

// different ways to send a message to the log
#define SM_TRACE(msg, ...) _log("TRACE: ", msg, TEXT_COLOR_GREEN, ##__VA_ARGS__);
#define SM_WARN(msg, ...) _log("WARN: ", msg, TEXT_COLOR_YELLOW, ##__VA_ARGS__);
#define SM_ERROR(msg, ...) _log("ERROR: ", msg, TEXT_COLOR_RED, ##__VA_ARGS__);

// a debug code that pauses the game completely
#define SM_ASSERT(x, msg, ...){                                 \
  if(!(x)){                                                     \
    SM_ERROR(msg, ##__VA_ARGS__);                               \
    DEBUG_BREAK();                                              \
    SM_ERROR("Assert Hit!");                                    \
  }                                                             \
}                                                               \


// Bump Allocator

struct BumpAllocator{ // the bump allocator for data movement / management
  size_t capacity;
  size_t used;
  char* memory;
};

BumpAllocator make_bump_allocator(size_t size){ // making a bump allocator with x size
  BumpAllocator ba = {};
  
  ba.memory = (char*)malloc(size);
  if(ba.memory){
    ba.capacity = size;
    memset(ba.memory, 0, size); //set memory to 0
  } else { SM_ASSERT(false, "Never allocated Memory"); }

  return ba;
}

char* bump_alloc(BumpAllocator* ba, size_t size){ //putting data in the bump allocator
  char* result = nullptr;

  size_t allignedSize = (size+7) & ~ 7; //makes the first 4 bits to 0

  if(ba->used + allignedSize < ba->capacity){ //making sure there is room
    result = ba->memory + ba->used;
    ba->used += allignedSize;
  }else {
    SM_ASSERT(false, "No room in Bump Allocator");
  }

  return result;
}

// File IO
long long get_timestamp(char* file){ //when was the file made
                                     //can be used for "hot fixing" the game later

  struct stat file_stat = {};
  stat(file, &file_stat); //getting file stats
  return file_stat.st_mtime;
}

bool file_exists(char* filePath){ // is there a file?
  SM_ASSERT(filePath, "File has no path");

  auto file = fopen(filePath, "rb");

  if(!file){
    return false;
  }
  fclose(file);

  return true;
}

long get_file_size(char* filePath){ //see how big the file is
  SM_ASSERT(filePath, "there is no file");

  long fileSize = 0;
  auto file = fopen(filePath, "rb");
  if(!file){ SM_ERROR("Failed to open file: %s", filePath); return 0; }

  fseek(file, 0, SEEK_END); //seeing size of file
  fileSize = ftell(file); //writting down file size
  fseek(file, 0, SEEK_SET); //resetting the file

  fclose(file);
  return fileSize;
}

char* read_file(char* filePath, int* fileSize, char* buffer){ //read a file without an allocator

  SM_ASSERT(filePath, "No file Path");
  SM_ASSERT(fileSize, "No file Size");
  SM_ASSERT(buffer, "No buffer");

  *fileSize = 0;
  auto file = fopen(filePath, "rb"); //opening the file
  if(!file){ SM_ERROR("Failed to open file: %s", filePath); return nullptr; }

  fseek(file, 0, SEEK_END); //seeing how big the file is
  *fileSize = ftell(file); //writing down the size
  fseek(file, 0, SEEK_SET); //resetiing the file

  memset(buffer, 0, *fileSize + 1); //setting the memory of the buffer
  fread(buffer, sizeof(char), *fileSize, file); //reading the data to the buffer

  fclose(file);
  return buffer; //returning the data via the buffer
}

char* read_file(char* filePath, int* fileSize, BumpAllocator* ba){ //read a file with an allocator

  char* file = nullptr; //making a varrible to put a file into
  long fileSize2 = get_file_size(filePath); //reading the file size from the path

  if(fileSize2){ //if there was a file found
    char* buffer = bump_alloc(ba, fileSize2 + 1); //make a buffer
    file = read_file(filePath, fileSize, buffer); //make the file equal to the file found with the buffer made
  }

  return file; // return the file found
}

void write_file(char* filePath, char* buffer, int size){ //for writting bufferes to files
  
  //check if the data put in is legit
  SM_ASSERT(filePath, "No file provided");
  SM_ASSERT(buffer, "no Buffer provided");

  auto file = fopen(filePath, "wb");

  if(!file){ SM_ERROR("failed to open file: %s", filePath); return; } //check if a file was opened

  fwrite(buffer, sizeof(char), size, file); //writing to the file
  fclose(file); //close the file
}

bool copy_file(char* fileName, char* outputName, char* buffer){ //to move data from file to output

  int fileSize = 0; 
  char* data = read_file(fileName, &fileSize, buffer); //getting the data to be copied

  auto outputFile = fopen(outputName, "wb"); //opening the file we are copying too
  if(!outputFile){ SM_ERROR("Failed to open file: %s", outputName); return false; }

  int result = fwrite(data, sizeof(char), fileSize, outputFile); //writting the data to the output
  if(!result){ SM_ERROR("failed to open file: %s", outputName); return false; }

  fclose(outputFile); //close file after done with it
  return true; // tell the function that copying worked
}

bool copy_file(char* fileName, char* outputName, BumpAllocator* ba){
  
  char* file = 0; //file I'm not confident, maybe be left over from copy/paste

  long fileSize2 = get_file_size(fileName); //getting data size
  if(fileSize2){ //if data exists

    char* buffer = bump_alloc(ba, fileSize2 + 1); //make a buffer with the Bump Allocator
    return copy_file(fileName, outputName, buffer); //use the other copy file with all these values
  }

  return false; // else return false
}

// heckin nerd math stuff
struct vector2 {
  float x;
  float y;
};
struct ivector2 {
  int x;
  int y;
};

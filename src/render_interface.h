#pragma once
#include "endurance_lib.h"
#include "assets.h"

// Constants
constexpr int MAX_TRANSFORMS = 1000;

// Structs
struct transform { //a basic transform varrible for right now to use for sprite / object rendering
    ivector2 atlasOffset;
    ivector2 spriteSize;
    vector2 position;
    vector2 size;
};

struct renderData {
    int transformCount;
    transform transform[MAX_TRANSFORMS];
};

// Global
static renderData rd;

// Functions
void draw_sprite(spriteID spriteID, vector2 position, vector2 size){
    sprite s = get_sprite(spriteID); //getting the sprite from ID

    transform trans = {}; //making an empty transform to fill from the sprite
    trans.position = position;
    trans.size = size;
    trans.atlasOffset = s.atlasOffset;
    trans.spriteSize = s.spriteSize;

    rd.transform[rd.transformCount++] = trans; //adding the transform to the render data
}
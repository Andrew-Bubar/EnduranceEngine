#pragma once
#include "endurance_lib.h"

// constants

// structs
enum spriteID{ // different tags for sprites from the 
    SPRITE_PLAYER,
    SPRITE_COIN,

    SPRITE_COUNT
};
struct sprite{
    ivector2 atlasOffset;
    ivector2 spriteSize;
};

// globals
sprite get_sprite(spriteID spriteID){ //assigning sprites to different ID's and returning them
    sprite x = {};

    switch (spriteID)
    {
    case SPRITE_COIN:
        x.atlasOffset= {0, 17};
        x.spriteSize = {16, 16};
        break;
    
    case SPRITE_PLAYER:
        x.atlasOffset = {0, 204};
        x.spriteSize = {16, 16};

    default:
        break;
    }

    return x;
}
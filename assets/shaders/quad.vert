#version 430 core

// Structs
struct transform { //a basic transform varrible for right now to use for sprite / object rendering
    ivec2 atlasOffset;
    ivec2 spriteSize;
    vec2 position;
    vec2 size;
};

//In
layout (std430, binding = 0) buffer TransformSBO{
    transform transforms[];
};

uniform vec2 screenSize;

//Out
layout (location = 0) out vec2 textureCoordsOut;

void main()
{
    transform trans = transforms[gl_InstanceID]; // getting the transform call
    

  vec2 vertices[6] = //plugging in the size / position for the transform
  {
    trans.position,                                        // Top Left
    vec2(trans.position + vec2(0.0, trans.size.y)),    // Bottom Left
    vec2(trans.position + vec2(trans.size.x, 0.0)),    // Top Right
    vec2(trans.position + vec2(trans.size.x, 0.0)),    // Top Right
    vec2(trans.position + vec2(0.0, trans.size.y)),    // Bottom Left
    trans.position + trans.size                        // Bottom Right
  };

    //for getting a texture off of the tile sheet
    float left = trans.atlasOffset.x; //plugging in the numbers from the transforms
    float top = trans.atlasOffset.y;
    float right = trans.atlasOffset.x + trans.spriteSize.x;
    float bottom = trans.atlasOffset.y + trans.spriteSize.y;

    vec2 textCoords[6] = {
        vec2(top, left),
        vec2(bottom, left),
        vec2(top, right),
        vec2(top, right),
        vec2(bottom, left),
        vec2(bottom, right)
    };

    { //normalize the size
      vec2 vertexPosition = vertices[gl_VertexID];
      vertexPosition.y = -vertexPosition.y + screenSize.y;
      vertexPosition = 2.0 * (vertexPosition / screenSize) - 1.0;
      gl_Position = vec4(vertexPosition, 0.0, 1.0);
    }

    textureCoordsOut = textCoords[gl_VertexID];
}
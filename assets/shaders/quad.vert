#version 430 core

//In

//Out
layout(location = 0) out vec2 textureCoordsOut;

void main()
{

    vec2 vertices[6] = {

        //Top left
        vec2(-0.5, 0.5),

        //bottom left
        vec2(-0.5, -0.5),

        //top right
        vec2( 0.5, 0.5),
        //top right
        vec2( 0.5, 0.5),

        //bottom left
        vec2(-0.5, -0.5),

        //bottom right
        vec2( 0.5, -0.5)

        //put in all the sides (including top and bottom) here
    };

    //for getting a texture off of the tile sheet
    float left = 17.0;
    float top = 0.0;
    float right = 33.0;
    float bottom = 17.0;

    vec2 textCoords[6] = {
        vec2(top, left),
        vec2(bottom, left),
        vec2(top, right),
        vec2(top, right),
        vec2(bottom, left),
        vec2(bottom, right)
    };

    gl_Position = vec4(vertices[gl_VertexID], 1.0, 1.0);
    textureCoordsOut = textCoords[gl_VertexID];
}
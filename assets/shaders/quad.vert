#version 430 core

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

    gl_Position = vec4(vertices[gl_VertexID], 1.0, 1.0);
}
#version 430 core

//Input
layout(location = 0) in vec2 textureCoordsIn;

//Output (think audio)
layout (location = 0) out vec4 fragColor;

// Bindings
layout (location = 0) uniform sampler2D textureAtlas;

void main(){
    
    vec4 textureColor = texelFetch(textureAtlas, ivec2(textureCoordsIn), 0); //get texture color
    if(textureColor.a == 0){discard;}
    fragColor = textureColor;
}
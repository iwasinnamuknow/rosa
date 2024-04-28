#version 420 core

layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec4 inColor;
layout (location = 3) in float inTexture;

uniform mat4 mvp;

out vec4 passColor;
out vec2 UV;
out float texture;

void main()
{
    gl_Position = mvp * vec4(inPosition, 0.0, 1.0);
    passColor = inColor;
    UV = inUV;
    texture = inTexture;
}
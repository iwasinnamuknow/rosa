#version 420 core

in vec4 passColor;
in vec2 UV;
in float texture;

out vec4 color;

layout(binding=0) uniform sampler2D textureSamplers[32];

void main()
{
    int index = int(texture);
    color = texture2D(textureSamplers[index], UV).rgba * passColor;
}
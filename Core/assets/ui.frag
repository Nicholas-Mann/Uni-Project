#version 430

in vec4 v2fColor;

layout( location = 0 ) out vec4 oColor;

layout( binding = 0 ) uniform sampler2D uTexture;

void main()
{
    oColor = v2fColor;
}

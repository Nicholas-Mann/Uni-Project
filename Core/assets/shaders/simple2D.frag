#version 430

in vec4 v2fColor;

layout( location = 0 ) out vec4 oColor;

void main()
{
    oColor = v2fColor;
}

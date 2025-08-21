#version 430

layout( location = 0 ) in vec2 iPosition;
layout( location = 1 ) in vec4 iColor;

out vec4 v2fColor;

void main()
{
    v2fColor = iColor;
    gl_Position =  vec4( iPosition.xy, 0.0, 1.0 );
}
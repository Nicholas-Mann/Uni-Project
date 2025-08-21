#version 430

layout( location = 0 ) in vec3 iPosition;
layout( location = 1 ) in vec2 iTexCoord;
layout( location = 2 ) in vec4 iColor;

layout( location = 0 ) uniform mat4 uProjCameraWorld;

out vec4 v2fColor;
out vec2 v2fTexCoord;

void main()
{
	v2fTexCoord = iTexCoord;
	v2fColor = iColor;
	gl_Position =  uProjCameraWorld * vec4( iPosition, 1.0 );
}

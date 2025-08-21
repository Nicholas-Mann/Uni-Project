#version 430

layout( location = 0 ) in vec3 iPosition;
layout( location = 1 ) in vec3 iColor;
layout( location = 2 ) in vec3 iNormal;

uniform mat4 uProjection;
uniform mat4 uCameraView;
uniform mat4 uModel;
uniform mat3 uNormalMatrix;

out vec3 v2fColor;
out vec3 v2fNormal;
out vec3 FragPos;

void main()
{
    FragPos = vec3(uModel * vec4(iPosition, 1.0));
    v2fNormal = normalize(uNormalMatrix * iNormal);
    v2fColor = iColor;
    gl_Position = uProjection * uCameraView * uModel * vec4( iPosition, 1.0 );
}
#version 430

in vec2 v2fTexCoord;
in vec4 v2fColor;

layout( location = 0 ) out vec4 oColor;

layout( binding = 0 ) uniform sampler2D uTexture;

void main()
{
    // Load the texture color
    vec4 texColour = vec4(texture( uTexture, v2fTexCoord ).rgba);

    vec4 result = texColour * v2fColor;

    oColor = result;
}

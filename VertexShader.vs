#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;
out float Offset_out;

uniform float Offset;
uniform mat4 transform;

void main()
{
   gl_Position = transform * vec4(aPos, 1.0);
   ourColor = gl_Position.xyz;
   TexCoord = aTexCoord;
   Offset_out = Offset;
}
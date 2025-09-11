#version 330 core

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;
in float Offset_out;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float T_Percent;

vec2 B = vec2(-TexCoord.x, TexCoord.y);

void main()
{
   FragColor = mix(texture(texture1, TexCoord), texture(texture2, B), T_Percent);
   // texture(ourTexture, TexCoord) * vec4(ourColor.x, ourColor.y, ourColor.z + 0.5f, 1.0);
}
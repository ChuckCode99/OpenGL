#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float T_Percent;

void main()
{
   FragColor = mix(texture(texture1, TexCoord), \
				   texture(texture2, TexCoord), \
				   T_Percent);
   // texture(ourTexture, TexCoord) * vec4(ourColor.x, ourColor.y, ourColor.z + 0.5f, 1.0);
}
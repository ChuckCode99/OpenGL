#version 330 core

out vec4 FragColor;

in vec3 Normal; 
in vec3 FragPos;  
// in vec2 TexCoord;

// uniform sampler2D texture1;
// uniform sampler2D texture2;
// uniform float T_Percent;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;


void main()
{
	// ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256); // 32 -> 256
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);

   // FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), T_Percent);
   // texture(ourTexture, TexCoord) * vec4(ourColor.x, ourColor.y, ourColor.z + 0.5f, 1.0);
}
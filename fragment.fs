#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{    
//    FragColor = texture(texture_diffuse1, TexCoords)* vec4(0.5f,0.5f,0.5f,1.0f);
	FragColor = vec4(0.8f,0.8f,0.8f,1.0f);
}
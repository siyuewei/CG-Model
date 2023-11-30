#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    //ambient
    float ambient_coefficient = 0.1;
    vec3 ambient = lightColor * ambient_coefficient;
    vec3 normal = normalize(Normal);

    //diffuse
    float diffuse_coefficient = 1;
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 diffuse = lightColor * max(0,dot(normal,lightDir)) * diffuse_coefficient;

    //specular
    float specular_coefficient = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflect = reflect(-lightDir,normal);
    vec3 specular = lightColor * pow(max(0,dot(viewDir,reflect)),32) * specular_coefficient;

    vec3 objectColor = vec3(221.0/255.0f,160.0/255.0f,221.0/255.0f);
    FragColor = vec4((ambient + diffuse + specular) * objectColor, 1.0);
}
#version 330 core

out vec4 FragColor;

in vec4 vertexColor;
in vec3 normal;
in vec3 FragPos;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec3 cubeColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
//    FragColor = vertexColor;
//     FragColor = texture(texture0, coord) * vertexColor;
//     FragColor = mix(texture(texture1, coord), texture(texture2, coord), 0.2);
    float ambientStrength = 0.1;
    float specularStrength = 0.5;

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    float diff = dot(norm, lightDir);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 ambient = ambientStrength * lightColor;
    vec3 diffuse = diff * lightColor;
    vec3 tmpColor = (ambient + diffuse + specular) * cubeColor;
    FragColor = vec4(tmpColor, 1.0);

//     FragColor = texture(texture1, coord);
}
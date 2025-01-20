
#version 330 core
layout(location = 0) in vec3 position;


uniform mat4 model;      
uniform mat4 view;       
uniform mat4 projection; 
uniform float time;

void main() {
vec3 displacedPosition = position;
    displacedPosition.x += sin(time + position.z * 0.5) * 0.1;
    displacedPosition.z += cos(time + position.x * 0.5) * 0.1; 
    gl_Position = projection * view * model * vec4(displacedPosition, 1.0);
}
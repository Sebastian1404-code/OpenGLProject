
#version 330 core
out vec4 fragColor;


uniform float time;



void main() {

    float alpha = 0.8;
    float flicker = 0.8 + 0.2 * sin(time * 10.0);
    vec3 color=vec3(1.0f);
    fragColor = vec4(color*flicker, 0.5f);
}
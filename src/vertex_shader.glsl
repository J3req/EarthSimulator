#version 330 core
layout(location = 0) in vec2 aPos;    // Twoja pozycja
layout(location = 1) in vec3 aColor;  // Kolor cząstki

out vec3 fragColor; // przekazujemy do fragment shadera

void main()
{
    gl_Position = vec4(aPos, 0.0, 1.0);
    fragColor = aColor;
}
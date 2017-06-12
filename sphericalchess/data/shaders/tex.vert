#version 330

uniform mat4 viewproj;

layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec2 aCoord;

out vec2 uv;

void main()
{
    uv = aCoord;
    gl_Position = viewproj * vec4(aPosition, 1.0);
}
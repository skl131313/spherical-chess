#version 330

uniform mat4 viewproj;
uniform vec2 offset;

layout(location = 0) in vec2 aPosition;
layout(location = 2) in vec2 aCoord;

out vec2 uv;

void main()
{
    uv = aCoord;
    gl_Position = viewproj * vec4(offset + aPosition, 0.0, 1.0);
}
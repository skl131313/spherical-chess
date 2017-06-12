#version 330

uniform mat4 viewproj;

layout(location = 0) in vec3 aPosition;

void main()
{
    gl_Position = viewproj * vec4(aPosition, 1.0);
}
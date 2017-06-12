#version 330

uniform mat4 viewproj;
uniform mat4 world;
uniform vec3 eyePosition;
uniform mat4 camera;

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aCoord;

out vec3 varNormal;
out vec3 varEyeDir;
out vec2 varCoord;

void main()
{
    vec3 vertex = vec3(world * vec4(aPosition, 1.0));

    varEyeDir = normalize(eyePosition - vertex);
    varNormal = mat3(world) * aNormal;
    varCoord = aCoord;


    gl_Position = viewproj * camera * vec4(vertex, 1.0);
}
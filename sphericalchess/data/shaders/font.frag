#version 330


uniform sampler2D diffuseTexture;

in vec2 uv;

out vec4 fragColor;

void main()
{
    fragColor = vec4(1.0, 1.0, 1.0, texture(diffuseTexture, uv).r);
}
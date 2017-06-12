#version 330


uniform sampler2D diffuseTexture;

in vec2 uv;

out vec4 fragColor;

void main()
{
    fragColor = texture(diffuseTexture, uv);
}
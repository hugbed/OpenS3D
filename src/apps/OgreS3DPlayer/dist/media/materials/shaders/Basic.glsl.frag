#version 130

in vec2 vTexCoord;
uniform sampler2D textureSampler;

out vec4 color;

void main(void)
{
    color = texture(textureSampler, vTexCoord);
}

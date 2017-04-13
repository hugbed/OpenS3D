#version 130

in vec2 vTexCoord;
uniform sampler2D texture;

void main(void)
{
    gl_FragColor = texture(texture, vTexCoord);
}

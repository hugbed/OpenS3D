#version 330
in vec2 vTexCoord;

out vec4 fColor;

uniform sampler2D uTexture0;

void main()
{
   fColor = texture(uTexture0, vTexCoord);
}

#version 130

in vec4 position;
in vec2 uv0;

uniform mat4 worldViewProj;

out vec2 vTexCoord;

void main()
{
   gl_Position = worldViewProj * position;
   vTexCoord = uv0;
}

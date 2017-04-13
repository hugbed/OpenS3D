#version 130

in vec4 position;
in vec2 uv0;

uniform mat4 worldViewProj;
uniform float horizontalShift;

out vec2 vTexCoord;

void main()
{
   gl_Position = worldViewProj * position;
   vTexCoord = vec2(uv0.x + horizontalShift, uv0.y);
}

#version 130

in vec4 position;
in vec2 uv0;

uniform mat4 worldViewProj;
uniform float horizontalShift;

out vec2 vTexCoordLeft;
out vec2 vTexCoordRight;

void main()
{
   gl_Position = worldViewProj * position;
   vTexCoordLeft = vec2(uv0.x + horizontalShift, uv0.y);
   vTexCoordRight = vec2(uv0.x - horizontalShift, uv0.y);
}

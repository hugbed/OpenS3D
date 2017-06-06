#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;

uniform float uHorizontalShift;
uniform float uAspectRatio;

out vec2 vTexCoord;

void main()
{
  vec4 pos;
  if (uAspectRatio <= 1.0f) {
      pos = vec4(position.x, position.y * uAspectRatio, position.z, 1.0);
  } else {
      pos = vec4(position.x / uAspectRatio, position.y, position.z, 1.0);
  }
  gl_Position = pos;

  vTexCoord = vec2(texCoord.x + uHorizontalShift, texCoord.y);
}

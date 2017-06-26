#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;

uniform float uHorizontalShift;

out vec2 vTexCoordLeft;
out vec2 vTexCoordRight;

void main()
{
  vec2 rightPosX = position.x;
  vec2 leftPosX = position.x;
  if (fabs(position.x - 1.0) < 0.00001) {
    rightPosX = 1.0;
    leftPosX = 0.5;
  }

  float imageRatio = 0.5;
  position = vec3(position.x, position.y * imageRatio, position.z);

  gl_Position = vec4(position, 1.0);
  vTexCoordLeft = vec2(texCoord.x + uHorizontalShift, texCoord.y);
  vTexCoordRight = vec2(texCoord.x - uHorizontalShift, texCoord.y);
}

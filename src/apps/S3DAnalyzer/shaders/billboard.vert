#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

out vec4 vColor;

uniform float uHorizontalShift = 0.0f;
uniform float uAspectRatio = 1.0f;
uniform float uPointSize = 10.0f;

uniform mat3 uImageToScreen;

void main()
{
    vec3 screenPos = uImageToScreen * vec3(position.x - 1, position.y - 1, 1);

    vec4 pos;
    if (uAspectRatio <= 1.0f) {
        pos = vec4(screenPos.x + uHorizontalShift, screenPos.y * uAspectRatio, screenPos.z, 1.0);
    } else {
        pos = vec4((screenPos.x + uHorizontalShift) / uAspectRatio, screenPos.y, screenPos.z, 1.0);
    }

    gl_Position = pos;
    gl_PointSize = uPointSize;
    vColor = vec4(color, 1.0f);
}

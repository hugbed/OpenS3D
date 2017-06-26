#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in float intensity;

out vec4 vColor;

uniform float uHorizontalShift = 0.0f;
uniform float uAspectRatio = 1.0f;
uniform float uPointSize = 10.0f;
uniform float uMinIntensity = 0.0f;
uniform float uMaxIntensity = 1.0f;
uniform mat3 uImageToScreen;

const vec4 colors[6] = vec4[](
    vec4(214.0f, 69.0f, 65.0f, 255.0f)/255.0f,   // red
    vec4(235.0f, 151.0f, 78.0f, 255.0f)/255.0f,  // orange
    vec4(135.0f, 211.0f, 124.0f, 255.0f)/255.0f, // green
    vec4(135.0f, 211.0f, 124.0f, 255.0f)/255.0f, // green
    vec4(68.0f, 108.0f, 179.0f, 255.0f)/255.0f,  // blue
    vec4(102.0f, 51.0f, 153.0f, 255.0f)/255.0f); // violet

const float steps[6] = float[](
    0.0, 0.1, 0.2, 0.8, 0.9, 1.0
);

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

    // normalize intensity in range
    float intensity = (intensity + uHorizontalShift * 100.0f - uMinIntensity) / (uMaxIntensity - uMinIntensity);

    vec4 color = mix(colors[0], colors[1], smoothstep(steps[0], steps[1], intensity));
    color = mix(color, colors[2], smoothstep(steps[1], steps[2], intensity));
    color = mix(color, colors[3], smoothstep(steps[2], steps[3], intensity));
    color = mix(color, colors[4], smoothstep(steps[3], steps[4], intensity));
    color = mix(color, colors[5], smoothstep(steps[4], steps[5], intensity));
    vColor = color;
}

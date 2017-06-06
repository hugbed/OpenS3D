#version 330

in vec2 vTexCoordLeft;
in vec2 vTexCoordRight;

uniform sampler2D uTexture0;
uniform sampler2D uTexture1;

out vec4 fColor;

void main(void)
{
    vec4 leftFrag = texture(uTexture0, vTexCoordLeft);
    leftFrag = vec4(1.0, leftFrag.g, leftFrag.b, 1.0); // Left eye is full red and actual green and blue

    vec4 rightFrag = texture(uTexture1, vTexCoordRight);
    rightFrag = vec4(rightFrag.r, 1.0, 1.0, 1.0); // Right eye is full green and blue and actual red

    // Multiply left and right components for final ourput colour
    fColor = vec4(leftFrag.rgb * rightFrag.rgb, 1.0);
}

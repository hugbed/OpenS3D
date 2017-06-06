#version 330

in vec2 vTexCoordLeft;
in vec2 vTexCoordRight;

uniform sampler2D uTexture0;
uniform sampler2D uTexture1;

out vec4 fColor;

void main(void)
{
    vec4 t0 = texture(uTexture0, vTexCoordLeft);
    vec4 t1 = texture(uTexture1, vTexCoordRight);

    // divide in 10 blocks
    float nbBins = 10;
    int val = int(floor(vTexCoordLeft.y * nbBins));
    fColor = val % 2 == 0 ? t0 : t1;
}

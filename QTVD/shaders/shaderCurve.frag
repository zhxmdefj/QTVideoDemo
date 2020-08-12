#version 330 core
precision highp float;
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D inputImageTexture;
uniform sampler2D inputImageTexture2;

void main()
{
    vec4 base = texture2D(inputImageTexture, TexCoord);

    float r = texture2D(inputImageTexture2, vec2(base.r, 0.5)).r;
    float g = texture2D(inputImageTexture2, vec2(base.g, 0.5)).g;
    float b = texture2D(inputImageTexture2, vec2(base.b, 0.5)).b;

    r = texture2D(inputImageTexture2, vec2(r, 0.5)).a;
    g = texture2D(inputImageTexture2, vec2(g, 0.5)).a;
    b = texture2D(inputImageTexture2, vec2(b, 0.5)).a;

    FragColor = vec4(r, g, b, 1.0);
}

#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D screenTexture;
uniform lowp float saturation;


//亮度的权重(将rgb转成灰度)
const mediump vec3 luminanceWeighting = vec3(0.2125, 0.7154, 0.0721);

void main()
{
    lowp vec4 textureColor = texture2D(screenTexture, TexCoords);
    lowp float luminance = dot(textureColor.rgb, luminanceWeighting);//获得亮度
    lowp vec3 greyScaleColor = vec3(luminance);//灰度图的颜色
    FragColor = vec4(mix(greyScaleColor, textureColor.rgb, (saturation * 0.02)), textureColor.w);
}


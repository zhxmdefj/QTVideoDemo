precision mediump float;
varying highp vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D uSkinLookUpTexture;
uniform lowp float strength;

void main()
{
    vec4 texColor = texture2D(inputImageTexture, textureCoordinate);
    vec4 color;
    color.r = texture2D(uSkinLookUpTexture, vec2(texColor.r, 0.5)).r;
    color.g = texture2D(uSkinLookUpTexture, vec2(texColor.g, 0.5)).g;
    color.b = texture2D(uSkinLookUpTexture, vec2(texColor.b, 0.5)).b;
    color.a = 1.0;

    gl_FragColor = mix (texColor, color, strength * 0.01);
}


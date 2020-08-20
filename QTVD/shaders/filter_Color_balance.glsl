precision mediump float;
varying mediump vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform float cyan_red_shadow;
uniform float cyan_red_midtones;
uniform float cyan_red_highlights;

uniform float magenta_green_shadow;
uniform float magenta_green_midtones;
uniform float magenta_green_highlights;

uniform float yellow_blue_shadow;
uniform float yellow_blue_midtones;
uniform float yellow_blue_highlights;


vec3 transfer(float value)
{
    const float a = 64.0, b = 85.0, scale = 1.785;
    vec3 result;
    float i = value * 255.0;
    float shadows = clamp ((i - b) / -a + 0.5, 0.0, 1.0) * scale;
    float midtones = clamp ((i - b) /  a + 0.5, 0.0, 1.0) * clamp ((i + b - 255.0) / -a + .5, 0.0, 1.0) * scale;
    float highlights = clamp (((255.0 - i) - b) / -a + 0.5, 0.0, 1.0) * scale;
    result.r = shadows;
    result.g = midtones;
    result.b = highlights;
    return result;
}

vec3 rgb2hsl(vec3 color){
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(color.bg, K.wz), vec4(color.gb, K.xy), step(color.b, color.g));
    vec4 q = mix(vec4(p.xyw, color.r), vec4(color.r, p.yzx), step(p.x, color.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

//hsla转rgb
vec3 hsl2rgb(vec3 color){
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(color.xxx + K.xyz) * 6.0 - K.www);
    return color.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), color.y);
}

void main() {
    vec4 base = texture2D(inputImageTexture, textureCoordinate);
    vec3 hsl = rgb2hsl(base.rgb);
    vec3 weight_r = transfer(base.r);
    vec3 weight_g = transfer(base.g);
    vec3 weight_b = transfer(base.b);
    vec3 color = vec3(base.rgb * 255.0);
    color.r += (cyan_red_shadow - 50.0) * 2.0 * weight_r.r;
    color.r += (cyan_red_midtones - 50.0) * 2.0 * weight_r.g;
    color.r += (cyan_red_highlights - 50.0) * 2.0 * weight_r.b;

    color.g += (magenta_green_shadow - 50.0) * 2.0 * weight_g.r;
    color.g += (magenta_green_midtones - 50.0) * 2.0 * weight_g.g;
    color.g += (magenta_green_highlights - 50.0) * 2.0 * weight_g.b;

    color.b += (yellow_blue_shadow - 50.0) * 2.0 * weight_b.r;
    color.b += (yellow_blue_midtones - 50.0) * 2.0 * weight_b.g;
    color.b += (yellow_blue_highlights - 50.0) * 2.0 * weight_b.b;



    color.r = clamp(color.r, 0.0, 255.0);
    color.g = clamp(color.g, 0.0, 255.0);
    color.b = clamp(color.b, 0.0, 255.0);

    vec3 hsl2 = rgb2hsl(color / 255.0);
    hsl2.z = hsl.z;

    gl_FragColor=vec4(hsl2rgb(hsl2), base.a);
}

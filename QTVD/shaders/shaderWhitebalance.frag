#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform lowp float temperature; // 50.0
uniform lowp float tint;        // 50.0

uniform sampler2D screenTexture;

const lowp vec3 warmFilter = vec3(0.93, 0.54, 0.0);

const mediump mat3 RGBtoYIQ = mat3(0.299, 0.587, 0.114, 0.596, -0.274, -0.322, 0.212, -0.523, 0.311);
const mediump mat3 YIQtoRGB = mat3(1.0, 0.956, 0.621, 1.0, -0.272, -0.647, 1.0, -1.105, 1.702);

void main()
{
    lowp vec4 source = texture2D(screenTexture, TexCoords);

    mediump vec3 yiq = RGBtoYIQ * source.rgb; //adjusting tint
	yiq.b = clamp(yiq.b + (tint * 0.02 - 1.0) *0.5226*0.1, -0.5226, 0.5226);
	lowp vec3 rgb = YIQtoRGB * yiq;

	lowp vec3 processed = vec3(
		(rgb.r < 0.5 ? (2.0 * rgb.r * warmFilter.r) : (1.0 - 2.0 * (1.0 - rgb.r) * (1.0 - warmFilter.r))), //adjusting temperature
		(rgb.g < 0.5 ? (2.0 * rgb.g * warmFilter.g) : (1.0 - 2.0 * (1.0 - rgb.g) * (1.0 - warmFilter.g))), 
		(rgb.b < 0.5 ? (2.0 * rgb.b * warmFilter.b) : (1.0 - 2.0 * (1.0 - rgb.b) * (1.0 - warmFilter.b))));

	FragColor = vec4(mix(rgb, processed, (temperature * 0.02 - 1.0)), source.a);
    //FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
}
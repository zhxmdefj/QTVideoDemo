#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform lowp float brightness;
uniform sampler2D screenTexture;


void main()
{
	lowp vec4 textureColor = texture2D(screenTexture, TexCoords);

	highp float uWhiteness = (brightness - 50.0) / 60.0;
	uWhiteness += step(0.0, uWhiteness) * 10.0 * uWhiteness;

	highp float mwhite = 0.01+ 1.0;
	highp float Whiteness = 1.01 + mwhite * uWhiteness;
	vec3 result = log(textureColor.rgb * (Whiteness - 1.0) + 1.0) / log(Whiteness);

	FragColor = vec4(result, textureColor.w);
}


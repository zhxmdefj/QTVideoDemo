varying mediump vec4 textureOut;
uniform sampler2D textureY;
uniform sampler2D textureUV;
void main(void)
{
	vec3 yuv;
	vec3 rgb;
	yuv.x = texture2D(textureY, textureOut.st).r;
	yuv.y = texture2D(textureUV, textureOut.st).r - 0.5;
	yuv.z = texture2D(textureUV, textureOut.st).g - 0.5;
	rgb = mat3( 1,       1,         1,
				0,       -0.39465,  2.03211,
				1.13983, -0.58060,  0) * yuv;
	gl_FragColor = vec4(rgb, 1);
}

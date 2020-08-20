attribute vec4 aPosition;
uniform mat4 uTextureMatrix;
attribute vec4 aTextureCoordinate;
varying vec2 textureCoordinate;
void main()
{
  textureCoordinate = aTextureCoordinate.xy;

  gl_Position = aPosition;
}


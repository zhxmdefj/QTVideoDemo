attribute vec4 position;
attribute vec4 aTextureCoordinate;
varying vec2 textureCoordinate;
void main()
{
    gl_Position = position;
    textureCoordinate = aTextureCoordinate.xy;
}
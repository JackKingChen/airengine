attribute highp vec4 inPosition;
attribute highp vec4 inColor;

uniform mat4  wvpMatrix;
varying highp vec4  Color;

void main()
{
    gl_Position = wvpMatrix * vec4(inPosition.xyz, 1.0);
    Color = inColor;
}
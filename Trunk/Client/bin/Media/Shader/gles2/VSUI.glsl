/************************************
// ui vertex shader
************************************/

attribute highp vec4 inPosition;
attribute highp vec4 inColor;
attribute highp vec2 uvCoords;
uniform mat4 projectionMatrix;
varying highp vec4  Color;
varying highp vec2  TexCoord; 
void main()
{ 
    gl_Position = projectionMatrix * vec4(inPosition.xyz, 1.0);
    Color = inColor;
    TexCoord = uvCoords;
    //TexCoord.y = 1.0 - TexCoord.y;
}
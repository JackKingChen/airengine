
uniform mediump mat4 wvpMatrix;

attribute mediump vec3 position;
attribute mediump vec4 vertexColor;
attribute mediump vec2 uvCoords;

varying mediump vec4 color;
varying mediump vec2 texCoords;


void main()
{
    gl_Position = wvpMatrix * vec4(position, 1.0);
    texCoords = uvCoords;
    color = vertexColor;
}

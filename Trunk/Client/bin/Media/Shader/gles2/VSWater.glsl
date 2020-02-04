uniform mediump mat4 wvpMatrix;
uniform mediump mat4 worldMatrix;
uniform mediump mat4 textureProjMatrix;
uniform mediump mat4 WorldViewMatrix;

uniform mediump vec2 baseUVScale;


attribute mediump vec3 position;
//attribute mediump vec3 normal;
//attribute mediump vec3 tangent;
attribute mediump vec4 inColor;
attribute mediump vec2 uvCoords;




varying mediump vec4 posWorld;
varying mediump vec4 color;
varying mediump vec2 texCoords;
varying mediump vec4 outTexProj;
//varying mediump vec4 normalWorld;
varying mediump float  DepthInView;



void main()
{	
    gl_Position = wvpMatrix * vec4(position.xyz, 1.0);

    posWorld = worldMatrix * vec4(position.xyz, 1.0);
    texCoords = uvCoords;
	texCoords.y = 1.0 - texCoords.y;
    color = inColor;
    outTexProj = textureProjMatrix * vec4(position, 1.0);
    //vec4 vNormal = vec4( normal, 0 );
    //normalWorld = worldMatrix * vNormal;
    
    DepthInView = (WorldViewMatrix * vec4(position, 1.0)).z;
}

uniform sampler2D samplerMesh;    //mesh自己的纹理

uniform highp vec2 	halfPixel;
uniform highp vec4  ambientColor; 
uniform highp vec3 	cameraPos;
uniform highp vec4  fogColor;
uniform highp float  alphaTest;
uniform highp float  fClipValue;

varying highp vec2  texCoords; 
//varying highp vec3  outNormal; 
varying highp vec4  color;
varying mediump float fFogFactor;
 

void main()
{
	if (fClipValue < 0)
	{
		discard;
	}
	highp vec4 texCol = texture2D(samplerMesh, texCoords);
	//highp vec4 mc = vec4( texCol.b, texCol.g, texCol.r, texCol.a );
	if (texCol.a < alphaTest)
	{
		discard;
	}
	//gl_FragColor = mc * color;

	gl_FragColor = mix(fogColor, texCol * color, fFogFactor);
}




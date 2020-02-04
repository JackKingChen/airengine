
uniform sampler2D  SampRGB;
uniform sampler2D  Samp0;    //A
uniform sampler2D  Samp1;    //R
uniform sampler2D  Samp2;    //G
uniform sampler2D  Samp3;    //B

//uniform mediump vec2[5] uvScales;

uniform mediump vec2 uvScales0;
uniform mediump vec2 uvScales1;
uniform mediump vec2 uvScales2;
uniform mediump vec2 uvScales3;

uniform mediump vec4 ambientColor;
//uniform mediump vec2 halfPixel;
uniform mediump vec3 cameraPos;
uniform mediump vec4 fogColor;

varying mediump vec4 color;
varying mediump vec2 texCoords;
varying mediump float fFogFactor;
varying mediump float fClipValue;

vec4 CalculateTerrainColor(vec2 uvCoords)
{
    vec4 rgbTex = texture2D(SampRGB, uvCoords);
    vec4 colorSurface = texture2D(Samp0, uvCoords * uvScales0);
    vec4 colorSamp1 = texture2D(Samp1, uvCoords * uvScales1);
    vec4 colorSamp2 = texture2D(Samp2, uvCoords * uvScales2);
    vec4 colorSamp3 = texture2D(Samp3, uvCoords * uvScales3);

    vec4 colorLevel = colorSurface * rgbTex.a + colorSamp1 * rgbTex.r + colorSamp2 * rgbTex.g + colorSamp3 * rgbTex.b; 

    return colorLevel;
}

vec2 postProjToScreen(vec4 pos) 
{     
    vec2 screenPos = pos.xy / pos.w;     
    return (0.5 * (vec2(screenPos.x, -screenPos.y) + 1.0)); 
}   

#ifdef SHADOWMAP
void main()
{
	if (fClipValue < 0)
	{
		return;
	}
    gl_FragColor = CalculateTerrainColor(input.texCoords, input.posWorld);


    float shadow = CaculateShadow16(vPos, posInLight, ambientColor);

    gl_FragColor *= color * vec4(shadow, shadow, shadow, 1);

    //gl_FragColor.rgb = LerpColorWithFog(output.Color.rgb, cameraPos, posWorld);

}
#else
void main()
{
	if (fClipValue < 0)
	{
		discard;
	}
    vec4 terrainColor = CalculateTerrainColor(texCoords);

    terrainColor *= color;

    gl_FragColor = mix(fogColor, terrainColor, fFogFactor); //LerpColorWithFog(terrainColor, cameraPos, posWorld); // * zrgb;
}
#endif

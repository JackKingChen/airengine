uniform sampler2D srcColor;
uniform sampler2D sceneColor;

varying highp vec4  Color;
varying highp vec2  TexCoord; 


//普通叠加
void PSNormalCombine()
{
	gl_FragColor = texture2D(srcColor, TexCoord);
	gl_FragColor += texture2D(sceneColor, TexCoord);
}

//  (Soft Light)
void PSSoftLight()
{
	highp vec4 color1 = texture2D(sceneColor, TexCoord);
	highp vec4 color2 = texture2D(srcColor, TexCoord);
	if( color1.r <= 0.5 )
	{
		gl_FragColor.r = color1.r + ( 2*color2.r - 1.0f ) * (color1.r - pow(color1.r, 2.0f));
	}
	else
	{
		gl_FragColor.r = color1.r + ( 2*color2.r - 1.0f ) * (pow(color1.r, 0.5f) - color1.r);
	}
	
	if( color1.g <= 0.5 )
	{
		gl_FragColor.g = color1.g + ( 2*color2.g - 1.0f ) * (color1.g - pow(color1.g, 2.0f));
	}
	else
	{
		gl_FragColor.g = color1.g + ( 2*color2.g - 1.0f ) * (pow(color1.g, 0.5f) - color1.g);
	}
	
	if( color1.b <= 0.5 )
	{
		gl_FragColor.b = color1.b + ( 2*color2.b - 1.0f ) * (color1.b - pow(color1.b, 2.0f));
	}
	else
	{
		gl_FragColor.b = color1.b + ( 2*color2.b - 1.0f ) * (pow(color1.b, 0.5f) - color1.b);
	}
	
	gl_FragColor.a = 1.0f;
	gl_FragColor *= Color;
}

//Screen 滤色
void PSScreen()
{
	highp vec4 color1 = texture2D(sceneColor, TexCoord);
	highp vec4 color2 = texture2D(srcColor, TexCoord);
	gl_FragColor.rgb = vec3(1.0f, 1.0f, 1.0f) - (vec3(1.0f, 1.0f, 1.0f) - color1.rgb) * (vec3(1.0f, 1.0f, 1.0f) - color2.rgb);
	gl_FragColor.a = 1.0f;
	gl_FragColor *= Color;
}


//颜色加深(Color Burn)
void PSColorBurn()
{
	highp vec4 color1 = texture2D(sceneColor, TexCoord);
	highp vec4 color2 = texture2D(srcColor, TexCoord);
	gl_FragColor.rgb = vec3(1.0f, 1.0f, 1.0f) - (vec3(1.0f, 1.0f, 1.0f) - color1.rgb) / color2.rgb;
	gl_FragColor.a = 1.0f;
	gl_FragColor *= Color;
}

void main()
{
#ifdef USE_PSNORMALCOMBINE
	PSNormalCombine();
#elif defined USE_PSSOFTLIGHT
	PSSoftLight();
#elif defined USE_PSSCREEN
	PSScreen();
#elif defined USE_PSCOLORBURN
	PSColorBurn();
#endif

}
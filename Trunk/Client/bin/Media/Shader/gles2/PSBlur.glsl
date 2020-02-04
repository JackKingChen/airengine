uniform sampler2D srcColor;
uniform highp float bloomScale = 1.2f;


//改为循环7次 以提高效率
static const int g_cKernelSize = 7;

uniform highp vec2 TexelKernel[g_cKernelSize] ;

uniform highp float BlurWeights[g_cKernelSize] ;
 
varying highp vec4 color;
varying highp vec2 tex0;

//普通的横向模糊
void PSBlur()
{
	for(int i = 0 ; i < g_cKernelSize; i++)
	{
		gl_FragColor += texture2D( srcColor, tex0.xy + TexelKernel[i].xy ) * BlurWeights[i];
	}
}

//模糊处理横向比普通的处理增加了亮度
void PSBloomBlur()
{
	for(int i = 0 ; i < g_cKernelSize; i++)
	{
		gl_FragColor += texture2D( srcColor, tex0.xy + TexelKernel[i].xy ) * BlurWeights[i];
	}
	gl_FragColor *= bloomScale;
}

void main()
{
#ifdef USE_PSBLUR
	PSBlur();
#else
	PSBloomBlur();
#endif
}
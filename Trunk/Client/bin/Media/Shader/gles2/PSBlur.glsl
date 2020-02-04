uniform sampler2D srcColor;
uniform highp float bloomScale = 1.2f;


//��Ϊѭ��7�� �����Ч��
static const int g_cKernelSize = 7;

uniform highp vec2 TexelKernel[g_cKernelSize] ;

uniform highp float BlurWeights[g_cKernelSize] ;
 
varying highp vec4 color;
varying highp vec2 tex0;

//��ͨ�ĺ���ģ��
void PSBlur()
{
	for(int i = 0 ; i < g_cKernelSize; i++)
	{
		gl_FragColor += texture2D( srcColor, tex0.xy + TexelKernel[i].xy ) * BlurWeights[i];
	}
}

//ģ������������ͨ�Ĵ�������������
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
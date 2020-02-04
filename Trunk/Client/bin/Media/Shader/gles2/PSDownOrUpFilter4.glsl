uniform sampler2D srcColor;

uniform highp vec2 TexCoordsDownFilter[16];

varying highp vec4 color;
varying highp vec2 tex0;


void PSDownFilter4()
{
	for(int i = 0 ; i < 16 ; i++)
	{
		gl_FragColor += texture2D(srcColor, tex0.xy + TexCoordsDownFilter[i].xy) ;
	}
	gl_FragColor /= 16; 
}

void PSUpFilter4()
{
	gl_FragColor = texture2D(srcColor, tex0.xy);
}

void main()
{
#ifdef USE_PSDOWNFILTER4
	PSDownFilter4();
#else
	PSUpFilter4();
#endif
}
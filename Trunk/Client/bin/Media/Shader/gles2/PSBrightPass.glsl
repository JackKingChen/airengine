
uniform highp float luminance = 0.08;
static const float fMiddleGray = 0.18;
static const float fWhiteCutoff = 0.8f;

sampler2D srcColor;


varying highp vec4 color;
varying highp vec2 tex0;

void main()
{
	gl_FragColor = texture2D(srcColor, tex0.xy) ;
	gl_FragColor *= fMiddleGray / (luminance + 0.001f);
	gl_FragColor *= (1.0f + (gl_FragColor/(fWhiteCutoff * fWhiteCutoff )));
	gl_FragColor -= 5.0f;
	gl_FragColor = max(gl_FragColor, 0.0f);
	gl_FragColor /= (10.0f + gl_FragColor);
	return output;
}
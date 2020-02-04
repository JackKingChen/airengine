uniform sampler2D SampUI;
uniform int UseTex;


varying highp vec4  Color;
varying highp vec2  TexCoord;

void main()
{   
	highp vec4 sampleCol = UseTex > 0 ? texture2D(SampUI, TexCoord) : vec4( 1, 1, 1, 1 );
    gl_FragColor = sampleCol.a * Color;
}


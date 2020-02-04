varying highp vec4  Color;
void main()
{
    gl_FragColor = Color;
	gl_FragColor.a = 1.0;
}
uniform sampler2D 	uMoon;

varying	vec4 vUVYLength;

void main()
{
    gl_FragColor = texture2D( uMoon, vUVYLength.xy );
    gl_FragColor.a *= clamp( (vUVYLength.z / vUVYLength.w) * 3.5f, 0.0, 1.0 );
}


uniform sampler2D SampParticleSystem;

varying mediump vec4 color;
varying mediump vec2 texCoords;

void main()
{
    //gl_FragColor = texture2D(SampParticleSystem, texCoords) * color;
	mediump vec4 texCol = texture2D(SampParticleSystem, texCoords);
	//mediump vec4 mc = vec4( texCol.b, texCol.g, texCol.r, texCol.a );
	gl_FragColor = texCol * color;

}
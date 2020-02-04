uniform vec3		weatherColor;
uniform float		weatherMultiplier;


uniform vec3 		uLightDir;				//̫���ⷽ��
uniform int   		uNumberOfSamples;		//ȡ������
uniform float 		uSamples;				//ȡ������
uniform float  		uG;						//Phase function
uniform float  		uG2;					//G * G

#ifdef STARFIELD
uniform float		uTime;					//��ǰʱ�����[0-24Сʱ], x:��ǰʱ��, y:�ճ�ʱ��, z:����ʱ��
uniform sampler2D 	sampStarfield;// : register(s0);			//�ǿ�����
#endif


#ifdef LDR 
float  		uExposure;				//�ع�ϵ��
#endif 

varying	vec2 oUV;
varying	vec3 oRayleighColor;
varying	vec3 oMieColor;
varying	vec3 oDirection;
varying	float  oOpacity;
varying	float  oHeight;
varying	vec3 oAccumColor;
varying mediump float fClipValue;

void main()
{
	if (fClipValue < 0)
	{
		discard;
	}
	float cos = dot(uLightDir, oDirection) / length( oDirection );
	float cos2 = cos*cos;
	
	float rayleighPhase = 0.75 * (1.0 + 0.5 * cos2);
	
	float miePhase = 1.5f * ((1.0f - uG2) / (2.0f + uG2)) * (1.0f + cos2) / pow((1.0f + uG2 - 2.0f * uG * cos), 1.5f);

#ifdef LDR
	gl_FragColor = vec4((1 - exp(-uExposure * (rayleighPhase * oRayleighColor + miePhase * oMieColor))), oOpacity);
#else
    gl_FragColor = vec4(rayleighPhase * oRayleighColor + miePhase * oMieColor, oOpacity);
#endif

	// For night rendering
	float nightmult = clamp(1 - max(gl_FragColor.x, max(gl_FragColor.y, gl_FragColor.z))*10, 0.0, 1.0);
	

#ifdef STARFIELD
	vec3 starColor = texture2D(sampStarfield, oUV+uTime).xyz * oAccumColor.y;
	gl_FragColor.xyz += nightmult *(vec3(0.05, 0.05, 0.1)*(2-0.75 * clamp(-uLightDir.y, 0.0, 1.0)) * pow(oHeight,3) + starColor * (0.35f + clamp(-uLightDir.y * 0.45f, 0.0, 1.0))); 

#else
	gl_FragColor.xyz += nightmult *(vec3(0.05, 0.05, 0.1)*(2-0.75 * clamp(-uLightDir.y, 0.0, 1.0)) * pow(oHeight,3)); 
#endif

	//debug output sky vertex color
	//output.oColor.xyz = iAccumColor;
	
	
	gl_FragColor.xyz 	= mix( weatherColor, gl_FragColor.xyz, weatherMultiplier );
}

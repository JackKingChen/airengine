
uniform mat4	wvpMatrix;				//WorldViewProj矩阵
uniform mat4	worldMatrix;




float scale(float cos, float uScaleDepth)
{
	float x = 1.0 - cos;
	return uScaleDepth * exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));
}



attribute vec4 iPosition;
attribute	vec3 iNPosition;
attribute	vec2 iUV;
attribute	float  iOpacity;



	//vec4 oPosition			: POSITION;
varying	vec2 oUV;
varying	vec3 oRayleighColor;
varying	vec3 oMieColor;
varying	vec3 oDirection;
varying	float  oOpacity;
varying	float  oHeight;
varying	vec3 oAccumColor;
varying mediump float fClipValue;

uniform vec3 		uLightDir;				//太阳光方向
uniform vec3 		uCameraPos;				//摄像机当前位置
uniform vec3 		uInvWaveLength;			//1 / pow(waveLength, 4),对应于RGB
uniform float  		uInnerRadius;			//内径
uniform float  		uKrESun;				//Kr * ESun
uniform float  		uKmESun;				//Km * ESun
uniform float  		uKr4PI;					//Kr * 4 * pi
uniform float  		uKm4PI;					//Km * 4 * pi
uniform float 		uScale;					//1 / (outerRadius - innerRadius)
uniform float 		uScaleDepth;			//Where the average atmosphere density is found
uniform float 		uScaleOverScaleDepth;	//Scale / ScaleDepth
uniform mediump int  nClipEnable;
uniform mediump vec4 clipPlane;

void main()
{
	int nSamples = 4;
	float fSamples = 4.0;
	
    // Clip space position
	gl_Position   = wvpMatrix * iPosition;

	vec3 v3Pos = iNPosition;
	v3Pos.y += uInnerRadius;

	// 顶点到摄像机的射线
    vec3 v3Ray = v3Pos - uCameraPos;
	float fFar = length(v3Ray);
	v3Ray /= fFar;
	
	// Calculate the ray's starting position, then calculate its scattering offset
	vec3 v3Start = uCameraPos;
	float fHeight = uCameraPos.y;
	float fStartAngle = dot(v3Ray, v3Start) / fHeight;
	float fDepth = exp(uScaleOverScaleDepth * (uInnerRadius - uCameraPos.y));
	float fStartOffset = fDepth * scale(fStartAngle, uScaleDepth);

    // Init loop variables
	float fSampleLength = fFar / fSamples;
	float fScaledLength = fSampleLength * uScale;
	vec3 v3SampleRay = v3Ray * fSampleLength;
	vec3 v3SamplePoint = v3Start + v3SampleRay * 0.5f;
	
	// Loop the ray
	vec3 color = vec3(0,0,0);
	for ( int i = 0; i < nSamples; i++ )
	{
		float fHeight = length(v3SamplePoint);
		float fDepth = exp(uScaleOverScaleDepth * (uInnerRadius - fHeight));
		
		float fLightAngle = dot(uLightDir, v3SamplePoint) / fHeight;
		float fCameraAngle = dot(v3Ray, v3SamplePoint) / fHeight;
		
		float fScatter = (fStartOffset + fDepth*(scale(fLightAngle, uScaleDepth) - scale(fCameraAngle, uScaleDepth)));
		vec3 v3Attenuate = exp(-fScatter * (uInvWaveLength * uKr4PI + uKm4PI));
		
		// Accumulate color
		v3Attenuate *= (fDepth * fScaledLength);
		color += v3Attenuate;
		
		// Next sample point
		v3SamplePoint += v3SampleRay;
	}

    // Outputs
    oRayleighColor 	= color * (uInvWaveLength * uKrESun);
    oMieColor      	= color * uKmESun;
    oDirection     	= uCameraPos - v3Pos;
    oUV 			= iUV;
    oOpacity 		= iOpacity;
    oHeight 		= 1 - iNPosition.y;
    oAccumColor		= iNPosition;

	fClipValue = 1.0;
	if (nClipEnable > 0)
	{
		vec4 posWorld = worldMatrix * iPosition;
		fClipValue = dot(vec4(posWorld.xyz, 1.0), clipPlane);
	}
}


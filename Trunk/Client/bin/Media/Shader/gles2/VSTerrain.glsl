

attribute mediump vec3 position;
attribute mediump vec3 normal;
attribute mediump vec4 vertexColor;
attribute mediump vec2 uvCoords;


#ifdef SHADOWMAP

#endif

//
// Functions
//
uniform mediump mat4 wvpMatrix;
uniform mediump mat4  worldMatrix;
#ifdef SHADOWMAP
/*
float4 CascadeFars;
matrix LightSpaceProjMatrix;  //在以灯光为视点的投影空间
matrix textureProjMatrix;
matrix LightSpaceProjMatrixArray[4];
float4x4 matViewToLightProj;  // Transform from view space to light projection space
*/
#endif
uniform mediump vec3  sunLight;
uniform mediump vec3  cameraPos;
uniform mediump vec4  ambientColor;
uniform mediump vec4  sunColor;    //太阳光颜色
uniform mediump float   shininess;
uniform mediump vec3 ka;
uniform mediump vec3 kd;
uniform mediump vec3 ks;
uniform mediump float fogStart;
uniform mediump float fogEnd;
uniform mediump float fogDensity;
uniform mediump int  nClipEnable;
uniform mediump vec4 clipPlane;

varying mediump vec4 color;
varying mediump vec2 texCoords;
varying mediump float fFogFactor;
varying mediump float fClipValue;


float GetLinearFactor(float dis)
{
    return clamp((fogEnd - dis) / (fogEnd - fogStart), 0.0, 1.0);
}

//dis 代表摄像机到某点的距离
float CaculateFogFactor(float dis)
{
    return GetLinearFactor(dis);
}


void main()
{
    gl_Position = wvpMatrix * vec4(position.xyz, 1.0);
    
    texCoords = uvCoords;
    
#ifdef SHADOWMAP

#endif

    vec3 ambient = ka * ambientColor.rgb;

    // Compute the diffuse term
    vec3 N = normal;

	vec3 posWorld = (worldMatrix * vec4(position, 1.0)).xyz;
	
    float diffuseLight = max(dot(sunLight, N), 0.0);
    vec3 diffuse = kd * sunColor.rgb * diffuseLight;
    //diffuse = clamp(diffuse, ambient, vec3(1.0, 1.0, 1.0));

    vec3 V = normalize(cameraPos - posWorld);     //眼睛到点的向量
    vec3 H = normalize(sunLight + V);               //用了blinn模型计算光照
    float specularLight = pow(max(dot(H, N), 0), shininess);

    vec3 specular = ks * ambientColor.rgb * specularLight;

	color.rgb = clamp(ambient + diffuse + specular, 0.0, 1.0);

	color *= vertexColor;

	fFogFactor = CaculateFogFactor(distance(posWorld, cameraPos));

	fClipValue = 1.0;
	if (nClipEnable > 0)
	{
		fClipValue = dot(vec4(posWorld, 1.0), clipPlane);
	}

	//if (diffuse.r == 1.0)
	//    color.rgb = vec3(1.0, 0.0, 0.0);
}
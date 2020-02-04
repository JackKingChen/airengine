//模型
uniform mediump mat4 wvpMatrix;
uniform mediump mat4 worldMatrix;

attribute mediump vec3 position;
attribute mediump vec3 normal;
attribute mediump vec4 inColor;
attribute mediump vec2 uvCoords;

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

#ifdef RENDER_TO_SHADOWMAP

#else
  #ifdef SHADOWMAP
  uniform mat4 LightSpaceProjMatrix;  //在以灯光为视点的投影空间

  varying mediump vec3 posWorld;
  varying mediump vec4 posInLight;

  #else   //什么宏都没定义
  
  #endif
#endif

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
	texCoords.y = 1.0 - texCoords.y;
	vec3 worldNormal = normalize(worldMatrix * vec4(normal, 0)).xyz;
   
  #ifdef SHADOWMAP
    posInLight = LightSpaceProjMatrix * position;   //这里会有问题
  #endif

    vec3 posWorld = (worldMatrix * vec4(position, 1.0)).xyz;

	vec3 N = normal;

	vec3 ambient = ka * ambientColor.rgb;
	
    float diffuseLight = max(dot(sunLight, N), 0.0);
    vec3 diffuse = kd * sunColor.rgb * diffuseLight;

    vec3 V = normalize(cameraPos - posWorld);     //眼睛到点的向量
    vec3 H = normalize(sunLight + V);               //用了blinn模型计算光照
    float specularLight = pow(max(dot(H, N), 0), shininess);

    vec3 specular = ks * ambientColor.rgb * specularLight;

    color.rgb = clamp(ambient + diffuse + specular, 0.0, 1.0);

	color *= inColor;

	fFogFactor = CaculateFogFactor(distance(posWorld, cameraPos));
    
	fClipValue = 1.0;
	if (nClipEnable > 0)
	{
		fClipValue = dot(vec4(posWorld, 1.0), clipPlane);
	}
}
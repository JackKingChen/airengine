
attribute mediump vec3 position;
attribute mediump vec3 normal;
attribute mediump vec4 inColor;
attribute mediump vec2 uvCoords;




uniform mediump mat4  worldMatrix;
uniform mediump mat4  wvpMatrix;
uniform mediump vec4	offset;	
uniform mediump vec3  cameraPos;      // 摄像机位置
uniform mediump float 	frameTime;
uniform mediump mat4 WorldViewMatrix;

uniform mediump vec3  sunLight;
uniform mediump vec4  ambientColor;
uniform mediump vec4  sunColor;    //太阳光颜色
uniform mediump float   shininess;

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
    vec4 mypos 	= vec4(position, 1.0);
    vec4 factor 	= vec4(1,1,1,1) - vec4(uvCoords.y, uvCoords.y, uvCoords.y, uvCoords.y);	
	
    vec4 wpos 	= worldMatrix * mypos;
    float dist      = distance( vec3(0,0,0), wpos.xyz );
    float distFactor = sin(dist + frameTime);    
	
    mypos = mypos + offset * distFactor * factor;

    gl_Position = wvpMatrix * mypos;
    texCoords = uvCoords;
	texCoords.y = 1.0 - texCoords.y;

	vec3 N = normal;

	vec3 ambient = ambientColor.rgb;
	
    float diffuseLight = max(dot(sunLight, N), 0.0);
    vec3 diffuse = sunColor.rgb * diffuseLight;

    vec3 V = normalize(cameraPos - wpos.xyz);     //眼睛到点的向量
    vec3 H = normalize(sunLight + V);               //用了blinn模型计算光照
    float specularLight = pow(max(dot(H, N), 0), shininess);

    vec3 specular = ambientColor.rgb * specularLight;

    color.rgb = clamp(ambient + diffuse + specular, 0.0, 1.0);

    color *= inColor;

	fFogFactor = CaculateFogFactor(distance(wpos.xyz, cameraPos));
    
	fClipValue = 1.0;
	if (nClipEnable > 0)
	{
		fClipValue = dot(wpos, clipPlane);
	}
}


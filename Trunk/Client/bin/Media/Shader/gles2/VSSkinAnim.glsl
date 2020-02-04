//#version 120

precision highp int;
precision highp float;

uniform highp mat4  	boneMatrix[50];
uniform highp mat4 		ViewProjMatrix;
uniform highp mat4 		wvpMatrix;
uniform highp mat4 		WorldViewMatrix;
uniform highp vec3  	sunLight;
uniform highp vec3  	cameraPos;
uniform highp vec4  	ambientColor;
uniform highp vec4  	sunColor;    //太阳光
uniform highp float   	shininess;
uniform highp vec3 		ka;
uniform highp vec3 		kd;
uniform highp vec3 		ks;
uniform highp mat4 		worldMatrix;
uniform highp float     fogStart;
uniform highp float     fogEnd;
uniform highp float     fogDensity;
uniform highp int       nClipEnable;
uniform highp vec4      clipPlane;


attribute highp    vec3 position;
attribute highp    vec3 normal;
attribute highp    vec4 inColor;   
attribute highp    vec2 uvCoords;
attribute highp    vec4 Weights;   //BLENDWEIGHT;
attribute highp    vec4 Indices;   //BLENDINDICES;

varying highp vec4 		color;
varying highp vec2 		texCoords;
varying highp float 	fFogFactor;
varying highp float 	fClipValue;


float CaculateFogFactor(float dis)
{
    return clamp((fogEnd - dis) / (fogEnd - fogStart), 0.0, 1.0);
}

void main()
{
    vec4 inPos = vec4(position.xyz, 1.0);
	vec4 inNormal = vec4(normal.xyz, 1.0);
    vec4 tempPos = vec4(0,0,0,0);
	vec4 tempNormal = vec4(0,0,0,0);
	
	int idx = int(Indices.x);
	int idy = int(Indices.y);
	int idz = int(Indices.z);
	int idw = int(Indices.w);

	//if( Weights.x > 0.0 )
	{
		tempPos += (boneMatrix[idx] * inPos * Weights.x);
		tempNormal += (boneMatrix[idx] * inNormal * Weights.x);
	}
	//if( Weights.y > 0.0 )
	{
		tempPos += (boneMatrix[idy] * inPos * Weights.y);
		tempNormal += (boneMatrix[idy] * inNormal * Weights.y);
	}
    //if( Weights.z > 0.0 )
	{
		tempPos += (boneMatrix[idz] * inPos * Weights.z);
		tempNormal += (boneMatrix[idz] * inNormal * Weights.z);
	}
	//if( Weights.w > 0.0 )
	{
		tempPos += (boneMatrix[idw] * inPos * Weights.w);
		tempNormal += (boneMatrix[idw] * inNormal * Weights.w);
	}
    
	vec4 finalPos = vec4(inPos.xyz, 1.0);
	vec4 finalNormal = vec4(tempNormal.xyz, 1.0);
    gl_Position = wvpMatrix * finalPos;

    color = inColor;
    texCoords = uvCoords;
	texCoords.y = 1.0 - texCoords.y;

	vec3 worldNormal = normalize( (worldMatrix * vec4(tempNormal.xyz, 0.0)).xyz );
	vec3 posWorld = (worldMatrix * finalPos).xyz;
	vec3 N = normal;
	vec3 ambient = ka * ambientColor.rgb;
    float diffuseLight = max(dot(sunLight, N), 0.0);
    vec3 diffuse = kd * sunColor.rgb * diffuseLight;
    vec3 V = normalize(cameraPos - posWorld);     //眼睛到点的向量
    vec3 H = normalize(sunLight + V);               //用了blinn模型计算光照
    float specularLight = pow(max(dot(H, N), 0.0), shininess);
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

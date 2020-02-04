uniform sampler2D Samp0;          // 水基本纹理
uniform sampler2D ReflectMap;     // 反射纹理
uniform sampler2D RefractMap;     // 折射纹理
uniform sampler2D BumpMap;        // bumpmap贴图
uniform sampler2D HeightMap;       // 深度贴图
uniform sampler2D DepthMap;

uniform mediump vec3  cameraPos;      // 摄像机位置
uniform mediump float   frameTime;      // 系统时间值
uniform mediump float   distorsion;     // 波浪扰动参数
uniform mediump vec2  flowDirection;  // 波浪方向
uniform mediump float   flowSpeed;      // 波浪速度
uniform mediump vec4  fogColor;       // 雾颜色

uniform mediump vec4  ambientColor;   // 环境光颜色
uniform mediump vec3  sunLight;       // 太阳光方向
uniform mediump float   waterShiness;   // 水体的高光
uniform mediump vec3  waterFogColor;  // 水底下雾的颜色
uniform mediump float   gradient;       // 水的透明最大距离

uniform mediump vec2  baseUVScale;

uniform mediump float	farPlane;

uniform mediump vec2  terrainRealSize;	//地形的实际大小
uniform mediump float   heightScale;		//地形的高度缩放值
uniform mediump float   realStartHeight;	//地形的起始高度


varying mediump vec4 posWorld;
varying mediump vec4 color;
varying mediump vec2 texCoords;
varying mediump vec4 outTexProj;
//varying mediump vec4 normalWorld;
varying mediump float  DepthInView;

//计算发射的颜色
vec4 CaculateReflectColor(vec2 texProj, vec2 offsets)
{
    vec2 realTexProj = vec2(1.0 - texProj.x, 1.0 - texProj.y);
    vec2 lastCoords = clamp(realTexProj + offsets, 0.0, 1.0);
    return texture2D(ReflectMap, lastCoords);
}

//计算折射的颜色
vec4 CaculateRefractColor(vec2 texProj, vec2 offsets)
{
	texProj.y = 1.0 - texProj.y;
    vec2 lastCoords = clamp(texProj + offsets, 0.0, 1.0);
    return texture2D(RefractMap, lastCoords);
}

//根据高度图对水面进行边缘柔化
float CaculateHeightmapSoftedge(vec3 posWorld)
{
    vec2 height_map_coord;
    height_map_coord.x = (posWorld.x + terrainRealSize.x/2.0) / terrainRealSize.x;
    height_map_coord.y = (terrainRealSize.x/2.0 - posWorld.z) / terrainRealSize.x;
    vec3 height_color = texture2D(HeightMap, height_map_coord).xyz;
    float  vecTerrainHeight = height_color.x;// * 255 * heightScale + realStartHeight;
    
    // 水面边缘采用透明处理的最大高度差
    //float  fMaxWaterHeight = 50.0f;//(fPos.y - realStartHeight) * 0.5;
    
    // offsetHeight : 水面高度减去对应的地面顶点的高度的差值
    float  offsetHeight =  posWorld.y - vecTerrainHeight;
    
    // 当水面低于对应地形顶点高度时,改像素的透明度为0
    // 当地形顶点低于水面时, 如果高度差在fMaxWaterHeight之内,采用渐变透明, 超过fMaxWaterHeight就设置为不透明
    //float  alphaRate = offsetHeight < 0.0 ? 0.0 : saturate( offsetHeight / fMaxWaterHeight );
    
    return clamp( offsetHeight / gradient, 0.0, 1.0 );//alphaRate;

}

//根据深度图对水面进行边缘柔化处理
float CaculateDepthmapSoftedge(float DepthInView, vec2 texProj)
{
    // 取得屏幕空间的点所对应的深度值
    vec4 DepthColor = texture2D(DepthMap, texProj);
    // 空间中的点的深度值
    float fDepth = -DepthColor.x * farPlane;
    // 水面顶点的深度值
    float fPosDepth = DepthInView;
    // 水面到水底的颜色的渐变参数
    float foam = 1.0f;
    // 水面边缘采用透明处理的最大高度差
    float fMaxWaterHeight = 50.0f;
    // 计算深度差
    float offsetDepth = fDepth - fPosDepth;
    //foam = offsetDepth < 0.0 ? 0.0 : saturate( offsetDepth / fMaxWaterHeight );
    return clamp( offsetDepth / fMaxWaterHeight, 0.0, 1.0 );//foam;
    
}


//计算水面高光
//float CaculateWaterSpecular(vec2 texCoord, vec4 dudv_offset, vec3 vCamToP)
float CaculateWaterSpecular(vec3 bumpNormal, vec3 vCamToP)
{
    //vec4 dudvn = dudv_offset + texture2D(BumpMap, texCoord);
    //太阳光的方向
    float fShine = waterShiness;  
    vec3 vL = normalize( -sunLight.xyz );
    //水面法向
    //vec3 vN = normalize( dudvn.xyz + vec3(0,1,0) );
    vec3 vN = normalize( bumpNormal.xyz + vec3(0,1,0) );
    //计算半角向量
    vec3 vH = normalize( vCamToP - vL );
    //计算高光
    float  vSpec = pow( abs( max( dot( vN, vH ), 0 ) ), fShine );
    return vSpec;
}


//主函数
void main()
{

    
    // 归一化水流方向
    vec3 windDir = normalize( vec3( flowDirection.x, 0, flowDirection.y ) );    
    // 获取垂直于水流的方向
    vec3 perpDir = cross(windDir, vec3(0,1,0));
    // 获取经水流方向修正的纹理uv坐标
    float ydot = dot(texCoords, windDir.xz);
    float xdot = dot(texCoords, perpDir.xz);
    vec2 moveVector = vec2(xdot, ydot);
    // 让纹理的v坐标随时间移动
    moveVector.y += frameTime * flowSpeed;    
    // 获取最终的凹凸纹理采样坐标
    vec2 bumpMapSamplingPos = moveVector * baseUVScale;
    
    // 采样凹凸纹理颜色
    vec4 bumpColor = texture2D(BumpMap, bumpMapSamplingPos);
    vec2 perturbation = distorsion * (bumpColor.rg - 0.5f) * 2.0f;


    // 计算水体基本颜色

    vec4 WaterColor = texture2D(Samp0, bumpMapSamplingPos);

    vec2 texProj = outTexProj.xy / outTexProj.w;

    vec3 vCamToP = normalize(cameraPos - posWorld.xyz);
#ifdef HAS_REFLECT
    // 计算入射角度的余弦值    
    float cosAngle = dot( vCamToP, vec3(0,1,0) );
    
    // 入射角的影响因子
    float angle_factor = clamp(cosAngle / 0.752f, 0.0, 1.0);
    vec4 ReflectColor = CaculateReflectColor(texProj, perturbation);
  #ifdef HAS_REFRACT
  //既有折射又有反射
    //计算折射颜色
    vec4 RefractColor = CaculateRefractColor(texProj, perturbation);
    // 根据菲涅尔参数对折射色和反射色进行混合
    gl_FragColor = mix( ReflectColor, RefractColor, angle_factor );
    // 再与水的纹理颜色进行混合
    gl_FragColor = mix( gl_FragColor, WaterColor,   0.01 );
  #else
    //只有反射
    gl_FragColor = mix( ReflectColor, WaterColor, 0.01 );
  #endif
#else
  #ifdef HAS_REFRACT
    //vec4 RefractColor = CaculateRefractColor(texProj, offsets);
    vec4 RefractColor = CaculateRefractColor(texProj, perturbation);
    gl_FragColor = mix( RefractColor, WaterColor, 0.01 );
  #else
  #endif
#endif
    gl_FragColor *= color;

	//处理高光
    //gl_FragColor.rgb += vec3(1,1,1) * CaculateWaterSpecular(texCoords, dudv_offset, vCamToP);
    vec3 perturbationNormal = distorsion * bumpColor.rgb;
    gl_FragColor.rgb += vec3(1,1,1) * CaculateWaterSpecular(perturbationNormal.xyz, vCamToP);

#ifdef HEIGHTMAP_SOFT_EDGE
    gl_FragColor.a = CaculateHeightmapSoftedge(posWorld.xyz);
#elif defined(DEPTHMAP_SOFT_EDGE)
    gl_FragColor.a = CaculateDepthmapSoftedge(DepthInView, texProj);
#endif
	
#ifdef HAS_REFLECT
	gl_FragColor.a = 1.0;
#endif
}

//--------------------------ps const table begin
#ifdef LDR
uniform float     uExposure;
#endif // LDR

// Sun information
uniform vec3    uSunPosition;
uniform vec3    uSunColor;

// Main cloud layer parameters
uniform float     uHeight;
uniform float     uTime;
uniform float     uScale;
uniform vec2    uWindDirection;

// Advanced cloud layer parameters
uniform float     uCloudLayerHeightVolume; 				// 0.25
uniform float     uCloudLayerVolumetricDisplacement; 	// 0.01
uniform vec3    uAmbientLuminosity; 					// 0.55 0.55 0.55
uniform float     uNormalMultiplier; 					// 2
uniform float     uDetailAttenuation; 					// 0.45
uniform float     uDistanceAttenuation; 				// 0.05

uniform vec3		weatherColor;
uniform float		weatherMultiplier;

uniform sampler2D   uCloud;// 	: register(s0);
uniform sampler2D   uNormal;// 	: register(s1);
uniform sampler2D   uDensity;// 	: register(s2);

varying vec3 vNPos;
//--------------------------ps const table end


void main()
{
    // Get the cloud pixel lenght on the projected plane
    float vh = uHeight / vNPos.y;
    
    // Get the 3D position of the cloud pixel
    vec3 CloudPosition = vNPos * vh;
    
    vec2 twcoord = uTime * uWindDirection * 0.25f;
    
    // Get texture coords
    vec2 TexCoord		= CloudPosition.xz * uScale;
    float  Density   	=        texture2D( uDensity, TexCoord + twcoord ).r;
    vec3 Normal   	= -( 2 * texture2D( uNormal,  TexCoord + twcoord ).xyz - 1 );
    Normal.zy 			= Normal.yz;
 
    ///------------ Volumetric effect:
    float CloudLayerHeightVolume 			= uCloudLayerHeightVolume * vNPos.y;
    float CloudLayerVolumetricDisplacement 	= uCloudLayerVolumetricDisplacement * vNPos.y;
    vec3 iNewPosition 					= normalize( vNPos + CloudLayerVolumetricDisplacement * vec3( Normal.x, 0, Normal.z ) );
    vh 										= ( uHeight + uHeight * ( 1 - Density ) * CloudLayerHeightVolume) / iNewPosition.y;
    CloudPosition 							= iNewPosition * vh;
    TexCoord      							= CloudPosition.xz * uScale;
    Density    								= texture2D( uDensity, TexCoord + twcoord ).r;
    ///------------
    
    vec3 SunToPixel 	= CloudPosition - uSunPosition;    
    float  CloudDetail 	=        texture2D( uCloud,  TexCoord - twcoord ).r;
    Normal     			= -( 2 * texture2D( uNormal, TexCoord + twcoord ).xyz - 1 );
    Normal.zy  			= Normal.yz;
    Normal.xz 		   *= uNormalMultiplier;

    vec3 PixelColor 	= vec3(0,0,0);
    
    // AMBIENT addition
    PixelColor 		   += uAmbientLuminosity;
    
    // SUN addition 
    PixelColor         += uSunColor * clamp( dot( -normalize(Normal), normalize(uSunPosition) ), 0.0, 1.0 );
    
    // FINAL colour
    float Alpha 		= Density * clamp( 10 * clamp( -uDistanceAttenuation + vNPos.y, 0.0, 1.0 ), 0.0, 1.0 );
    
    //gl_FragColor 		= float4( PixelColor * ( 1- Density * 0.35 ), Alpha * clamp( 1 - CloudDetail * uDetailAttenuation, 0.0, 1.0 ) );
    gl_FragColor.rgb	= PixelColor * ( 1- Density * 0.35 );
	gl_FragColor.a 		= Alpha * clamp( 1 - CloudDetail * uDetailAttenuation, 0.0, 1.0 );
	
#ifdef LDR
    gl_FragColor.xyz 	= vec3( 1 - exp( -uExposure * gl_FragColor.xyz ) );
#endif // LDR

	gl_FragColor.xyz 	= mix( weatherColor, gl_FragColor.xyz, weatherMultiplier );
}

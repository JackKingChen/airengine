
attribute vec4 iPosition;
attribute vec4 iColor;
attribute vec2 iUV;


uniform mat4 	wvpMatrix;
uniform mat4	worldMatrix;
uniform vec3 	cameraPos;

varying	vec4 vUVYLength;


void main()
{
    // Clip space position
	gl_Position   = wvpMatrix * iPosition;
	// World position
	vec3 ObjectSpacePosition = ( worldMatrix * iPosition).xyz - cameraPos;

    // UV
    vUVYLength.xy = iUV;
    // Y
    vUVYLength.z  = ObjectSpacePosition.y;
    // Length
    vUVYLength.w  = length(ObjectSpacePosition);
}



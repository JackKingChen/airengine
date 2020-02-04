


attribute vec4 iPosition;
attribute vec3 vNPosition;



varying vec3 vNPos;



//--------------------------vs const table begin
uniform mat4  wvpMatrix;	
//--------------------------vs const table end	







void main()
{
	gl_Position = wvpMatrix * iPosition;
    vNPos  	 = vNPosition;
}


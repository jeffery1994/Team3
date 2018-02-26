#version 330 core

uniform mat4 uProjViewMtx;
uniform mat4 uModelMtx;

layout (location = 0) in  vec3 position;

//out Vertex{
//	smooth vec2 	 texCoord;
//} OUT;

void main(void)	{

	gl_Position		= uProjViewMtx * uModelMtx * vec4(position, 1.0);
	//OUT.texCoord	= texCoord;
}
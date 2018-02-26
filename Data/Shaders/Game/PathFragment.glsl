#version 440 core


in Vertex	{
	smooth vec2 	 texCoord;
} IN;

out vec4 OutFrag;

uniform vec4 uColor;

uniform sampler2D	uPathTex;

layout(binding = 0, offset = 0) uniform atomic_uint scores[4];

void main(void)	{

	vec4 color = texture(uPathTex,IN.texCoord);

	// if (color == vec4(1,0,0,1)) 
	// {
	// 	atomicCounterIncrement(scores[0]);
	// } 
	// else if (color == vec4(0,1,0,1))
	// {
	// 	atomicCounterIncrement(scores[1]);
	// }
	// else if (color == vec4(0,0,1,1))
	// {
	// 	atomicCounterIncrement(scores[2]);
	// }
	// vec2 texCoord = normalize(vec2(gl_FragCoord.x,gl_FragCoord.y));
	// OutFrag = vec4(texCoord.x,texCoord.y,0,1);
	// return;

	if (uColor != color)
	{
		if (uColor == vec4(1,0,0,1))
		{
			atomicCounterIncrement(scores[0]);
		}
		else if (uColor == vec4(0,1,0,1))
		{
			atomicCounterIncrement(scores[1]);
		}
		else if (uColor == vec4(0,0,1,1))
		{
			atomicCounterIncrement(scores[2]);
		}
	}

	OutFrag = uColor;

}
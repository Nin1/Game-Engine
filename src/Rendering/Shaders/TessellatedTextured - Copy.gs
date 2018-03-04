#version 430 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 tePosition[3];
in vec3 teFragPos[3];
in vec2 teTexCoord[3];
in vec3 teNormal[3];
out vec3 gFragPos;
out vec3 gNormal;
out vec2 gTexCoord;

void main()
{
	gFragPos = teFragPos[0];
	gNormal = teNormal[0];
	gTexCoord = teTexCoord[0];
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	
	gFragPos = teFragPos[1];
	gNormal = teNormal[1];
	gTexCoord = teTexCoord[1];
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();
	
	gFragPos = teFragPos[2];
	gNormal = teNormal[2];
	gTexCoord = teTexCoord[2];
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();

	EndPrimitive();
}
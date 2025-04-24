#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

uniform mat4 mvp;

out vec2 fragTexCoordColor;
out vec2 fragTexCoordMask;

void main()
{
	vec3 Pos = vertexPosition.xyz;
	fragTexCoordColor = vertexTexCoord;
	fragTexCoordMask = (mvp*vec4(Pos, 1.0)).xy/2.f + vec2(0.5, 0.5);
	//fragTexCoordMask.y = -fragTexCoordMask.y;

	vec4 FinalPos = mvp*vec4(Pos, 1.0);
    gl_Position = FinalPos;
}
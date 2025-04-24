#version 330

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

uniform mat4 mvp;

out vec2 fragPos;
out vec2 fragTexCoord;
out vec2 fragTexCoord2;
out vec4 fragColor;

void main()
{
	vec3 Pos = vertexPosition.xyz;
	fragPos = Pos.xy;
	fragTexCoord = (mvp*vec4(Pos, 1.0)).xy/2.f + vec2(0.5, 0.5);
	fragTexCoord2 = fragTexCoord;
	fragTexCoord.y = -fragTexCoord.y 
;
    fragColor = vertexColor;

	vec4 FinalPos = mvp*vec4(Pos, 1.0);
	//FinalPos.y = -FinalPos.y;
    gl_Position = FinalPos;
}
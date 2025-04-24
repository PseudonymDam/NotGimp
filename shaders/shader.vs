#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

// Input uniform values
uniform mat4 mvp;

// Output vertex attributes (to fragment shader)
out vec2 fragPos;
out vec2 fragTexCoord;
out vec4 fragColor;
out vec2 fragCirclePos;

// NOTE: Add here your custom variables

void main()
{
	vec3 Pos = vertexPosition.xyz;
	fragPos = Pos.xy;
	fragTexCoord = (mvp*vec4(Pos, 1.0)).xy/2.f + vec2(0.5, 0.5);
	fragTexCoord.y = -fragTexCoord.y;
    fragColor = vertexColor;
	fragCirclePos = vertexTexCoord;

	vec4 FinalPos = mvp*vec4(Pos, 1.0);
    gl_Position = FinalPos;
}
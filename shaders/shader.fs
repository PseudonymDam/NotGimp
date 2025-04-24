#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragPos;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec2 fragCirclePos;

// Output fragment color
out vec4 finalColor;

uniform float radius;
uniform sampler2D mask;

void main(){
	float t = 1 - length(fragCirclePos-fragPos)/radius;
	float a = smoothstep(0, 1, t);
	finalColor =  texture(mask, fragTexCoord).r*fragColor*vec4(1, 1, 1, a);
}
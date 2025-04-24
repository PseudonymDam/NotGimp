#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragPos;
in vec2 fragTexCoord;
in vec2 fragTexCoord2;
in vec4 fragColor;
in vec2 fragCirclePos;

// Output fragment color
out vec4 finalColor;

uniform sampler2D mask;
uniform sampler2D text;

void main(){
	finalColor = texture(mask, fragTexCoord).r*texture(text, fragTexCoord2);
}
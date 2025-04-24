#version 330

in vec2 fragTexCoordColor;
in vec2 fragTexCoordMask;

out vec4 finalColor;

uniform sampler2D colors;
uniform sampler2D mask;

void main(){
	finalColor =  texture(colors, fragTexCoordColor) * texture(mask, fragTexCoordMask).r;
}
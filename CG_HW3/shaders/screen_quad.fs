#version 330 core

uniform sampler2D screenTexture;

in vec2 iTexCoord;

out vec4 FragColor; 

void main()
{
    vec3 TexelColor = texture(screenTexture, iTexCoord).rgb;
	TexelColor = pow(TexelColor, vec3(1.0f/2.2f));
	FragColor = vec4(TexelColor, 1.0f);
}
#version 330

out vec4 FragColor;
in vec2 texCoord0;

uniform sampler2D diffuse;
uniform float myCounter;

void main()
{

	////////////////////////////////////////////////////////////
	// AD lighting
	// This section of the code is inspired by Basic Lighting
	// https://learnopengl.com/Lighting/Basic-Lighting 

	// ambient
	vec3 lightColor = vec3(0.9, 0.6, 0.0);
	float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;
    vec3 result = ambient;
	//vec3 result = ambient * vec3(0.2,0.9,0.0);

	// diffuse
	vec4 dColour;
	dColour = texture2D(diffuse, texCoord0);

	// compine effects (Ambient and Diffuse)
	vec4 colour = dColour + vec4(result, 0.5);
	////////////////////////////////////////////////////////////


	// Self-made really simple transparency effect
	// transparency changes depending on the value of myCounter value
	FragColor = vec4(colour.xyz, myCounter);
}

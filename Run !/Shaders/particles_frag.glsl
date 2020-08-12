#version 430

uniform sampler2D Texture;
uniform float iTime;

in vec2 texCoords;
out vec4 FragColor;
uniform vec2 res;

void main() {
	vec3 col = texture(Texture, texCoords).rgb;
	if(col.x == 1.0 && col.y == 0.0 && col.z == 1.0) discard;
	FragColor = vec4(col, 1.0);
	
}
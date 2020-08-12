#version 430

uniform sampler2D Texture;
uniform float iTime;

in vec2 texCoords;
out vec4 FragColor;
uniform vec2 res;

uniform float life;

void main() {
	vec3 col = texture(Texture, texCoords).rgb;
	if(col.x == 1.0 && col.y == 0.0 && col.z == 1.0) discard;
	vec2 co = gl_FragCoord.xy / res.xy;
	if(co.x > 0.04375 && co.x < 0.04375 + 0.12625 * life && co.y < 0.939f && co.y > 0.916) col = vec3(0, 1, 0);
	FragColor = vec4(col, 1.0);
}
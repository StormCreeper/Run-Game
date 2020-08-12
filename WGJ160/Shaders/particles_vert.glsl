#version 430

layout (location = 0) in vec2 aVertexPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 texCoords;

uniform vec2 res;
uniform vec2 offset;
uniform vec2 view;
uniform float zoom;

uniform int tcAsAttribute;
uniform vec4 UtexCoords;

uniform int aspect;

void main() {
	if(tcAsAttribute == 0) {
		texCoords = vec2(mix(UtexCoords.x, UtexCoords.z, aTexCoords.x), mix(UtexCoords.y, UtexCoords.w, aTexCoords.y));
	} else {
		texCoords = aTexCoords;
	}
	gl_Position = vec4((aVertexPos + offset - view) * zoom, 0.0, 1.0);
	gl_Position.y *= - 1;
	if(aspect == 1) gl_Position.y *= res.x / res.y;
}
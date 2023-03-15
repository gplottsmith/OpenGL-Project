#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

out vec2 vTexCoord;

uniform mat4 uMVP;

void main() {
	gl_Position = uMVP * position;
	vTexCoord = texCoord;
};

#shader fragment
#version 330 core

uniform vec4 uColor;
uniform sampler2D uTexture;

in vec2 vTexCoord;

layout(location = 0) out vec4 color;

void main() {
	vec4 texColor = texture(uTexture, vTexCoord);
	color = texColor;
};
#shader vertex
#version 330 core

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aColor;
layout( location = 2 ) in float aModelIndex;

out vec4 vColor;

uniform mat4 uVP;
uniform vec4 uColor;
uniform mat4 uModelTransforms[ 32 ];

void main() {

	int matrixIndex = int( aModelIndex );
	mat4 model = uModelTransforms[ matrixIndex ];

	gl_Position = uVP * model * aPosition;
	vColor = aColor;
};

#shader fragment
#version 330 core

uniform vec4 uColor;

in vec4 vColor;

layout(location = 0) out vec4 color;

void main() {
	color = vColor;
};
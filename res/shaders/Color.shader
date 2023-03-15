#shader vertex
#version 330 core

layout( location = 0 ) in vec4 aPosition;

out vec4 vColor;

uniform mat4 uMVP;
uniform vec4 uColor;

void main( ) {

	gl_Position = uMVP * aPosition;
	vColor = uColor;
};

#shader fragment
#version 330 core

uniform vec4 uColor;

in vec4 vColor;

layout( location = 0 ) out vec4 color;

void main( ) {
	color = vColor;
};
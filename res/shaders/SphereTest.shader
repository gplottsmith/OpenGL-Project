#shader vertex
#version 330 core

layout( location = 0 ) in vec4 aPosition;
layout( location = 1 ) in vec4 aColor;

out vec4 vColor;

uniform mat4 uMVP;
uniform vec4 uColor;
uniform vec4 uQuaternion;

void main( ) {


	gl_Position = uMVP * aPosition;
	vColor = aColor;
};

#shader fragment
#version 330 core

uniform vec4 uColor;

in vec4 vColor;

layout( location = 0 ) out vec4 color;

void main( ) {
	color = vColor;
};
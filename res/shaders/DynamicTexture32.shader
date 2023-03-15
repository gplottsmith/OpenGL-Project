#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout( location = 1 ) in vec4 appColor;
layout(location = 2) in vec2 texCoord;
layout( location = 3 ) in float texIndex;
layout( location = 4 ) in float modelIndex;

out vec2 vTexCoord;
out vec4 vColor;
flat out int vTexIndex;

uniform mat4 uVP;
uniform mat4 uModelTransforms[ 32 ];

void main() {

	int matrixIndex = int( modelIndex );
	mat4 model = uModelTransforms[ matrixIndex ];
	gl_Position = uVP * model * position;
	
	vTexCoord = texCoord;
	vTexIndex = int( texIndex );
	vColor = appColor;
};

#shader fragment
#version 330 core

uniform sampler2D uTextures[ 32 ];

in vec2 vTexCoord;
in vec4 vColor;
flat in int vTexIndex;

layout(location = 0) out vec4 color;

void main() {

	vec4 texColor = vColor;
	
	if( vTexIndex != 0 )
		texColor = texture( uTextures[ vTexIndex ], vTexCoord );

	color = texColor;
};
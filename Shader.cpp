

#include "Shader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Shader.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Block/GameObject.h"

Shader::Shader(const std::string & filepath) :
	mFilePath(filepath), mRendererID(0), mVA( ), mVB( ), 
	mRenderIndices( nullptr ), mRenderIndicesCount( 0 ), mTextures( nullptr ), mTexCount( 0 ), mModifier( )
{
	ShaderProgramSource source = parseShader(filepath);
	mRendererID = createShader(source.vertexSource, source.fragmentSource);

}

Shader::~Shader()
{
	glCall(glDeleteProgram(mRendererID));

	delete[ ] mRenderIndices;
	mRenderIndices = nullptr;

	for( uint32_t i = 0; i < mTexCount; i++ )
	{
		delete mTextures[ i ];
		mTextures[ i ] = nullptr;
	}
	delete[ ] mTextures;
	mTextures = nullptr;
}

unsigned int Shader::createShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

unsigned int Shader::compileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glCall(glShaderSource(id, 1, &src, nullptr));
	glCall(glCompileShader(id));

	int result;
	glCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
	if (!result)
	{
		int length;
		glCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		char* message = (char*)alloca(length * sizeof(char));
		glCall(glGetShaderInfoLog(id, length, &length, message));
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader:" << std::endl;
		std::cout << message << std::endl;
		glCall(glDeleteShader(id));
		return 0;
	}
	return id;
}

ShaderProgramSource Shader::parseShader(const std::string& filepath)
{
	enum class ShaderType 
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	ShaderType type = ShaderType::NONE;

	std::ifstream stream(filepath);
	std::stringstream ss[2];

	std::string line;

	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else
		{
			ss[(int)type] << line << "\n";
		}
	}
	return { ss[0].str(), ss[1].str() };
}

void Shader::bind() const
{
	glCall( glUseProgram( mRendererID ) );
}

void Shader::unbind() const
{
	glCall( glUseProgram( 0 ) );
}

void Shader::setUniform4d(const std::string& name, float f0, float f1, float f2, float f3)
{
	glCall( glUniform4f( getUniformLocation( name ), f0, f1, f2, f3 ) );
}

void Shader::setUniform4d( const std::string& name, const glm::vec4& v )
{
	glCall( glUniform4f( getUniformLocation( name ), v.x, v.y, v.z, v.w ) );
}

void Shader::setUniform4f(const std::string& name, Quaternion& q)
{
	glCall( glUniform4f( getUniformLocation( name ), ( GLfloat )q.getX(), ( GLfloat )q.getY( ), ( GLfloat )q.getZ( ), ( GLfloat )q.getW( ) ) );
}

void Shader::setUniform1d(const std::string& name, float d0)
{
	glCall(glUniform1f(getUniformLocation(name), d0));
}

void Shader::setUniform1i(const std::string& name, int i0)
{
	glCall(glUniform1i(getUniformLocation(name), i0));
}

void Shader::setUniform1iv( const std::string& name, int size, int* indices )
{
	glCall( glUniform1iv( getUniformLocation( name ), size, indices ) );
}

void Shader::setUniformMat4f(const std::string& name, const glm::mat4 matrix)
{
	glCall(glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

void Shader::setUniformMat4fv( const std::string& name, glm::mat4* matrix, uint32_t length )
{
	glCall( glUniformMatrix4fv( getUniformLocation( name ), length, GL_FALSE, &matrix[ 0 ][ 0 ][ 0 ] ) );
}

int Shader::getUniformLocation( const std::string& name )
{
	if( mUniformLocationCache.find( name ) != mUniformLocationCache.end( ) )
		return mUniformLocationCache[ name ];

	glCall( int location = glGetUniformLocation( mRendererID, name.c_str( ) ) );
	if( location == -1 )
		std::cout << "Warning: Uniform " << name << " Doesn't exist.\n";

	mUniformLocationCache[ name ] = location;
	return location;
}

void Shader::initRenderDataStaticModel( const std::vector< Block >& blockArray, uint32_t arrayLength )
{
	//The application needs to fill an unordered_map with image filenames from the blocks, then set the texture shader's uniforms for them all
	std::unordered_map< std::string, int > texMap;
	std::queue< std::string > texNameQueue;
	std::queue< glm::vec4 > colQueue;
	std::queue< Texture* > texQueue;
	int texIndex = 1;	// A face having a texIndex of 0 means it uses a color
	std::vector< Material > materials;

	for( uint32_t i = 0; i < arrayLength; i++ )
	{
		materials = blockArray[ i ].getMaterialVec( );
		for( unsigned int j = 0; j < materials.size( ); j++ )
		{
			switch( materials.at( j ).getType( ) )
			{
			case Material::image:
				if( texMap.find( materials.at( j ).getImgName( ) ) == texMap.end( ) )
				{
					texMap[ materials.at( j ).getImgName( ) ] = texIndex++;
					texNameQueue.push( materials.at( j ).getImgName( ) );
				}
				break;

			case Material::color:
				colQueue.push( materials.at( j ).getColor( ) );

			}
		}
	}
	materials.clear( );

	// init Textures. Textures are bound to their corresponding slots
	int samplers[ 32 ] = { 0 };
	int index = 1;
	for( int i = 1; !texNameQueue.empty( ); i++ )
	{
		Texture* t = new Texture( texNameQueue.front( ) );
		texNameQueue.pop( );

		samplers[ index++ ] = i;

		t->bind( i );
		texQueue.push( t );
	}
	this->setUniform1iv( "uTextures", 32, samplers );

	// init mTextures
	mTexCount = texQueue.size( );
	mTextures = new Texture* [ mTexCount ];
	for( int i = 0; !texQueue.empty( ); i++ )
	{
		mTextures[ i ] = texQueue.front( );
		texQueue.pop( );
	}

	//Next, we fill Queues with the necessary data to instantiate a VB, VA, and IB
	std::queue< glm::vec3 > texVertPos;
	std::queue< float > texUV;
	std::queue< int > texTexIndex;
	std::queue< glm::vec4 > texColors;
	std::queue< int > texIndexElements;
	std::queue< float > texModelIndices;
	std::unordered_map< std::string, int >::const_iterator indTex;
	int bufferTexIndex;
	int materialCount;
	int materialIndex = 0;
	std::string name;
	glm::vec4 colorVec;
	std::vector< Face > faces;
	glm::vec3 vertPos;
	int texModelIndex = 0;
	for( uint32_t i = 0; i < arrayLength; i++ )
	{
		faces = blockArray[ i ].getFaces( );
		materials = blockArray[ i ].getMaterialVec( );
		materialCount = materials.at( 0 ).getCount( );
		for( int j = 0, size = blockArray[ i ].getFaceCount( ); j < size; j++ )
		{
			switch( materials.at( materialIndex ).getType( ) )
			{
			case Material::image:

				name = materials.at( materialIndex ).getImgName( );
				indTex = texMap.find( name );
				colorVec = glm::vec4( 0.0f, 0.0f, 0.0f, 1.0f );
				bufferTexIndex = indTex->second;
				break;

			case Material::color:

				colorVec = materials.at( materialIndex ).getColor( );
				bufferTexIndex = 0;
			}
			for( int k = 0; k < 3; k++ )
			{
				texColors.push( colorVec );
				texVertPos.push( faces[ j ].getVertexAt( k ).getCoordinate( ) );
				texTexIndex.push( bufferTexIndex );
				texIndexElements.push( texModelIndex++ );
				texModelIndices.push( ( float ) i );
			}

			materialCount--;
			if( materialCount == 0 && j != size - 1 )
			{
				materialIndex++;
				materialCount = materials.at( materialIndex ).getCount( );
			}
		}

		//Fill a temp Queue with UV Elements
		std::vector< float > uvArr = blockArray[ i ].getUVArray( );
		std::vector< int > uvIndices = blockArray[ i ].getUVIndices( );
		float u, v;
		for( int j = 0, size = blockArray[ i ].getFaceCount( ) * 3; j < size; j++ )
		{
			u = uvArr[ uvIndices[ j ] * 2 ];
			v = uvArr[ uvIndices[ j ] * 2 + 1 ];
			texUV.push( u );
			texUV.push( v );
		}
		materialIndex = 0;
	}
	int offset = 12;
	int texVertSize = texVertPos.size( ) * offset;   // # of Vertices is texVertPos.size( ), and there are offset floats per vert
	float* texVertices = new float[ texVertSize ];
	float x, y, z, w, r, g, b, a, u, v, tc, mi;
	for( int i = 0; i < texVertSize; i += offset )
	{
		colorVec = texColors.front( ); texColors.pop( );
		vertPos = texVertPos.front( ); texVertPos.pop( );

		x = vertPos.x;
		y = vertPos.y;
		z = vertPos.z;
		w = 1.0f;
		r = colorVec.r;
		g = colorVec.g;
		b = colorVec.b;
		a = colorVec.a;
		u = texUV.front( ); texUV.pop( );
		v = texUV.front( ); texUV.pop( );
		tc = ( float ) texTexIndex.front( ); texTexIndex.pop( );
		mi = texModelIndices.front( ); texModelIndices.pop( );

		texVertices[ i ] = x;  texVertices[ i + 1 ] = y;  texVertices[ i + 2 ] = z;  texVertices[ i + 3 ] = w;
		texVertices[ i + 4 ] = r;  texVertices[ i + 5 ] = g;  texVertices[ i + 6 ] = b;  texVertices[ i + 7 ] = a;
		texVertices[ i + 8 ] = u;  texVertices[ i + 9 ] = v;  texVertices[ i + 10 ] = tc;  texVertices[ i + 11 ] = mi;
		/*
		if( ( i / offset ) % 3 == 0 )
			std::cout << std::endl;
		std::cout << "Position: " << x << " " << y << " " << z << " " << w << std::endl
				  << "Color: " << r << " " << g << " " << b << " " << a << std::endl
				  << "Other: " << " " << u << " " << v << " " << tc << " " << mi << std::endl;
		*/
	}
	VertexBufferLayout texLayout;
	texLayout.push< float >( 4 ); // Position
	texLayout.push< float >( 4 ); // Color
	texLayout.push< float >( 2 ); // UV
	texLayout.push< float >( 1 ); // Texture Index
	texLayout.push< float >( 1 ); // Model Index

	mVB = std::make_unique<VertexBuffer>( texVertices, texVertSize * sizeof( float ) );

	delete[ ] texVertices;
	texVertices = nullptr;

	mVA = std::make_unique<VertexArray>( );
	mVA->addBuffer( *mVB, texLayout );
	mVA->unbind( );
	mVB->unbind( );

	// init Tex Index Array and Index Buffer
	mRenderIndicesCount = texIndexElements.size( );
	mRenderIndices = new int[ mRenderIndicesCount ];

	for( int i = 0; !texIndexElements.empty( ); i++ )
	{
		mRenderIndices[ i ] = texIndexElements.front( );
		texIndexElements.pop( );
	}

}

void Shader::initRenderDataDynamicModel( const std::vector< Block >& blockArray, uint32_t arrayLength, glm::mat4* transforms )
{
	// Initializing shader data for dynamic shader is the same as for static, just need to add a Matrix array for Transforms
	initRenderDataStaticModel( blockArray, arrayLength ); 

	setUniformMat4fv( "uModelTransforms", transforms, 32 );
}

void Shader::initRenderDataStaticCollider( const std::vector< GameObject >& objArray )
{
	mModifier.matrices.reserve( objArray.size( ) );
	mModifier.positionOffsets.reserve( objArray.size( ) );
	std::queue< glm::vec3 > positions;
	std::queue< glm::vec4 > colors;
	glm::vec3 positionElement;
	std::queue< int > indices;
	float* vertexBufferElements;
	int vertexBufferElementCount;
	int vertexOffset;
	float x, y, z, w, r, g, b, a;

	std::vector< int > indexIterator;

	for( const GameObject& g : objArray )
	{
		mModifier.matrices.emplace_back( 1.0f );
		mModifier.positionOffsets.emplace_back( g.getCollidersVertexCount( ) );
		for( uint32_t i = 0; i < g.getColliders( ).size( ); i++ )
		{
			for( int j = 0, size = g.getColliders( )[ i ].getVertexCount( ); j < size; j++ )
			{
				positions.push( g.getColliders( )[ i ].getVertexVec3At( j ) );
				colors.push( glm::vec4( ( std::rand( ) % 256 ) / 255.0f, ( std::rand( ) % 256 ) / 255.0f,
					( std::rand( ) % 256 ) / 255.0f, 1.0f ) );
			}
		}
	}
	// Number of Floats per Vertex
	vertexOffset = 8;
	vertexBufferElementCount = positions.size( ) * vertexOffset;
	vertexBufferElements = new float[ vertexBufferElementCount ];

	for( int i = 0; i < vertexBufferElementCount; i += vertexOffset )
	{
		positionElement = positions.front( );  positions.pop( );

		x = positionElement.x;
		y = positionElement.y;
		z = positionElement.z;
		w = 1.0f;
		r = ( std::rand( ) % 256 ) / 255.0f;
		g = ( std::rand( ) % 256 ) / 255.0f;
		b = ( std::rand( ) % 256 ) / 255.0f;
		a = 1.0f;

		vertexBufferElements[ i ] = x;  vertexBufferElements[ i + 1 ] = y;  vertexBufferElements[ i + 2 ] = z;  vertexBufferElements[ i + 3 ] = w;
		vertexBufferElements[ i + 4 ] = r;  vertexBufferElements[ i + 5 ] = g;  vertexBufferElements[ i + 6 ] = b;  vertexBufferElements[ i + 7 ] = a;
	}

	VertexBufferLayout texLayout;
	texLayout.push< float >( 4 ); // Position
	texLayout.push< float >( 4 ); // Color

	mVB = std::make_unique<VertexBuffer>( vertexBufferElements, vertexBufferElementCount * sizeof( float ) );

	mVA = std::make_unique<VertexArray>( );
	mVA->addBuffer( *mVB, texLayout );
	mVA->unbind( );
	mVB->unbind( );

	delete[ ] vertexBufferElements;
	vertexBufferElements = nullptr;

	//init Indices
	uint32_t indexOffset = 0;
	for( uint32_t i = 0; i < objArray.size( ); i++ )
	{
		const std::vector< Block >& blockArray = objArray[ i ].getColliders( );
		for( uint32_t j = 0; j < blockArray.size( ); j++ )
		{
			indexIterator = blockArray[ j ].getIndices( );
			for( int k = 0, count = blockArray[ j ].getIndexCount( ); k < count; k++ )
			{
				indices.push( indexIterator[ k ] + indexOffset );
			}
			indexOffset += blockArray[ j ].getVertexCount( );
		}
	}
	
	mRenderIndicesCount = indices.size( );
	mRenderIndices = new int[ mRenderIndicesCount ];
	for( uint32_t i = 0; i < mRenderIndicesCount; i++ )
	{
		mRenderIndices[ i ] = indices.front( );
		indices.pop( );
	}
}

void Shader::initRenderDataDynamicCollider( const std::vector< GameObject >& objArray, glm::mat4* transforms )
{
	// Initializing shader data for dynamic shader is the same as for static, just need to add a Matrix array for Transforms
	initRenderDataStaticCollider( objArray );

	setUniformMat4fv( "uModelTransforms", transforms, 32 );
}

void Shader::initRenderDataWireSphere( float r, float t, uint32_t div, const glm::vec3& pos, const glm::vec4& col )
{
	uint32_t index = 0;
	// Number of Floats per Vertex
	int vertexOffset = 4;
	int vertexBufferElementCount = ( 8 + ( div * 4 ) ) * 24;
	float* vertexBufferElements = new float[ vertexBufferElementCount ];

	float angle = glm::pi< float >( ) / 2.0f;
	angle /= ( float )( div + 1 );
	Quaternion q( glm::vec3( 0, 1, 0 ), angle );

	glm::vec3 left( -r, 0, 0 );
	glm::vec3 mid( 0, 0, r );
	glm::vec3 right( r, 0, 0 );
	t /= 2.0f;

	// 2 Points from Left Quad
	vertexBufferElements[ index++ ] = left.x;
	vertexBufferElements[ index++ ] = left.y + t;
	vertexBufferElements[ index++ ] = left.z + t;
	vertexBufferElements[ index++ ] = 1.0f;
	vertexBufferElements[ index++ ] = left.x;
	vertexBufferElements[ index++ ] = left.y - t;
	vertexBufferElements[ index++ ] = left.z + t;
	vertexBufferElements[ index++ ] = 1.0f;

	// Side Curve
	for( uint32_t i = 0; i < div; i++ )
	{
		q.rotate( left );
		vertexBufferElements[ index++ ] = left.x;
		vertexBufferElements[ index++ ] = left.y + t;
		vertexBufferElements[ index++ ] = left.z;
		vertexBufferElements[ index++ ] = 1.0f;
		vertexBufferElements[ index++ ] = left.x;
		vertexBufferElements[ index++ ] = left.y - t;
		vertexBufferElements[ index++ ] = left.z;
		vertexBufferElements[ index++ ] = 1.0f;
	}
	// Mid Quad
	vertexBufferElements[ index++ ] = mid.x - t;
	vertexBufferElements[ index++ ] = mid.y + t;
	vertexBufferElements[ index++ ] = mid.z;
	vertexBufferElements[ index++ ] = 1.0f;
	vertexBufferElements[ index++ ] = mid.x - t;
	vertexBufferElements[ index++ ] = mid.y - t;
	vertexBufferElements[ index++ ] = mid.z;
	vertexBufferElements[ index++ ] = 1.0f;
	vertexBufferElements[ index++ ] = mid.x + t;
	vertexBufferElements[ index++ ] = mid.y + t;
	vertexBufferElements[ index++ ] = mid.z;
	vertexBufferElements[ index++ ] = 1.0f;
	vertexBufferElements[ index++ ] = mid.x + t;
	vertexBufferElements[ index++ ] = mid.y - t;
	vertexBufferElements[ index++ ] = mid.z;
	vertexBufferElements[ index++ ] = 1.0f;

	// Side Curve
	for( uint32_t i = 0; i < div; i++ )
	{
		q.rotate( mid );
		vertexBufferElements[ index++ ] = mid.x;
		vertexBufferElements[ index++ ] = mid.y + t;
		vertexBufferElements[ index++ ] = mid.z;
		vertexBufferElements[ index++ ] = 1.0f;
		vertexBufferElements[ index++ ] = mid.x;
		vertexBufferElements[ index++ ] = mid.y - t;
		vertexBufferElements[ index++ ] = mid.z;
		vertexBufferElements[ index++ ] = 1.0f;
	}
	// 2 Points from Right Quad
	vertexBufferElements[ index++ ] = right.x;
	vertexBufferElements[ index++ ] = right.y + t;
	vertexBufferElements[ index++ ] = right.z + t;
	vertexBufferElements[ index++ ] = 1.0f;
	vertexBufferElements[ index++ ] = right.x;
	vertexBufferElements[ index++ ] = right.y - t;
	vertexBufferElements[ index++ ] = right.z + t;
	vertexBufferElements[ index++ ] = 1.0f;

	Quaternion backArc( glm::vec3( 0, 1, 0 ), glm::pi< float >( ) );
	Quaternion topArc( glm::vec3( -1, 0, 0 ), glm::pi< float >( ) / 2 );
	topArc.composeWith( Quaternion( glm::vec3( 0, 1, 0 ), glm::pi< float >( ) / 2.0f ) );
	Quaternion botArc( topArc );
	botArc.composeWith( Quaternion( glm::vec3( 0, 0, 1 ), glm::pi< float >( ) ) );
	Quaternion leftArc( glm::vec3( 0, -1, 0 ), glm::pi< float >( ) / 2.0f );
	leftArc.composeWith( Quaternion( glm::vec3( -1, 0, 0 ), glm::pi< float >( ) / 2 ) );
	Quaternion rightArc( leftArc );
	rightArc.composeWith( Quaternion( glm::vec3( 0, 1, 0 ), glm::pi< float >( ) ) );

	uint32_t floatsPerArc = vertexBufferElementCount / 6;
	int ind1 = floatsPerArc;
	int ind2 = floatsPerArc * 2;
	int ind3 = floatsPerArc * 3;
	int ind4 = floatsPerArc * 4;
	int ind5 = floatsPerArc * 5;
	glm::vec3 frontVec;
	glm::vec3 vec;
	for( uint32_t i = 0; i < floatsPerArc; i += 4 )
	{
		frontVec.x = vertexBufferElements[ i ];
		frontVec.y = vertexBufferElements[ i + 1 ];
		frontVec.z = vertexBufferElements[ i + 2 ];

		vec = backArc.getRotatedVec3( frontVec );
		vertexBufferElements[ ind1++ ] = vec.x;
		vertexBufferElements[ ind1++ ] = vec.y;
		vertexBufferElements[ ind1++ ] = vec.z;
		vertexBufferElements[ ind1++ ] = 1.0f;

		vec = topArc.getRotatedVec3( frontVec );
		vertexBufferElements[ ind2++ ] = vec.x;
		vertexBufferElements[ ind2++ ] = vec.y;
		vertexBufferElements[ ind2++ ] = vec.z;
		vertexBufferElements[ ind2++ ] = 1.0f;

		vec = botArc.getRotatedVec3( frontVec );
		vertexBufferElements[ ind3++ ] = vec.x;
		vertexBufferElements[ ind3++ ] = vec.y;
		vertexBufferElements[ ind3++ ] = vec.z;
		vertexBufferElements[ ind3++ ] = 1.0f;

		vec = leftArc.getRotatedVec3( frontVec );
		vertexBufferElements[ ind4++ ] = vec.x;
		vertexBufferElements[ ind4++ ] = vec.y;
		vertexBufferElements[ ind4++ ] = vec.z;
		vertexBufferElements[ ind4++ ] = 1.0f;

		vec = rightArc.getRotatedVec3( frontVec );
		vertexBufferElements[ ind5++ ] = vec.x;
		vertexBufferElements[ ind5++ ] = vec.y;
		vertexBufferElements[ ind5++ ] = vec.z;
		vertexBufferElements[ ind5++ ] = 1.0f;
	}

	VertexBufferLayout layout;
	layout.push< float >( 4 ); // Position

	mVB = std::make_unique<VertexBuffer>( vertexBufferElements, vertexBufferElementCount * sizeof( float ) );

	mVA = std::make_unique<VertexArray>( );
	mVA->addBuffer( *mVB, layout );
	mVA->unbind( );
	mVB->unbind( );

	delete[ ] vertexBufferElements;
	vertexBufferElements = nullptr;

	//init Indices
	index = 0;
	mRenderIndicesCount = ( ( vertexBufferElementCount / 8 ) - 1 ) * 6;
	mRenderIndices = new int[ mRenderIndicesCount ];
	for( uint32_t i = 0; index < mRenderIndicesCount; i += 2 )
	{
		if( ( i + 2 ) % ( floatsPerArc / 4 ) == 0 )
			i += 2;
		
		mRenderIndices[ index++ ] = i;
		mRenderIndices[ index++ ] = i + 2;
		mRenderIndices[ index++ ] = i + 1;
		mRenderIndices[ index++ ] = i + 2;
		mRenderIndices[ index++ ] = i + 3;
		mRenderIndices[ index++ ] = i + 1;
	}
}

void Shader::drawStaticModel( const glm::mat4& projTimesView )
{
	bind( );
	int samplers[ 32 ] = { 0 };
	for( int i = 0; i < 32; i++ )
		samplers[ i ] = i;
	setUniform1iv( "uTextures", 32, samplers );

	for( uint32_t i = 0; i < mTexCount; i++ )
		mTextures[ i ]->bind( i + 1 );

	mVA->bind( );
	setUniformMat4f( "uVP", projTimesView );

	IndexBuffer ib( mRenderIndices, mRenderIndicesCount );
	ib.bind( );

	glDrawElements( GL_TRIANGLES, mRenderIndicesCount, GL_UNSIGNED_INT, nullptr );

	unbind( );
	mVA->unbind( );
	ib.unbind( );
}

void Shader::drawDynamicModel( const glm::mat4& projTimesView, glm::mat4* transforms )
{
	bind( );
	int samplers[ 32 ] = { 0 };
	for( int i = 0; i < 32; i++ )
		samplers[ i ] = i;
	setUniform1iv( "uTextures", 32, samplers );

	setUniformMat4fv( "uModelTransforms", transforms, 32 );

	for( uint32_t i = 0; i < mTexCount; i++ )
		mTextures[ i ]->bind( i + 1 );

	mVA->bind( );
	setUniformMat4f( "uVP", projTimesView );

	IndexBuffer ib( mRenderIndices, mRenderIndicesCount );
	ib.bind( );

	glDrawElements( GL_TRIANGLES, mRenderIndicesCount, GL_UNSIGNED_INT, nullptr );

	unbind( );
	mVA->unbind( );
	ib.unbind( );
}

void Shader::drawStaticCollider( const glm::mat4& projTimesView )
{
	bind( );

	for( uint32_t i = 0; i < mTexCount; i++ )
		mTextures[ i ]->bind( i + 1 );

	mVA->bind( );
	setUniformMat4f( "uVP", projTimesView );

	IndexBuffer ib( mRenderIndices, mRenderIndicesCount );
	ib.bind( );

	glDrawElements( GL_TRIANGLES, mRenderIndicesCount, GL_UNSIGNED_INT, nullptr );

	unbind( );
	mVA->unbind( );
	ib.unbind( );
}

void Shader::drawDynamicCollider( const glm::mat4& viewProjection, glm::mat4* transforms )
{
	bind( );

	setUniformMat4fv( "uModelTransforms", transforms, 32 );

	mVA->bind( );
	setUniformMat4f( "uVP", viewProjection );

	IndexBuffer ib( mRenderIndices, mRenderIndicesCount );
	ib.bind( );

	glDrawElements( GL_TRIANGLES, mRenderIndicesCount, GL_UNSIGNED_INT, nullptr );

	unbind( );
	mVA->unbind( );
	ib.unbind( );
}

void Shader::drawWireSphere( const glm::mat4& mvp, const glm::vec4& color )
{
	bind( );
	setUniform4d( "uColor", color);

	for( uint32_t i = 0; i < mTexCount; i++ )
		mTextures[ i ]->bind( i + 1 );

	mVA->bind( );
	setUniformMat4f( "uMVP", mvp );

	IndexBuffer ib( mRenderIndices, mRenderIndicesCount );
	ib.bind( );

	glDrawElements( GL_TRIANGLES, mRenderIndicesCount, GL_UNSIGNED_INT, nullptr );

	unbind( );
	mVA->unbind( );
	ib.unbind( );
}



#include "GameObject.h"



bool GameObject::colliderRayIntersects( const glm::vec3& rayPoint, const glm::vec3& rayDir, glm::vec3* hit ) const
{
	glm::vec3 cubeHit, minHit;
	float distSquared;
	float min = INFINITY;
	for( const Block& b : mColliders )
	{
		for( const Facet& f : b.getFacets( ) )
		{
			if( f.rayIntersects( rayDir,rayPoint, &cubeHit, &distSquared ) )
			{
				if( distSquared < min )
				{
					min = distSquared;
					minHit = cubeHit;
				}
			}
		}
	}
	if( min == INFINITY )
		return false;
	if( hit )
		*hit = minHit;
	return true;
}

uint32_t GameObject::getCollidersVertexCount( ) const
{
	uint32_t count = 0;
	for( const Block& b : mColliders )
		count += ( uint32_t )b.getVertexCount( );

	return count;
}

void GameObject::updateTransform( const glm::vec4& t, const Quaternion& r, const float& s )
{
	mModel.updateTransform( t, r, s );
	for( Block& b : mColliders )
		b.updateTransform( t, r, s );
}

void GameObject::setStatic( bool s )
{
	mModel.setStatic( s );

	for( uint32_t i = 0; i < mColliders.size( ); i++ )
		mColliders[ i ].setStatic( s );
}

GameObject::GameObject( ) :
	mModel( ), mColliders( )
{
}

GameObject::GameObject( const GameObject& other ) :
	mModel( other.mModel ), mColliders( other.mColliders )
{
}

glm::vec3 calculateCentroid( const std::vector< Vertex >& verts )
{
	unsigned int n = verts.size( );
	glm::vec3 centroid = glm::vec3( );

	for( unsigned int i = 0; i < n; i++ )
	{
		centroid += verts[ i ].getCoordinateVec3( );
	}

	centroid.x /= n;
	centroid.y /= n;
	centroid.z /= n;

	return centroid;
}

GameObject::GameObject( const std::string& modelFile, const std::string& colliderFile ) :
	mModel( modelFile.c_str( ) ), mColliders( )
{
	DAEFileParser parser( colliderFile.c_str( ) );
	int colliderMeshCount = parser.getMeshCount( );
	mColliders.reserve( colliderMeshCount );

	int indexBufferSize;

	std::vector< Vertex > vertices; 
	std::vector< float > vertexElements;
	std::vector< int > faceVertexIndices; 
	std::vector< int > normalsVertexIndices;
	std::vector< Face > faces;  
	std::vector< glm::vec3 > normals;
	std::vector< float > normalsElements;

	int* tempV;
	int* tempN;
	int* tempUV;

	for( int i = 0; i < colliderMeshCount; i++ )
	{
		parser.initVertexElements( vertexElements, i );
		parser.initNormalArray( normalsElements, i );
		parser.initIndexArrays( &tempV, &tempN, &tempUV, i, &indexBufferSize );

		vertices.reserve( vertexElements.size( ) / 3 );

		// init vertices
		for( unsigned int vertexIndex = 0; vertexIndex < vertexElements.size( ); vertexIndex += 3 )
		{
			vertices.emplace_back( vertexElements[ vertexIndex ],
				vertexElements[ vertexIndex + 2 ], vertexElements[ vertexIndex + 1 ] );
		}

		// init indices
		faceVertexIndices.reserve( indexBufferSize );
		for( int j = 0; j < indexBufferSize; j++ )
		{
			faceVertexIndices.emplace_back( tempV[ j ] );
			normalsVertexIndices.emplace_back( tempN[ j ] );
		}

		// init normals
		normals.reserve( normalsElements.size( ) / 3 );

		for( unsigned int j = 0; j < normalsElements.size( ); j += 3 )
		{
			normals.emplace_back( normalsElements[ j ], normalsElements[ j + 2 ], normalsElements[ j + 1 ] ); // Swap X and Y
		}

		//Init Face Array
		faces.reserve( indexBufferSize );
		std::vector< int > faceIndices;
		faceIndices.reserve( 3 );

		for( int j = 0; j < indexBufferSize; j += 3 )
		{
			faceIndices.emplace_back( tempV[ j ] );
			faceIndices.emplace_back( tempV[ j + 1 ] );
			faceIndices.emplace_back( tempV[ j + 2 ] );

			// The Constructor used to make Colliders for Game Objects sets the Face Vertex vector, don't need to set it here
			faces.emplace_back( nullptr, faceIndices );
			faceIndices.clear( );
		}

		mColliders.emplace_back( vertices, tempV, tempN, faces, normals, calculateCentroid( vertices ), colliderFile );

		vertices.clear( );
		faceVertexIndices.clear( );
		faces.clear( );
		normals.clear( );
		vertexElements.clear( );
		normalsElements.clear( );

		delete[ ] tempV;
		tempV = nullptr;

		delete[ ] tempUV;
		tempUV = nullptr;

		delete[ ] tempN;
		tempN = nullptr;
	}
}

GameObject::GameObject( const std::string& colliderFile ) : mModel( )
{
	DAEFileParser parser( colliderFile.c_str( ) );
	int colliderMeshCount = parser.getMeshCount( );
	mColliders.reserve( colliderMeshCount );

	int indexBufferSize;

	std::vector< Vertex > vertices;
	std::vector< float > vertexElements;
	std::vector< int > faceVertexIndices;
	std::vector< int > normalsVertexIndices;
	std::vector< Face > faces;
	std::vector< glm::vec3 > normals;
	std::vector< float > normalsElements;

	int* tempV;
	int* tempN;
	int* tempUV;

	for( int i = 0; i < colliderMeshCount; i++ )
	{
		parser.initVertexElements( vertexElements, i );
		parser.initNormalArray( normalsElements, i );
		parser.initIndexArrays( &tempV, &tempN, &tempUV, i, &indexBufferSize );

		vertices.reserve( vertexElements.size( ) / 3 );

		// init vertices
		for( unsigned int vertexIndex = 0; vertexIndex < vertexElements.size( ); vertexIndex += 3 )
		{
			vertices.emplace_back( vertexElements[ vertexIndex ],
				vertexElements[ vertexIndex + 2 ], vertexElements[ vertexIndex + 1 ] );
		}

		// init indices
		faceVertexIndices.reserve( indexBufferSize );
		for( int j = 0; j < indexBufferSize; j++ )
		{
			faceVertexIndices.emplace_back( tempV[ j ] );
			normalsVertexIndices.emplace_back( tempN[ j ] );
		}

		// init normals
		normals.reserve( normalsElements.size( ) / 3 );

		for( unsigned int j = 0; j < normalsElements.size( ); j += 3 )
		{
			normals.emplace_back( normalsElements[ j ], normalsElements[ j + 2 ], normalsElements[ j + 1 ] ); // Swap X and Y
		}

		//Init Face Array
		faces.reserve( indexBufferSize );
		std::vector< int > faceIndices;
		faceIndices.reserve( 3 );

		for( int j = 0; j < indexBufferSize; j += 3 )
		{
			faceIndices.emplace_back( tempV[ j ] );
			faceIndices.emplace_back( tempV[ j + 1 ] );
			faceIndices.emplace_back( tempV[ j + 2 ] );

			// The Constructor used to make Colliders for Game Objects sets the Face Vertex vector, don't need to set it here
			faces.emplace_back( nullptr, faceIndices );
			faceIndices.clear( );
		}

		mColliders.emplace_back( vertices, tempV, tempN, faces, normals, calculateCentroid( vertices ), colliderFile );

		vertices.clear( );
		faceVertexIndices.clear( );
		faces.clear( );
		normals.clear( );
		vertexElements.clear( );
		normalsElements.clear( );

		delete[ ] tempV;
		tempV = nullptr;

		delete[ ] tempUV;
		tempUV = nullptr;

		delete[ ] tempN;
		tempN = nullptr;
	}
}

GameObject::GameObject( const std::vector<Block>& colliders ) : mModel( )
{
	mColliders.reserve( colliders.size( ) );

	for( const Block& b : colliders )
		mColliders.emplace_back( b );
}


#include "Block.h"



#define VECTOR_EQUALS( a, b ) glm::abs( a.x - b.x ) + glm::abs( a.y - b.y ) + glm::abs( a.z - b.z ) < 0.001f ? true : false
#define SIMILAR_DIRECTION( a, b ) glm::dot( a, b ) >= 0 ? true : false
#define DIFFERENT_DIRECTION( a, b ) glm::dot( a, b ) < 0 ? true : false

glm::vec3 Vertex::getUntransformedCoordinate( const glm::mat4& translation, const Quaternion& rotation, const glm::mat4& scale ) const
{
	Quaternion inv = rotation.getInverse( );
	glm::vec4 coord ( mCoordinate, 1.0f );

	coord = glm::inverse( translation ) * coord;
	coord = inv.toMat4( ) * coord;
	coord = glm::inverse( scale ) * coord;

	return coord;
}

void Vertex::print( void ) const
{
	std::cout << "Coordinate: ( " << mCoordinate.x << ", " << mCoordinate.y << ", " << mCoordinate.z << " )\n";
}

void Vertex::mat4Multiply( const glm::mat4& mat )
{
	mCoordinate = mat * glm::vec4( mCoordinate, 1.0f );
}

void Vertex::quaternionRotate( const Quaternion& q )
{
	mCoordinate = q.rotate( glm::vec4( mCoordinate, 1.0f ) );
}

Vertex::Vertex( void )
	: mCoordinate( ),
	  mNormal( )
{
}
Vertex::Vertex( float X, float Y, float Z ) :
	mCoordinate( X, Y, Z ),
	mNormal( )
{
}
Vertex::Vertex( const Vertex& vert )
	: mCoordinate( vert.mCoordinate ),
	  mNormal( vert.mNormal )
{
}
Vertex::Vertex( const glm::vec3& vec )
	: mCoordinate( glm::vec4( vec, 1.0f ) ),
	  mNormal( glm::vec4( ) )
{

}
Vertex::Vertex( const glm::vec4& vec )
	: mCoordinate( vec ),
	  mNormal( glm::vec4( ) )
{

}
Vertex::~Vertex( )
{
}

void const Face::printVertices( ) const
{
	std::cout << "\nFace Vertices: \n";
	std::vector< Vertex > v = *( mVertices );
	for( int i = 0; i < 3; i++ )
		mVertices->at( mIndices[ i ] ).print( );
}

void const Face::printNormal( ) const
{
	glm::vec3 n = getNormal( );

	std::cout << "\nNormal: \n  ( " << n.x << ", " << n.y << ", " << n.z << " )\n";
}

Face Face::operator=( const Face& that ) 
{
	mVertices = that.mVertices;
	mIndices[ 0 ] = that.mIndices[ 0 ];
	mIndices[ 1 ] = that.mIndices[ 1 ];
	mIndices[ 2 ] = that.mIndices[ 2 ];
	//mSpherePtr = that.mSpherePtr;

	return *this;
}

glm::vec3 Face::getNormal( void ) const
{
	glm::vec3 p0, p1, p2;
	p0 = mVertices->at( mIndices[ 0 ] ).getCoordinateVec3( );
	p1 = mVertices->at( mIndices[ 1 ] ).getCoordinateVec3( );
	p2 = mVertices->at( mIndices[ 2 ] ).getCoordinateVec3( );

	return glm::normalize(  glm::cross( p0 - p1, p2 - p1 ) );
}

glm::vec3 Face::getCentroidVec3( void ) const
{
	glm::vec3 p1, p2, p3;

	p1 = mVertices->at( mIndices[ 0 ] ).getCoordinateVec3( );
	p2 = mVertices->at( mIndices[ 1 ] ).getCoordinateVec3( );
	p3 = mVertices->at( mIndices[ 2 ] ).getCoordinateVec3( );

	return glm::vec3( ( p1.x + p2.x + p3.x ) / 3.0f, ( p1.y + p2.y + p3.y ) / 3.0f, ( p1.z + p2.z + p3.z ) / 3.0f );
}
/*
void Face::deleteSphere( void )
{
	delete mSpherePtr;
	mSpherePtr = nullptr;
}

// TODO 7/3/2022: In block copy constructor, deleting this Face's spherePtr somehow deletes the other Block's Face's spherePtr. Shouldn't happen
void Face::memcpySphere( const Face& other )
{
	mSpherePtr = new BoundingSphere;
	memcpy( mSpherePtr, other.mSpherePtr, sizeof( BoundingSphere ) );
}
*/
Face::~Face( )
{
}

Face::Face( )
	: mVertices( ),
	  mIndices( ),
	  //mSpherePtr( nullptr ),
	  mTextureIndex( 0 )
{
}

//	mVertices is an array for all the Vertices of a block. This constructor 
//	inits the Face's array of indices to refer to only the Vertices comprising this face.
Face::Face( std::vector< Vertex >* vertices,std::vector< int > & indices ) :
	  mIndices( ),
	  mVertices( vertices ),
	  //mSpherePtr( nullptr ),
	  mTextureIndex( 0 )
{
	mIndices[ 0 ] = indices[ 0 ];
	mIndices[ 1 ] = indices[ 1 ];
	mIndices[ 2 ] = indices[ 2 ];
}

void BlockTransform::setTranslation( float x, float y, float z )
{
	mTranslation[ 3 ][ 0 ] = x;
	mTranslation[ 3 ][ 1 ] = y;
	mTranslation[ 3 ][ 2 ] = z;
}

void BlockTransform::setTranslation( glm::vec3& t )
{
	mTranslation[ 3 ][ 0 ] = t.x;
	mTranslation[ 3 ][ 1 ] = t.y;
	mTranslation[ 3 ][ 2 ] = t.z;
}

void BlockTransform::setTranslation( const glm::vec4& t )
{
	mTranslation[ 3 ][ 0 ] = t.x;
	mTranslation[ 3 ][ 1 ] = t.y;
	mTranslation[ 3 ][ 2 ] = t.z;
}


void BlockTransform::translateBy( glm::vec4& t )
{
	mTranslation[ 3 ][ 0 ] += t.x;
	mTranslation[ 3 ][ 1 ] += t.y;
	mTranslation[ 3 ][ 2 ] += t.z;
}

void BlockTransform::translateBy( glm::vec3& t )
{
	mTranslation[ 3 ][ 0 ] += t.x;
	mTranslation[ 3 ][ 1 ] += t.y;
	mTranslation[ 3 ][ 2 ] += t.z;
}

void BlockTransform::translateBy( float x, float y, float z )
{
	mTranslation[ 3 ][ 0 ] += x;
	mTranslation[ 3 ][ 1 ] += y;
	mTranslation[ 3 ][ 2 ] += z;
}

void BlockTransform::setRotation( const Quaternion& q )
{
	mRotation = q;
}

void BlockTransform::rotateBy( const Quaternion& q )
{
	//TODO: Write method to compose q with mRotation 2/21/2020
	//Current iteration needs bugtesting 3/7/2020
	mRotation = mRotation * q;
}

void BlockTransform::rotateAroundPoint( const Quaternion& q, const glm::vec3& point )
{
	glm::vec3 blockPosition( mTranslation[ 3 ][ 0 ], mTranslation[ 3 ][ 1 ], mTranslation[ 3 ][ 2 ] );
	glm::vec3 blockTranslation = blockPosition;
	blockTranslation -= point;
	q.rotate( blockTranslation );
	blockTranslation += point;
	blockTranslation -= blockPosition;

	translateBy( blockTranslation );
	rotateBy( q );
}

void BlockTransform::setScale( const float factor )
{
	mScale[ 0 ][ 0 ] = factor;
	mScale[ 1 ][ 1 ] = factor;
	mScale[ 2 ][ 2 ] = factor;
}

//You can apply transformations to the Block Individually or all at once with the next 4 functions.
void BlockTransform::applyTransform( Vertex* vertices, int vertexCount ) const
{
	for( int i = 0; i < vertexCount; i++ )
	{
		//TODO: Apply Rotation Transformation to mNormal  2/9/2020
		vertices[ i ].mat4Multiply( mScale );

		//	2/20/21: Possible bug in Hamiltonian product function
		//vertices[ i ].setCoordinate( mRotation.rotate( vertices[ i ].getCoordinate( ) ) );

		vertices[ i ].mat4Multiply( mRotation.toMat4( ) );
		vertices[ i ].mat4Multiply( mTranslation );
	}
}

void BlockTransform::applyTransform( Vertex* vertices, int vertexCount, const Quaternion& q ) const
{
	for( int i = 0; i < vertexCount; i++ )
	{
		//TODO: Apply Rotation Transformation to mNormal  2/9/2020
		vertices[ i ].mat4Multiply( mTranslation );
		vertices[ i ].setCoordinate( ( mRotation * q ).rotate( glm::vec4( vertices[ i ].getCoordinate( ), 1.0f ) ) );
		vertices[ i ].mat4Multiply( mScale );
	}
}

void BlockTransform::applyTranslation( Vertex* vertices, int vertexCount ) const
{
	for( int i = 0; i < vertexCount; i++ )
		vertices[ i ].mat4Multiply( mTranslation );
}

void BlockTransform::applyRotation( Vertex* vertices, int vertexCount ) const
{
	for( int i = 0; i < vertexCount; i++ )
	{
		//TODO: Apply Rotation Transformation to mNormal  2/9/2020
		vertices[ i ].quaternionRotate( mRotation );
	}
}

void BlockTransform::unTransform( std::vector< Vertex >& vertices, std::vector< Face >& faces ) const
{
	Quaternion inv = mRotation.getInverse( );
	glm::vec3 axis = inv.getAxis( );
	float angle = inv.getAngle( );

	for( unsigned int i = 0; i < vertices.size( ); i++ )
	{
		vertices[ i ].mat4Multiply( glm::inverse( mTranslation ) );	
		vertices[ i ].mat4Multiply( inv.toMat4( ) );
		vertices[ i ].mat4Multiply( glm::inverse( mScale ) );
	}
}

glm::mat4 BlockTransform::getUnTransform( void ) const
{
	return glm::inverse( mTranslation ) * mRotation.getInverse( ).toMat4( ) * glm::inverse( mScale );
}

void BlockTransform::applyScale( Vertex* vertices, int vertexCount ) const
{
	for( int i = 0; i < vertexCount; i++ )
		vertices[ i ].mat4Multiply( mScale );
}


BlockTransform::BlockTransform( )
	: mTranslation( glm::mat4( 1.0f ) ),
	mRotation( Quaternion( ) ),
	mScale( glm::mat4( 1.0f ) )
{
}

BlockTransform::BlockTransform( glm::mat4& t, Quaternion& r, glm::mat4& s )
	: mTranslation( t ),
	  mRotation( r ),
	  mScale( s )
{
}

BlockTransform::BlockTransform( const BlockTransform& other ) :
	mTranslation( other.mTranslation ),
	mRotation( other.mRotation ),
	mScale( other.mScale )
{

}

void printArr( float* arr, int size )
{
	std::cout << std::endl;
	for( int i = 0; i < size; i++ )
	{
		std::cout << arr[ i ] << " ";
	}
}

Block Block::operator=( const Block& other )
{
	return Block( other );
}

void const Block::printVertices( ) const
{
std::cout << "\nAll Vertices: \n";
for( int i = 0; i < mVertexCount; i++ )
	mVertices[ i ].print( );
}

void const Block::printVerticesWithNormals( void ) const
{
	glm::vec3 n;
	std::cout << "\nAll Vertices: \n";
	for( int i = 0; i < mVertexCount; i++ )
	{
		mVertices[ i ].print( );

		n = mVertices[ i ].getNormal( );
		std::cout << "	Normal: ( " << n.x << ", " << n.y << ", " << n.z << " )\n";
	}
}

void Block::rotateAroundPoint( const Quaternion& q, const glm::vec3& point, const Quaternion& originalRotation )
{
	glm::vec3 blockTranslation = mTransform.getTranslationVec3( );

	blockTranslation -= point;
	q.rotate( blockTranslation );
	blockTranslation += point;

	mTransform.setTranslation( blockTranslation );
	mTransform.setRotation( originalRotation * q );

	//mTransform.rotateAroundPoint( q, point );
}

void Block::setGround( const Block* b )
{
	mGround = b;

	//mPhys will have its grounded state set appropriately in the updatePhysics function.
}

void Block::setMoveDirection( const glm::vec3& dir )
{
	*mMoveDirectionPtr = dir;
}

void Block::setFacets( )
{
	mFacets.clear( );
	mFacets.reserve( mFaceCount / 2 );// Just guessing that on avg most facets will be quads
	bool unique;
	glm::vec3 faceNormal;
	glm::vec3 vertNormal;
	glm::vec3 centroid;
	const int* faceIndices;
	std::vector< int > indices;
	std::vector< int > zeroToPi;
	std::vector< int > piToTwoPi;
	indices.reserve( mFaces.size( ) );
	glm::vec3 prev;
	glm::vec3 next;
	for( unsigned int i = 0; i < mFaces.size( ); i++ )
		indices.emplace_back( i );

	for( unsigned int i = 0; i < mFaces.size( ); i++ )
	{
		int index = indices[ i ];

		if( index != -1 )
		{
			faceNormal = mFaces[ i ].getNormal( );
			faceIndices = mFaces[ index ].getIndices( );

			std::vector< int > hull;
			std::vector< int > hullWithRedundant;
			hullWithRedundant.reserve( 4 ); // Just guessing that on avg most facets will be quads
			for( int j = 0; j < 3; j++ )
			{
				if( !( VECTOR_EQUALS( faceNormal, mVertices[ faceIndices[ j ] ].getNormal( ) ) ) )
					hullWithRedundant.emplace_back( faceIndices[ j ] );
			}

			for( unsigned int j = i + 1; j < mFaces.size( ); j++ )
			{
				if( VECTOR_EQUALS( faceNormal, mFaces[ j ].getNormal( ) ) )
				{
					indices[ j ] = -1;
					faceIndices = mFaces[ j ].getIndices( );

					for( unsigned int k = 0; k < 3; k++ )
					{
						if( !( VECTOR_EQUALS( faceNormal, mVertices[ faceIndices[ k ] ].getNormal( ) ) ) )
						{
							unique = true;
							for( int l : hullWithRedundant )
							{
								if( l == faceIndices[ k ] )
								{
									unique = false;
									break;
								}
							}
							if( unique )
								hullWithRedundant.emplace_back( faceIndices[ k ] );	
						}
					}
				}
			}
			if( hullWithRedundant.size( ) == 0 )
				setPlaneFacet( ); // Block is a single Plane, needs special function to define Facet
			else
			{
				// Sort the rest of hull going clockwise from hull[ 0 ].
				vertNormal = mVertices[ hullWithRedundant[ 0 ] ].getNormal( );

				centroid = glm::vec3( );
				for( int j : hullWithRedundant )
					centroid += mVertices[ j ].getCoordinateVec3( );
				centroid.x /= hullWithRedundant.size( );
				centroid.y /= hullWithRedundant.size( );
				centroid.z /= hullWithRedundant.size( );

				// fill vectors with range [0-pi] and range (pi-2Pi) angle from hull[ 0 ]
				zeroToPi.clear( );
				piToTwoPi.clear( );
				zeroToPi.reserve( hullWithRedundant.size( ) / 2 );
				zeroToPi.emplace_back( hullWithRedundant[ 0 ] );
				piToTwoPi.reserve( hullWithRedundant.size( ) / 2 );
				for( unsigned int j = 1; j < hullWithRedundant.size( ); j++ )
				{
					if( DIFFERENT_DIRECTION( faceNormal, glm::cross( mVertices[ hullWithRedundant[ 0 ] ].getCoordinateVec3( ) - centroid, mVertices[ hullWithRedundant[ j ] ].getCoordinateVec3( ) - centroid ) ) )
						zeroToPi.emplace_back( hullWithRedundant[ j ] );
					else
						piToTwoPi.emplace_back( hullWithRedundant[ j ] );
				}
				std::sort( zeroToPi.begin( ), zeroToPi.end( ), [&]( const int lhs, const int rhs )
				{
					return ( DIFFERENT_DIRECTION( faceNormal, glm::cross( mVertices.at( lhs ).getCoordinateVec3( ) - centroid, mVertices.at( rhs ).getCoordinateVec3( ) - centroid ) ) );
				} );
				std::sort( piToTwoPi.begin( ), piToTwoPi.end( ), [&]( const int lhs, const int rhs )
				{
					return ( DIFFERENT_DIRECTION( faceNormal, glm::cross( mVertices.at( lhs ).getCoordinateVec3( ) - centroid, mVertices.at( rhs ).getCoordinateVec3( ) - centroid ) ) );
				} );
				for( unsigned int j = 0; j < hullWithRedundant.size( ); j++ )
				{
					j < zeroToPi.size( ) ? hullWithRedundant[ j ] = zeroToPi[ j ] : hullWithRedundant[ j ] = piToTwoPi[ j - zeroToPi.size( ) ];
				}

				// Keep Redundant points on an edge out of final vector
				hull.reserve( hullWithRedundant.size( ) );
				for( unsigned int j = 0; j < hullWithRedundant.size( ); j++ )
				{
					if( j != 0 )
						prev = mVertices[ hullWithRedundant[ j - 1 ] ].getCoordinateVec3( );
					else
						prev = mVertices[ hullWithRedundant.back( ) ].getCoordinateVec3( );

					if( j != hullWithRedundant.size( ) - 1 )
						next = mVertices[ hullWithRedundant[ j + 1 ] ].getCoordinateVec3( );
					else
						next = mVertices[ hullWithRedundant.front( ) ].getCoordinateVec3( );

					if( !( VECTOR_EQUALS( glm::normalize( prev - mVertices[ hullWithRedundant[ j ] ].getCoordinateVec3( ) ), 
						glm::normalize( mVertices[ hullWithRedundant[ j ] ].getCoordinateVec3( ) - next ) ) ) )
						hull.emplace_back( hullWithRedundant[ j ] );
				}

				mFacets.emplace_back( faceNormal, hull, mVertices );
			}
		}
	}
}

void Block::setPlaneFacet( )
{
	// TODO 12/31/2022: implement function that defines the boundary for a Block that is a single plane. For now, just give mFacets an empty Facet
	mFacets.emplace_back( );
}

void Block::fillVertices( std::vector<glm::vec3>& verts ) const
{
	verts.reserve( mVertexCount );
	for( int i = 0; i < mVertexCount; i++ )
		verts.emplace_back( mVertices[ i ].getCoordinateVec3( ) );
}

void Block::applyTransformationMatrix( const glm::mat4 transform )
{
	for( int i = 0; i < mVertexCount; i++ )
		mVertices[ i ].mat4Multiply( transform );
}

void Block::updateTransform( const glm::vec4& t, const Quaternion& r, const float& s )
{
	glm::mat4 transform( 1.0f );

	glm::vec3 oldTranslation = mTransform.getTranslationVec3( );
	Quaternion oldRotation = mTransform.getRotation( );
	float oldScale = mTransform.getScale( )[ 0 ][ 0 ];

	applyUntransform( );

	mTransform.setTranslation( t.x, t.y, t.z );
	mTransform.setRotation( r );
	mTransform.setScale( s );

	transform *= mTransform.getTranslation( );
	transform *= r.toMat4( );
	transform *= mTransform.getScale( );


	for( int i = 0; i < mVertexCount; i++ )
	{
		mVertices[ i ].mat4Multiply( transform );
	}
	//update Bounding Volumes
	mSphere.setCenter( t );
	mSphere.calculateBounds( mVertices );
	mAABB.setCenter( t );
	mAABB.calculateBounds( mVertices );

	if( r != oldRotation )
	{
		//Need to rotate the Vertex Normals and Facet Normals
		glm::vec4 norm;
		for( int i = 0; i < mVertexCount; i++ )
		{
			norm = mVertices[ i ].getNormalVec4( );
			norm = oldRotation.getInverse( ).toMat4( ) * norm;
			norm = r.toMat4( ) * norm;
			mVertices[ i ].setNormal( norm );  
		}
		for( unsigned int i = 0; i < mFacets.size( ); i++ )
		{
			norm = glm::vec4( mFacets[ i ].getNormal( ), 1.0f );
			norm = oldRotation.getInverse( ).toMat4( ) * norm;
			norm = r.toMat4( ) * norm;
			mFacets[ i ].setNormal( norm );
		}
	}
}

void Block::updateTranslation( const glm::vec4& t )
{
	glm::mat4 transform( 1.0f );

	glm::vec3 oldTranslation = mTransform.getTranslationVec3( );

	applyUntransform( );

	mTransform.setTranslation( t.x, t.y, t.z );

	transform *= mTransform.getTranslation( );


	for( int i = 0; i < mVertexCount; i++ )
	{
		mVertices[ i ].mat4Multiply( transform );
	}

	mSphere.setCenter( mTransform.getTranslationVec3( ) );
	mAABB.setCenter( mTransform.getTranslationVec3( ) );
}

std::vector<int> Block::getIndices( void ) const
{
	const int* faceIndices;
	std::vector< int > totalIndices;
	totalIndices.reserve( mFaces.size( ) * 3 );

	for( unsigned int i = 0; i < mFaces.size( ); i++ )
	{
		faceIndices = mFaces[ i ].getIndices( );
		totalIndices.emplace_back( faceIndices[ 0 ] );
		totalIndices.emplace_back( faceIndices[ 1 ] );
		totalIndices.emplace_back( faceIndices[ 2 ] );
	}
	return totalIndices;
}

// n must be normalized by callee
const Facet* Block::getFacetPtrWithNormal( const glm::vec3& n ) const
{
	for( unsigned int i = 0; i < mFacets.size( ); i++ )
	{
		if( mFacets[ i ].matchesNormal( n ) )
			return &mFacets[ i ];
	}

	return nullptr;
}

void Block::applyUntransform( )
{
	mTransform.unTransform( mVertices, mFaces );

	mTransform.setTranslation( 0.0f, 0.0f, 0.0f );
	mTransform.setRotation( Quaternion( ) );
	mTransform.setScale( 1.0f );
}

void Block::updatePhysics( float deltaTime, bool grounded )
{
	if( !grounded )
	{
		mPhysicsPtr->incrFallSpeed( deltaTime );
		mPhysicsPtr->updateFallDistance( deltaTime );
		mPhysicsPtr->setGrounded( false );
	}
	else if( !mPhysicsPtr->isGrounded( ) )
	{
		mPhysicsPtr->setGrounded( true );
		mPhysicsPtr->resetFalling( );
	}

}

bool Block::isGrounded( void ) const
{
	if( mGround )
		return true;

	return false;
}

Block::~Block( )
{
	delete mMoveDirectionPtr;
	delete mPhysicsPtr;
	mSphere.deleteWireSphere( );
}

Block::Block( )
	: mVertices( ),
	  mSphere( ), mAABB( ),
	  mFaces( ),
	  mFaceCount( 0 ), 
	  mGround( nullptr ), mVertexCount( 0 ), mPhysicsPtr( nullptr ), mID( 0 ), mMoveDirectionPtr( nullptr ), mStatic( false ),
	  mUVIndices( ), mUVArray( ), mNormals( ), mNormalsCount( 0 ), mName( )
{
}

Block::Block( const char* filePath ) : mPhysicsPtr( new PhysicsProperties( ) ), mSphere( ), mAABB( ), mMaterials( ),
	mGround( nullptr ), mMoveDirectionPtr( new glm::vec3 ), mID( 0 ), mUVIndices( ), mUVArray( ), mNormals( ), mNormalsCount( 0 ), mStatic( false ), mName( filePath )
{
	// Parser inits the Vertex, Index, and Normals data from the DAE file of the 3d Model

  	DAEFileParser parser( filePath ); 
	std::vector< float > vertexElements;
	std::vector< float > normalsElements;
	std::queue< int > faceTexIndices;
	std::vector< int > vertexNormalsIndices;
	int vertexComponentCount;


	parser.initMaterials( mMaterials );
	parser.initVertexElements( vertexElements, 0 );
	parser.initUVArray( mUVArray, 0 );
	parser.initNormalArray( normalsElements, 0 );

	// Init Vertex Array
	vertexComponentCount = parser.getVertexComponentCount( );
	mVertexCount = vertexComponentCount / 3;
	mVertices.reserve( mVertexCount );

	// New 8/20/21: We're swapping the Y and Z to line up with Blender's coordinate system.
	for( int vertexIndex = 0; vertexIndex < vertexComponentCount; vertexIndex += 3 )
	{
		mVertices.emplace_back( vertexElements[ vertexIndex ],
			vertexElements[ vertexIndex + 2 ], vertexElements[ vertexIndex + 1 ] );
	}

	// Every set of triangles using a different material has its own <p> array in the DAE File
	std::vector< int* > tempVertInd;
	tempVertInd.reserve( mMaterials.size( ) );
	std::vector< int* > tempVertNormInd;
	tempVertNormInd.reserve( mMaterials.size( ) );
	std::vector< int* > tempUVInd;
	tempUVInd.reserve( mMaterials.size( ) );
	std::vector< int > arrLengths;
	arrLengths.reserve( mMaterials.size( ) );

	for( unsigned int i = 0; i < mMaterials.size( ); i++ )
	{
		int* tempV; 
		int* tempN;
		int* tempUV;
		int tempLen = parser.getIndexCount( ); // TODO 12/20/2022: Find out how parser determines indexCount, vertexElementCount, etc., and see if multiple meshes messes it up.
		parser.initIndexArrays( &tempV, &tempN, &tempUV, i, nullptr );

		tempVertInd.emplace_back( tempV );
		tempVertNormInd.emplace_back( tempN );
		tempUVInd.emplace_back( tempUV );
		arrLengths.emplace_back( parser.getIndexCount( ) - tempLen );
		faceTexIndices.push( parser.getIndexCount( ) - tempLen );
	}

	// init mIndices and mUVIndices
	std::vector< int > vertexIndices;
	vertexIndices.reserve( parser.getIndexCount( ) );
	mUVIndices.reserve( parser.getIndexCount( ) );

	for(unsigned int i = 0; i < mMaterials.size( ); i++ )
	{
		for( int j = 0; j < arrLengths.at( i ); j++ )
		{
			vertexIndices.emplace_back( tempVertInd.at( i )[ j ] );
			vertexNormalsIndices.emplace_back( tempVertNormInd.at( i )[ j ] );
			mUVIndices.emplace_back( tempUVInd.at( i )[ j ] );
		}

		delete[ ] tempVertInd.at( i );
		tempVertInd.at( i ) = nullptr;

		delete[ ] tempVertNormInd.at( i );
		tempVertNormInd.at( i ) = nullptr;

		delete[ ] tempUVInd.at( i );
		tempUVInd.at( i ) = nullptr;
	}
	
	// init mNormals
	mNormalsCount = ( int )normalsElements.size( ) / 3;
	mNormals.reserve(  mNormalsCount );

	for( int i = 0; i < mNormalsCount; i++ )
	{
		// Swap Y and Z 
		mNormals.emplace_back( normalsElements[ i * 3 ], normalsElements[ i * 3 + 2 ], normalsElements[ i * 3 + 1 ] );
	}
	
	//Init Face Array
	mFaceCount = parser.getIndexCount( ) / 3;
	mFaces.reserve( mFaceCount );
	std::vector< int > faceIndices;
	faceIndices.reserve( 3 );

	for( int i = 0, faceIndex = 0, uvIndexIndex = 0; i < mFaceCount; i++ )
	{
		faceIndices.emplace_back( vertexIndices[ faceIndex++ ] );
		faceIndices.emplace_back( vertexIndices[ faceIndex++ ] );
		faceIndices.emplace_back( vertexIndices[ faceIndex++ ] );

		mFaces.emplace_back( &mVertices, faceIndices );
		faceIndices.clear( );
	}

	// Init Face texIndices
	int texIndex = 0;
	int faceIndex = 0;
	while( !faceTexIndices.empty( ) )
	{
		int texCount = faceTexIndices.front( ) / 3;
		faceTexIndices.pop( );

		for( int i = 0; i < texCount; i++, faceIndex++ )
			mFaces[ faceIndex ].setTextureIndex( texIndex );

		texIndex++;
	}

	// Vertex Normals initialized here
	std::vector< std::vector< int > > normalsIndexArray;
	normalsIndexArray.reserve( mVertexCount );
	for( int i = 0; i < mVertexCount; i++ )
		normalsIndexArray.emplace_back( );
	
	int indexIntoVertices;
	int indexIntoNormals;
	bool unique;
	for( int i = 0; i < mFaceCount * 3; i++ )
	{
		indexIntoVertices = vertexIndices[ i ];
		indexIntoNormals = vertexNormalsIndices[ i ];

		unique = true;
		for( unsigned int j = 0; j < normalsIndexArray[ indexIntoVertices ].size( ); j++ )
		{
			if( VECTOR_EQUALS( mNormals[ indexIntoNormals ], mNormals[ normalsIndexArray[ indexIntoVertices ][ j ] ] ) )
			{
				unique = false;
				break;
			}
		}
		if( unique )
			normalsIndexArray[ indexIntoVertices ].emplace_back( indexIntoNormals );
	}
	for( int i = 0; i < mVertexCount; i++ )
	{
		for( unsigned int j = 0; j < normalsIndexArray[ i ].size( ); j++ )
		{
			mVertices[ i ].addToNormal( mNormals[ normalsIndexArray[ i ][ j ] ] );
		}
		mVertices[ i ].finalizeNormal( );
	}

	// Init BoundingSphere
	mSphere.calculateBounds( mVertices );

	// init Bounding Box
	mAABB.calculateBounds( mVertices );

	// Only need Facets if it's a collider; colliders don't use materials, or have default material
	if( mMaterials.size( ) == 0 || ( mMaterials.size( ) == 1 && mMaterials[ 0 ].getColor( ) == glm::vec4( 0.8f, 0.8f, 0.8f, 1.0f ) ) )
	{
		setFacets( );
		// DEBUG 12/31/2022
		for( unsigned int i = 0; i < mFacets.size( ); i++ )
			mFacets[ i ].printIncorrect( mVertices );
	}
}

Block::Block( const Block& other ) :

	mVertices( other.mVertices ),
	mFaces( other.mFaces ),
	mUVIndices( other.mUVIndices ),
	mUVArray( other.mUVArray ),
	mNormals( other.mNormals ),

	mID( other.mID ),

	mTransform( other.mTransform ),

	mVertexCount( other.mVertexCount ),
	mFaceCount( other.mFaceCount ),
	mNormalsCount( other.mNormalsCount ),

	mStatic( other.mStatic ),	//The application is responsible for checking if a block is static before calling updateTransform.
	mGround( nullptr ),

	mSphere( other.mSphere ),
	mAABB( other.mAABB ),

	mMoveDirectionPtr( new glm::vec3( *( other.mMoveDirectionPtr ) ) ),
	mPhysicsPtr( new PhysicsProperties( ) ),

	mMaterials( other.mMaterials ),

	mFacets( ),
	mName( other.mName )
{
	for( int i = 0, faceIndex = 0, uvIndexIndex = 0; i < mFaceCount; i++ )
		mFaces[ i ].setVertices( &mVertices );

	for( Facet f : other.mFacets )
		mFacets.emplace_back( f.getNormal( ), f.getHull( ), mVertices );
}

// 12/20/2022: This constructor will only be used to make collider blocks, don't need mMaterials.

Block::Block( const std::vector< Vertex >& vertices, const int* tempV, const int* tempN, const std::vector< Face >& faces, 
			  const std::vector< glm::vec3 >& normals, const glm::vec3& blockCenter, const std::string& filePath ) :

	mVertices( vertices ),
	mFaces( faces ),
	mNormals( normals ),
	mUVIndices( ),
	mUVArray( ),
	mMaterials( ),

	mID( 0 ),

	mTransform( BlockTransform( ) ),

	mVertexCount( vertices.size( ) ),
	mFaceCount( faces.size( ) ),
	mNormalsCount( normals.size( ) ),

	mStatic( true ),	//The application is responsible for checking if a block is static before calling updateTransform.
	mGround( nullptr ),

	mSphere( BoundingSphere( ) ),
	mAABB( AABoundingBox( ) ),

	mMoveDirectionPtr( new glm::vec3( ) ),
	mPhysicsPtr( new PhysicsProperties( ) ),

	mName( filePath )
{
	for( int i = 0, faceIndex = 0, uvIndexIndex = 0; i < mFaceCount; i++ )
		mFaces[ i ].setVertices( &mVertices );

	// Vertex Normals initialized here
	std::vector< std::vector< int > > normalsIndexArray;
	normalsIndexArray.reserve( mVertexCount );
	for( int i = 0; i < mVertexCount; i++ )
		normalsIndexArray.emplace_back( );

	int indexIntoVertices;
	int indexIntoNormals;
	bool unique;
	for( int i = 0; i < mFaceCount * 3; i++ )
	{
		indexIntoVertices = tempV[ i ];
		indexIntoNormals = tempN[ i ];

		unique = true;
		for( unsigned int j = 0; j < normalsIndexArray[ indexIntoVertices ].size( ); j++ )
		{
			if( VECTOR_EQUALS( mNormals[ indexIntoNormals ], mNormals[ normalsIndexArray[ indexIntoVertices ][ j ] ] ) )
			{
				unique = false;
				break;
			}
		}
		if( unique )
			normalsIndexArray[ indexIntoVertices ].emplace_back( indexIntoNormals );
	}
	for( int i = 0; i < mVertexCount; i++ )
	{
		for( unsigned int j = 0; j < normalsIndexArray[ i ].size( ); j++ )
		{
			mVertices[ i ].addToNormal( mNormals[ normalsIndexArray[ i ][ j ] ] );
		}
		mVertices[ i ].finalizeNormal( );
	}

	mSphere.setCenter( blockCenter );
	mSphere.calculateBounds( mVertices );
	mAABB.setCenter( blockCenter );
	mAABB.calculateBounds( mVertices );

	setFacets( );
}

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Util.h"

#include "FBXFileParser.h"
#include "DAEFileParser.h"

#include <memory>
#include <iostream>
#include <queue>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "Math/Quaternion.h"

#include "Block/PhysicsProperties.h"
#include "ProceduralGeometry/WireSphere.h"

class Vertex
{
public:

	void print( void ) const;

	inline void setCoordinate( const glm::vec3& v ) { mCoordinate = v; }
	inline void setNormal( const glm::vec3& n ) { mNormal = n; }
	
	inline const glm::vec3& getCoordinate( void ) const { return mCoordinate; }
	glm::vec3 getUntransformedCoordinate( const glm::mat4& translation, const Quaternion& rotation, const glm::mat4& scale ) const;
	inline glm::vec3 getCoordinateVec3( void ) const { return mCoordinate;  } 
	inline const glm::vec3& getNormal( void ) const { return mNormal; }
	inline glm::vec4 getNormalVec4( void ) const { return glm::vec4( mNormal, 1.0f ); }

	// In Block Constructors, normalized normals are added to each Vertex, then the final result is normalized
	inline void addToNormal( const glm::vec3& n ) { mNormal += n; }
	inline void finalizeNormal( ) { mNormal = glm::normalize( mNormal ); }

	void mat4Multiply( const glm::mat4& mat );
	void quaternionRotate( const Quaternion& q );

	Vertex( void );
	Vertex( const Vertex& vert );
	Vertex( float X, float Y, float Z );
	Vertex( const glm::vec3& vec );
	Vertex( const glm::vec4& vec );
	~Vertex( );
protected:
	glm::vec3 mCoordinate;
	glm::vec3 mNormal;
};

//Every Block will have a Bounding Sphere as a first collision check.
struct BoundingSphere
{
public:
	inline const glm::vec3& getCenter( void ) const { return mCenter; }
	inline float getRadius( void ) const { return mRadius; }

	inline void setCenter( const glm::vec3& point ) { mCenter = point; }
	inline void setRadius( float rad ) { mRadius = rad; }

	bool isContacting( const BoundingSphere& other ) const
	{
		glm::vec3 distanceVec = mCenter - other.mCenter;
		float distanceSquared = distanceVec.x * distanceVec.x + distanceVec.y * distanceVec.y + distanceVec.z * distanceVec.z;
		float radiusSumSquared = ( mRadius + other.mRadius ) * ( mRadius + other.mRadius );

		if( distanceSquared <= radiusSumSquared )
			return true;
		else
			return false;
	}
	void calculateBounds( const std::vector< Vertex >& vertices )
	{
		float max = -INFINITY;
		float lengthSquared;
		glm::vec3 vert;
		for( unsigned int i = 0; i < vertices.size( ); i++ )
		{
			vert = vertices[ i ].getCoordinateVec3( ) - mCenter;
			lengthSquared = vert.x * vert.x + vert.y * vert.y + vert.z * vert.z;
			if( lengthSquared > max )
				max = lengthSquared;
		}
		mRadius = glm::sqrt( max );
	}
	void initWireSphere( )
	{
		if( !mWirePtr )
			mWirePtr = new WireSphere( mCenter, mRadius, 10.0f, 6, glm::vec4( 0, 0, 0, 1 ) );
	}
	void deleteWireSphere( )
	{
		delete mWirePtr;
		mWirePtr = nullptr;
	}
	void drawWireSphere( const glm::mat4& viewTimewProj ) const
	{
		if( mWirePtr )
			mWirePtr->draw( viewTimewProj );
	}

	BoundingSphere( const glm::vec3& center, float rad ) : mCenter( center ), mRadius( rad ), mWirePtr( nullptr )
	{
	}
	BoundingSphere( ) : mCenter( glm::vec3( ) ), mRadius( 0 ), mWirePtr( nullptr )
	{
	}
	BoundingSphere( const BoundingSphere& other ) : mCenter( other.mCenter ), mRadius( other.mRadius ), mWirePtr( nullptr )
	{
	}
	~BoundingSphere( )
	{
		delete mWirePtr;
		mWirePtr = nullptr;
	}
protected:
	glm::vec3 mCenter;
	float mRadius;
	WireSphere* mWirePtr;
};

struct AABoundingBox
{
public:

	inline const glm::vec3& getCenter( ) const { return mCenter; }
	inline const glm::vec3& getMax( ) const { return mMax; }
	inline const glm::vec3& getMin( ) const { return mMin; }

	inline void setCenter( const glm::vec3& c ) { mCenter = c; }

	void calculateBounds( const std::vector< Vertex >& vertices )
	{
		mMax = glm::vec3( -INFINITY, -INFINITY, -INFINITY );
		mMin = glm::vec3( INFINITY, INFINITY, INFINITY );

		glm::vec3 v;
		for( unsigned int i = 0; i < vertices.size( ); i++ )
		{
			v = vertices[ i ].getCoordinateVec3( );

			if( v.x > mMax.x )
				mMax.x = v.x;
			if( v.y > mMax.y )
				mMax.y = v.y;
			if( v.z > mMax.z )
				mMax.z = v.z;

			if( v.x < mMin.x )
				mMin.x = v.x;
			if( v.y < mMin.y )
				mMin.y = v.y;
			if( v.z < mMin.z )
				mMin.z = v.z;
		}
		mMax -= mCenter;
		mMin -= mCenter;
	}

	AABoundingBox( const std::vector< Vertex >& vertices, const glm::vec3& center ) : mCenter( center ), mMax( ), mMin( )
	{
		calculateBounds( vertices );
	}
	AABoundingBox( ) : mCenter( ), mMax( ), mMin( )
	{
	}
	AABoundingBox( const AABoundingBox& other ) : mCenter( other.mCenter ), mMax( other.mMax ), mMin( other.mMin )
	{
	}
protected:
	glm::vec3 mCenter;
	glm::vec3 mMax;
	glm::vec3 mMin;

};

class Face
{
public:
	Face operator=( const Face& that );
	void const printVertices( void ) const;
	void const printNormal( void ) const;

	inline void setTextureIndex( int texInd ) { mTextureIndex = texInd; }
	inline void setVertices( const std::vector< Vertex >* v ) { mVertices = v; }

	glm::vec3 getNormal( void ) const;
	inline const int* getIndices( void ) const { return &mIndices[ 0 ]; }
	inline const Vertex& getVertexAt( int index ) const { return mVertices->at( mIndices[ index ] ); }
	inline glm::vec3 getVec3At( int index ) const { return mVertices->at( mIndices[ index ] ).getCoordinateVec3( ); }
	glm::vec3 getCentroidVec3( void ) const;
	inline int getTextureIndex( ) const { return mTextureIndex; }

	~Face( );
	Face( std::vector< Vertex >* vertices, std::vector< int >& indices );
	Face( );
protected:
	// mIndices describes the Vertices in Block's mVertices buffer that make up this face
	const std::vector< Vertex >* mVertices;
	int mIndices[ 3 ];
	int mTextureIndex;
};

//Every Block will have a BlockTransform, and is responsible for using it to change its vertex data
struct BlockTransform
{
public:
	void setTranslation( float x, float y, float z );
	void setTranslation( glm::vec3& t );
	void setTranslation( const glm::vec4& t );

	void translateBy( glm::vec3& t );
	void translateBy( glm::vec4& t );
	void translateBy( float x, float y, float z );

	void setRotation( const Quaternion& q );
	void rotateBy( const Quaternion& q );
	void rotateAroundPoint( const Quaternion& q, const glm::vec3& point );

	void setScale( const float factor );

	inline const glm::mat4& getTranslation( void ) const { return mTranslation; }
	inline const glm::vec4& getTranslationVec4( void ) const { return glm::vec4( mTranslation[ 3 ][ 0 ], 
		mTranslation[ 3 ][ 1 ], mTranslation[ 3 ][ 2 ], mTranslation[ 3 ][ 3 ] ); }
	inline const glm::vec3 getTranslationVec3( void ) const { return glm::vec3( mTranslation[ 3 ][ 0 ], mTranslation[ 3 ][ 1 ], mTranslation[ 3 ][ 2 ] ); }
	inline const Quaternion& getRotation( void ) const { return mRotation; }
	inline const glm::mat4& getScale( void ) const { return mScale; }
	inline float getScaleFactor( void ) const { return mScale[ 0 ][ 0 ]; }	//Only uniform scaling allowed for now

	//An Apply Transform function without any Matrices or Quaternions in the argument just applies the block's transform.
	//A function with a Matrix or Quaternion applies the block's transform and composes the transform in the argument.

	void applyTransform( Vertex* vertices, int vertexCount ) const;
	void applyTransform( Vertex* vertices, int vertexCount, const Quaternion& q ) const;
	void applyTranslation( Vertex* vertices, int vertexCount ) const;
	void applyRotation( Vertex* vertices, int vertexCount ) const;
	void applyScale( Vertex* vertices, int vertexCount ) const;

	void unTransform( std::vector< Vertex >& vertices, std::vector< Face >& faces ) const;
	glm::mat4 getUnTransform( void ) const;

	inline glm::mat4 getCombinedTransform( void ) const { return mTranslation * mRotation.toMat4() * mScale; }

	BlockTransform( );
	BlockTransform( glm::mat4& t, Quaternion& r, glm::mat4& s );
	BlockTransform( const BlockTransform& other );

	//Translation, Rotation, Scale
protected:
	glm::mat4 mTranslation;
	Quaternion mRotation;
	glm::mat4 mScale;
};

struct Facet
{
public:
	inline const std::vector< int >& getHull( ) const { return mConvexHull; }
	inline const glm::vec3& getNormal( ) const { return mNormal; }
	inline int getIndexAt( int i ) const { return mConvexHull[ i ]; }
	inline const glm::vec3& getCoordAt( int index ) const { return verticesRef.at( mConvexHull[ index ] ).getCoordinate( ); }

	inline void setNormal( const glm::vec3& n ) { mNormal = n; }
	inline bool matchesNormal( const glm::vec3& n ) const { return glm::abs( mNormal.x - n.x ) + glm::abs( mNormal.y - n.y ) + glm::abs( mNormal.z - n.z ) < 0.0001f; }

	bool isOver( const glm::vec3& point, glm::vec3* planeToPoint, const glm::vec3& offset ) const
	{
		int maxInd = -1;
		float max = -INFINITY;
		float tempMax;
		glm::vec3 proj = verticesRef.at( mConvexHull[ 0 ] ).getCoordinate( ) + offset - point;
		proj = point + glm::dot( mNormal, proj ) * mNormal;

		glm::vec3 edgeNormal;
		glm::vec3 next;

		const uint32_t size = mConvexHull.size( );
		for( uint32_t i = 0; i < size; i++ )
		{
			next = verticesRef.at( mConvexHull[ ( i + 1 ) % size ] ).getCoordinate( ) + offset;
			edgeNormal = next - ( verticesRef.at( mConvexHull[ i ] ).getCoordinate( ) + offset );
			edgeNormal = glm::cross( mNormal, edgeNormal );
			tempMax = glm::dot( edgeNormal, proj - next );
			if( tempMax > 0 && tempMax > max )
			{
				maxInd = i;
				max = tempMax;
			}
		}
		if( maxInd == -1 )
		{
			if( planeToPoint )
				*planeToPoint = point - proj;
			
			return true;
		}
		if( planeToPoint )
		{
			glm::vec3 prev = verticesRef.at( mConvexHull[ maxInd ] ).getCoordinate( ) + offset;
			next = verticesRef.at( mConvexHull[ ( maxInd + 1 ) % size ] ).getCoordinate( ) + offset;

			if( glm::dot( next - prev, proj - prev ) < 0.0f )       // closest to prev
				*planeToPoint = point - prev;

			else if( glm::dot( prev - next, proj - next ) < 0.0f )  // closest to next
				*planeToPoint = point - next;
			else                                                    // closest to edgeProj
			{
				edgeNormal = next - prev;
				edgeNormal = glm::normalize( glm::cross( mNormal, edgeNormal ) );
				proj = proj + glm::dot( prev - proj, edgeNormal ) * edgeNormal;

				*planeToPoint = point - proj;
			}
		}
		return false;
	}

	bool rayIntersects( const glm::vec3& rayDir, const glm::vec3& rayPoint, glm::vec3* hit, float* hitDistance ) const
	{
		if( glm::dot( rayDir, mNormal ) >= 0.0f )
			return false;

		glm::vec3 rayDirNorm = glm::normalize( rayDir );
		float dist = glm::dot( getCoordAt( 0 ) - rayPoint, mNormal ) / glm::dot( rayDirNorm, mNormal );
		if( dist < 0.0f )
			return false;

		glm::vec3 planeHit = rayPoint + rayDirNorm * dist;
		glm::vec3 edgeNormal;
		glm::vec3 next;
		int maxInd = -1;
		float max = -INFINITY;

		const uint32_t size = mConvexHull.size( );
		for( uint32_t i = 0; i < size; i++ )
		{
			next = verticesRef.at( mConvexHull[ ( i + 1 ) % size ] ).getCoordinate( );
			edgeNormal = next - ( verticesRef.at( mConvexHull[ i ] ).getCoordinate( ) );
			edgeNormal = glm::cross( mNormal, edgeNormal );

			if( glm::dot( edgeNormal, planeHit - next ) > 0.0f )
				return false;
		}
		if( hit )
			*hit = planeHit;
		if( hitDistance )
			*hitDistance = dist;

		return true;

	}

	void printIncorrect( const std::vector< Vertex >& verts ) const
	{
		glm::vec3 centroid = glm::vec3( ); 
		glm::vec3 cross;

		for( int i : mConvexHull )
			centroid += verts[ i ].getCoordinateVec3( );
		centroid.x /= mConvexHull.size( );
		centroid.y /= mConvexHull.size( );
		centroid.z /= mConvexHull.size( );

		for( unsigned int i = 0; i < mConvexHull.size( ); i++ )
		{
			cross = glm::cross( verts[ mConvexHull[ i ] ].getCoordinateVec3( ) - centroid, verts[ mConvexHull[ ( i + 1 ) % mConvexHull.size( ) ] ].getCoordinateVec3( ) - centroid );

			if( glm::dot( mNormal, cross ) > 0 )
			{
				std::cout << "Incorrect order in Facet: " << mConvexHull[ i ] << ", " << mConvexHull[ ( i + 1 ) % mConvexHull.size( ) ] << " Should be swapped.\n";
			}
		}
	}

	Facet( const glm::vec3& n, const std::vector< int >& vec, const std::vector< Vertex >& ref ) :
		mNormal( n ), mConvexHull( vec ), verticesRef( ref )
	{
	}
	Facet( const Facet& f ) :
		mNormal( f.mNormal ), mConvexHull( f.mConvexHull ), verticesRef( f.verticesRef )
	{
	}
	Facet( ) :
		mNormal( ), mConvexHull( ), verticesRef( std::vector< Vertex >( ) )
	{
	}

protected:
	// Convex Hull contains the minimum number of points needed to define a Facet of a 3D Model
	std::vector< int > mConvexHull;
	glm::vec3 mNormal;
	const std::vector< Vertex >& verticesRef;
};

class Block
{
public:
	//void initVertexBufferColor( std::unique_ptr< VertexBuffer >& vb ) const;
	//void initVertexBufferTexture( std::unique_ptr< VertexBuffer >& vb, float* uvs, int* uvIndices ) const;
	//void updateVertexBuffer( void );

	Block operator=( const Block& other );

	void const printVertices( void ) const;
	void const printVerticesWithNormals( void ) const;
	void rotateAroundPoint( const Quaternion& q, const glm::vec3& point, const Quaternion& original );
	void setGround( const Block* b );
	void setMoveDirection( const glm::vec3& dir );
	inline void setStatic( bool s ) { mStatic = s; }
	inline void setSphereCenter( const glm::vec3& center ) { mSphere.setCenter( center ); }
	inline void setSphereRadius( float rad ) { mSphere.setRadius( rad ); }

	virtual void updateTransform( const glm::vec4& t, const Quaternion& r, const float& s );
	void updateTranslation( const glm::vec4& t );

	std::vector< int > getIndices( void ) const;
	const Facet* getFacetPtrWithNormal( const glm::vec3& n ) const;

	inline glm::vec3 getTranslation( void ) const { return mTransform.getTranslationVec3( ); }
	inline const Quaternion& getRotation( ) const { return mTransform.getRotation( ); }
	inline float getScaleFactor( ) const { return mTransform.getScaleFactor( ); }
	inline glm::vec3* getMoveDirectionPtr( ) const { return mMoveDirectionPtr; }
	inline const int getFaceCount( void ) const { return mFaceCount; }
	inline const int getVertexCount( void ) const { return mVertexCount; }
	inline Face getFaceAtIndex( int index ) const { return mFaces[ index ]; }
	inline const std::vector< Face >& getFaces( void ) const { return mFaces; }
	inline int getIndexCount( void ) const { return mFaceCount * 3; }
	inline const std::vector< Vertex >& getVertices( void ) const { return mVertices; }
	inline glm::vec3 getVertexVec3At( int i ) const { return mVertices[ i ].getCoordinateVec3( ); }
	inline glm::mat4 getCombinedTransform( void ) const { return mTransform.getCombinedTransform( ); }
	inline glm::mat4 getUnTransform( void ) const { return mTransform.getUnTransform( ); }
	inline glm::vec3 getFallTranslation( void ) const { return mPhysicsPtr->getFallTranslation( ); }
	inline const Block* getGround( void ) const { return mGround; }
	inline PhysicsProperties* getPhys( void ) const { return mPhysicsPtr; }
	inline bool isStatic( void ) const { return mStatic; }
	inline BoundingSphere getSphere( void ) const { return mSphere; }
	inline const BoundingSphere* getSphereAdr( void ) const { return &mSphere; }
	inline AABoundingBox getAABB( ) const { return mAABB; }
	inline const AABoundingBox* getAABBAdr( )const { return &mAABB; }
	inline std::vector< Material > getMaterialVec( void ) const { return mMaterials; }
	inline std::vector< float > getUVArray( void ) const { return mUVArray; }
	inline std::vector< int > getUVIndices( void ) const { return mUVIndices; }
	inline Material getMaterialAt( int i ) const { return mMaterials.at( i ); }
	inline const std::vector< Facet >& getFacets( ) const { return mFacets; }
	inline const Facet* getFacetPtrAt( int i ) const { return &mFacets[ i ]; }

	inline void initBoundarySphereWire( ) { mSphere.initWireSphere( ); }
	inline void deleteBoundarySphereWire( ) { mSphere.deleteWireSphere( ); }

	void fillVertices( std::vector< glm::vec3 >& verts ) const;

	void applyTransformationMatrix( const glm::mat4 transform );
	void applyUntransform( void );

	void updatePhysics( float deltaTime, bool grounded );
	bool isGrounded( void ) const;

	struct CSOFacet
	{
	protected:
		glm::vec3 mFacet[ 3 ];
		glm::vec3 mNormal;
		float mDistanceFromOriginSquared;
	public:
		inline const glm::vec3& getPoint( unsigned int n ) const { return mFacet[ n % 3 ]; }
		inline const glm::vec3& getNormal( ) const { return mNormal; }
		inline float getDistanceSquared( ) const { return mDistanceFromOriginSquared; }

		void setData( const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3 )
		{
			mFacet[ 0 ] = p1;
			mFacet[ 1 ] = p2;
			mFacet[ 2 ] = p3;

			mNormal = glm::cross( p2 - p1, p3 - p1 );
			if( glm::dot( p1, mNormal ) < 0 )
				mNormal = -mNormal;

			mNormal = glm::normalize( mNormal );

			mDistanceFromOriginSquared = glm::dot( ( glm::dot( mNormal, p1 ) / glm::dot( mNormal, mNormal ) ) * mNormal, 
												  ( glm::dot( mNormal, p1 ) / glm::dot( mNormal, mNormal ) ) * mNormal );
		}

		void translate( const glm::vec3& t )
		{
			mFacet[ 0 ] += t;
			mFacet[ 1 ] += t;
			mFacet[ 2 ] += t;

			mDistanceFromOriginSquared = glm::dot( ( glm::dot( mNormal, mFacet[ 0 ] ) / glm::dot( mNormal, mNormal ) ) * mNormal,
				( glm::dot( mNormal, mFacet[ 0 ] ) / glm::dot( mNormal, mNormal ) ) * mNormal );
		}

		void setNormal( glm::vec3& n )
		{
			mNormal = n;
		}

		void setDistanceSquared( float d )
		{
			mDistanceFromOriginSquared = d;
		}

		void operator= ( const CSOFacet& other )
		{
			mFacet[ 0 ] = other.mFacet[ 0 ];
			mFacet[ 1 ] = other.mFacet[ 1 ];
			mFacet[ 2 ] = other.mFacet[ 2 ];

			mNormal = other.mNormal;
			mDistanceFromOriginSquared = other.mDistanceFromOriginSquared;
		}

		CSOFacet( const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3 )
			: mFacet{ p1, p2, p3 }
		{
			mNormal = glm::cross( p2 - p1, p3 - p1 );
			if( glm::dot( p1, mNormal ) < 0 )
				mNormal = -mNormal;
			glm::vec3 distanceVec = ( glm::dot( mNormal, p1 ) / glm::dot( mNormal, mNormal ) ) * mNormal;
			mDistanceFromOriginSquared = glm::dot( distanceVec, distanceVec );
		}

		CSOFacet( const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& norm )
			: mFacet{ p1, p2, p3 }, mNormal( norm )
		{
			glm::vec3 distanceVec = ( glm::dot( mNormal, p1 ) / glm::dot( mNormal, mNormal ) ) * mNormal;
			mDistanceFromOriginSquared = glm::dot( distanceVec, distanceVec );
		}

		CSOFacet( glm::vec3* tri )
			: mFacet{ tri[ 0 ], tri[ 1 ], tri[ 2 ] }
		{
			mNormal = glm::cross( tri[ 1 ] - tri[ 0 ], tri[ 2 ] - tri[ 0 ] );
			if( glm::dot( tri[ 0 ], mNormal ) < 0 )
				mNormal = -mNormal;
			glm::vec3 distanceVec = ( glm::dot( mNormal, tri[ 0 ] ) / glm::dot( mNormal, mNormal ) ) * mNormal;
			mDistanceFromOriginSquared = glm::dot( distanceVec, distanceVec );
		}

		CSOFacet( ) : mFacet{ glm::vec3( ), glm::vec3( ), glm::vec3( ) }, mNormal( glm::vec3( ) ), mDistanceFromOriginSquared( 0.0f )
		{
		}
	};

	struct CSOQueueCompare
	{
		bool operator()( const CSOFacet& lhs, const CSOFacet& rhs )
		{
			return lhs.getDistanceSquared( ) > rhs.getDistanceSquared( );
		}

	};

	bool aboveTri( const glm::vec3& dir, const glm::vec3& triPoint, const glm::vec3& testPoint ) const	//Is triPoint further along dir than testPoint? Is the ratio of the squares of the two distances greater than 1.01?
	{
		if( glm::dot( testPoint - triPoint, dir ) < 0.0f )	//If the testPoint is far along in the opposite direction of dir, its distanceSquare will be greater than triPoint
			return false;									//even though it isn't further along dir. This check handles that case.

		glm::vec3 distanceVec;
		float d1, d2;

		distanceVec = ( glm::dot( dir, triPoint ) / glm::dot( dir, dir ) )* dir;
		d1 = glm::dot( distanceVec, distanceVec );
		distanceVec = ( glm::dot( dir, testPoint ) / glm::dot( dir, dir ) ) * dir;
		d2 = glm::dot( distanceVec, distanceVec );

		if( d1 >= 1.0f )
			return d2 / d1 > 1.05f;	//To be considered above the tri, the square of the testpoint's distance from the origin must be >5% of the tripoint's square of the distance from the origin

		else
		{
			if( d2 < 1.0f )
				return false;
		}
		return true;
	}

	bool containsOrigin( glm::vec3* simplex, int size  ) const
	{
		glm::vec3 triNormal;
		std::queue< int > indexQueue;
		int ind0, ind1, ind2, ind3;

		indexQueue.push( 0 );
		indexQueue.push( 1 );
		indexQueue.push( 2 );
		indexQueue.push( 3 );

		indexQueue.push( 1 );
		indexQueue.push( 2 );
		indexQueue.push( 3 );
		indexQueue.push( 0 );

		indexQueue.push( 0 );
		indexQueue.push( 1 );
		indexQueue.push( 3 );
		indexQueue.push( 2 );

		indexQueue.push( 0 );
		indexQueue.push( 2 );
		indexQueue.push( 3 );
		indexQueue.push( 1 );

		switch( size )
		{
		case 4:
			for( int i = 0; i < 4; i++ )	//In each iteration, ind0 -> ind2 are the triPoints and ind3 is the point outside the tri
			{
				ind0 = indexQueue.front( );
				indexQueue.pop( );
				ind1 = indexQueue.front( );
				indexQueue.pop( );
				ind2 = indexQueue.front( );
				indexQueue.pop( );
				ind3 = indexQueue.front( );
				indexQueue.pop( );

				triNormal = glm::cross( simplex[ ind0 ] - simplex[ ind1 ], simplex[ ind2 ] - simplex[ ind1 ] );

				if( glm::dot( simplex[ ind0 ], triNormal ) < 0.0f )
					triNormal = -triNormal;

				if( glm::dot( triNormal, simplex[ ind3 ] ) > 0.0f )
					return false;
			}
		}
		return true;
	}


	bool inTri( const glm::vec3& dir, const CSOFacet& facet )	//Suspicious, might need more planning.
	{
		glm::vec3 edgeNormal;

		if( glm::dot( dir, facet.getNormal( ) ) <= 0.0f )
			return false;

		for( int i = 0; i < 3; i++ )
		{
			edgeNormal = glm::cross( facet.getPoint( i ) - facet.getPoint( i + 1 ), facet.getNormal( ) );	//getPoint takes the index % 3, don't need to worry about outOfBounds
			if( glm::dot( edgeNormal, facet.getPoint( i + 2 ) ) > 0.0f )
				edgeNormal = -edgeNormal;

			if( glm::dot( edgeNormal, facet.getPoint( i ) - dir ) < 0.0f )
				return false;
		}
		return true;
	}

	//The line direction is dir, it passes through the origin ( 0, 0, 0 )
	float linePlaneIntersectDistance( const glm::vec3& dir, const CSOFacet& facet )
	{
		//Assuming the line contains 1 point of intersection, i.e. the line and plane are not parallel.
		float denom = glm::dot( dir, facet.getNormal( ) );
		if( denom )
			return ( glm::dot( facet.getPoint( 0 ), facet.getNormal( ) ) ) / ( denom );
		else
		{
			glm::vec3 projection = ( glm::dot( facet.getPoint( 0 ), dir ) / glm::dot( dir, dir ) ) * dir;
			return ( float ) projection.length( );
		}
	}

	bool doSimplex( glm::vec3* simplex, int* simplexIndex, glm::vec3* D ) const
	{
		float dot;
		glm::vec3 cross;
		glm::vec3 triNormal;

		glm::vec3 temp1;
		glm::vec3 temp2;
		glm::vec3 temp3;

		switch( *simplexIndex - 1 )
		{
		case 0:
			std::cout << "\nError: unexpected index of 0 in doSimplex function.\n";
			break;    //invalid index, the simplex should at least have 1 point in it already.

		//Line
		case 1: /* Origin is either closest to simplex[ 0 ] or the line segment between simplex[ 0 ] and simplex[ 1 ]  */

			//First check if origin is on the line
			if( simplex[ 0 ] == glm::vec3( 0, 0, 0 ) || simplex[ 1 ] == glm::vec3( 0, 0, 0 ) )
				return true;

			if( glm::normalize( simplex[ 0 ] ) == -glm::normalize( simplex[ 1 ] ) )
				return true;

			dot = glm::dot( -simplex[ 0 ], simplex[ 1 ] - simplex[ 0 ] );
			if( dot > 0 )
			{	//Origin is closest to the line

				cross = glm::cross( simplex[ 1 ] - simplex[ 0 ], -simplex[ 0 ] );
				cross = glm::cross( cross, simplex[ 1 ] - simplex[ 0 ] );
				*D = cross;      //Simplex is now a line, D is now a direction perpendicular to the simplex pointing toward the origin.
			}
			else 
			{	//Origin is closest to the point
				( *simplexIndex )--;
				*D = -simplex[ 0 ]; //Simplex is now a point, D points from the point to the origin.
			}
			
			return false;

		//Triangle
		case 2:
			triNormal = glm::cross( simplex[ 1 ] - simplex[ 2 ], simplex[ 0 ] - simplex[ 2 ] );

			//Start with line 20
			dot = glm::dot( -simplex[ 2 ], glm::cross( triNormal, simplex[ 0 ] - simplex[ 2 ] ) );
			if( dot > 0 )
			{
				//Origin is either closest to line 20 or point 2
				dot = glm::dot( -simplex[ 2 ], simplex[ 0 ] - simplex[ 2 ] );
				if( dot > 0 )
				{
					//closest to line 20

					cross = glm::cross( simplex[ 0 ] - simplex[ 2 ], -simplex[ 2 ] );
					cross = glm::cross( cross, simplex[ 0 ] - simplex[ 2 ] );
					*D = cross;   

					temp1 = simplex[ 0 ];
					temp2 = simplex[ 2 ];
					simplex[ 0 ] = temp1;
					simplex[ 1 ] = temp2;
					( *simplexIndex )--;	//Simplex is now a line, D is now a direction perpendicular to the simplex pointing toward the origin.
				}
				else
				{
					//closest to point 2
					simplex[ 0 ] = simplex[ 2 ];
					( *simplexIndex ) = 1;
					*D = -simplex[ 2 ];
				}
				return false;
			}

			//Next, line 21
			dot = glm::dot( -simplex[ 2 ], glm::cross( simplex[ 1 ] - simplex[ 2 ], triNormal ) );
			if( dot > 0 )
			{
				//Origin is either closest to line 21 or point 2
				dot = glm::dot( -simplex[ 2 ], simplex[ 1 ] - simplex[ 2 ] );
				if( dot > 0 )
				{
					//closest to line 21

					cross = glm::cross( simplex[ 1 ] - simplex[ 2 ], -simplex[ 2 ] );
					cross = glm::cross( cross, simplex[ 1 ] - simplex[ 2 ] );
					*D = cross;    

					temp1 = simplex[ 1 ];
					temp2 = simplex[ 2 ];
					simplex[ 0 ] = temp1;
					simplex[ 1 ] = temp2;
					( *simplexIndex )--;	//Simplex is now a line, D is now a direction perpendicular to the simplex pointing toward the origin.
				}
				else
				{
					//closest to point 2
					simplex[ 0 ] = simplex[ 2 ];
					( *simplexIndex ) = 1;
					*D = -simplex[ 2 ];
				}
				return false;
			}

			//Origin is above or below the triangle
			dot = glm::dot( -simplex[ 2 ], triNormal );
			if( dot > 0 )
			{
				//Origin is above Triangle (in direction of triangle normal)
				*D = triNormal;
			}
			else if( dot < 0 )
			{
				//Origin is below Triangle (in opposite direction of triangle normal)
				*D = -triNormal;
			}
			else
			{
				//Origin is enclosed by triangle
				return true;
			}

			return false;

		//Tetrahedron
		case 3:

			bool over1 = false, over2 = false, over3 = false, over4 = false;

			triNormal = glm::cross( simplex[ 1 ] - simplex[ 2 ], simplex[ 0 ] - simplex[ 2 ] );
			// old
			triNormal = glm::cross( simplex[ 1 ] - simplex[ 2 ], simplex[ 0 ] - simplex[ 2 ] );

			//Test if the origin is above each face of the tetrahedron, if it is then the face normal is the new search direction.
			
			if( glm::dot( triNormal, *D ) < 0 ) 
			{
				triNormal = glm::cross( simplex[ 3 ] - simplex[ 2 ], simplex[ 1 ] - simplex[ 2 ] );

				if( glm::dot( triNormal, -simplex[ 3 ] ) > 0 )
				{
					//Origin is either above triangle 123, closest to line 13, or closest to line 32
					cross = glm::cross( triNormal, simplex[ 3 ] - simplex[ 1 ] );
					
					if( glm::dot( cross, -simplex[ 3 ] ) > 0 )
					{
						//Origin is closest to line 13
						cross = glm::cross( simplex[ 3 ] - simplex[ 1 ], -simplex[ 1 ] );
						cross = glm::cross( cross, simplex[ 3 ] - simplex[ 1 ] );
						*D = cross;
						temp1 = simplex[ 1 ];
						temp2 = simplex[ 3 ];
						simplex[ 0 ] = temp1;
						simplex[ 1 ] = temp2;
						*simplexIndex = 2;

						return false;
					}

					cross = glm::cross( triNormal, simplex[ 2 ] - simplex[ 3 ] );

					if( glm::dot( cross, -simplex[ 3 ] ) > 0 )
					{
						//Origin is closest to line 32
						cross = glm::cross( simplex[ 2 ] - simplex[ 3 ], -simplex[ 3 ] );
						cross = glm::cross( cross, simplex[ 2 ] - simplex[ 3 ] );
						*D = cross;
						temp1 = simplex[ 2 ];
						temp2 = simplex[ 3 ];
						simplex[ 0 ] = temp1;
						simplex[ 1 ] = temp2;
						*simplexIndex = 2;

						return false;
					}

					//Origin is closest to triangle 123
					*D = triNormal;
					temp1 = simplex[ 1 ];
					temp2 = simplex[ 2 ];
					temp3 = simplex[ 3 ];
					simplex[ 0 ] = temp1;
					simplex[ 1 ] = temp2;
					simplex[ 2 ] = temp3;
					*simplexIndex = 3;

					return false;

				}
				else
				{
					triNormal = glm::cross( simplex[ 0 ] - simplex[ 2 ], simplex[ 3 ] - simplex[ 2 ] );

					if( glm::dot( triNormal, -simplex[ 3 ] ) > 0 )
					{
						//Origin is either closest to line 03, line 32, or triangle 023
						cross = glm::cross( triNormal, simplex[ 3 ] - simplex[ 2 ] );

						if( glm::dot( cross, -simplex[ 3 ] ) > 0 )
						{
							//Origin is closest to line 23
							cross = glm::cross( simplex[ 3 ] - simplex[ 2 ], -simplex[ 2 ] );
							cross = glm::cross( cross, simplex[ 3 ] - simplex[ 2 ] );
							*D = cross;
							temp1 = simplex[ 2 ];
							temp2 = simplex[ 3 ];
							simplex[ 0 ] = temp1;
							simplex[ 1 ] = temp2;
							*simplexIndex = 2;

							return false;
						}

						cross = glm::cross( triNormal, simplex[ 0 ] - simplex[ 3 ] );

						if( glm::dot( cross, -simplex[ 3 ] ) > 0 )
						{
							//Origin is closest to line 30
							cross = glm::cross( simplex[ 0 ] - simplex[ 3 ], -simplex[ 3 ] );
							cross = glm::cross( cross, simplex[ 0 ] - simplex[ 3 ] );
							*D = cross;
							temp1 = simplex[ 0 ];
							temp2 = simplex[ 3 ];
							simplex[ 0 ] = temp1;
							simplex[ 1 ] = temp2;
							*simplexIndex = 2;

							return false;
						}

						//Origin is closest to triangle 023
						*D = triNormal;
						temp1 = simplex[ 0 ];
						temp2 = simplex[ 2 ];
						temp3 = simplex[ 3 ];
						simplex[ 0 ] = temp1;
						simplex[ 1 ] = temp2;
						simplex[ 2 ] = temp3;
						*simplexIndex = 3;

						return false;
					}
					else
					{
						triNormal = glm::cross( simplex[ 3 ] - simplex[ 1 ], simplex[ 0 ] - simplex[ 1 ] );

						if( glm::dot( triNormal, -simplex[ 3 ] ) > 0 )
						{
							//Origin is either closest to line 03, line 31, or triangle 013
							cross = glm::cross( triNormal, simplex[ 3 ] - simplex[ 0 ] );

							if( glm::dot( cross, -simplex[ 3 ] ) > 0 )
							{
								//Origin is closest to line 03
								cross = glm::cross( simplex[ 3 ] - simplex[ 0 ], -simplex[ 0 ] );
								cross = glm::cross( cross, simplex[ 3 ] - simplex[ 0 ] );
								*D = cross;
								temp1 = simplex[ 0 ];
								temp2 = simplex[ 3 ];
								simplex[ 0 ] = temp1;
								simplex[ 1 ] = temp2;
								*simplexIndex = 2;

								return false;
							}

							cross = glm::cross( triNormal, simplex[ 1 ] - simplex[ 3 ] );

							if( glm::dot( cross, -simplex[ 3 ] ) > 0 )
							{
								//Origin is closest to line 31
								cross = glm::cross( simplex[ 1 ] - simplex[ 3 ], -simplex[ 3 ] );
								cross = glm::cross( cross, simplex[ 1 ] - simplex[ 3 ] );
								*D = cross;
								temp1 = simplex[ 1 ];
								temp2 = simplex[ 3 ];
								simplex[ 0 ] = temp1;
								simplex[ 1 ] = temp2;
								*simplexIndex = 2;

								return false;
							}

							//Origin is closest to triangle 013
							*D = triNormal;
							temp1 = simplex[ 0 ];
							temp2 = simplex[ 1 ];
							temp3 = simplex[ 3 ];
							simplex[ 0 ] = temp1;
							simplex[ 1 ] = temp2;
							simplex[ 2 ] = temp3;
							*simplexIndex = 3;

							return false;
						}
						else
							return true;	//Origin is in Tetrahedron
					}
				}
			}
			else
			{
				triNormal = glm::cross( simplex[ 1 ] - simplex[ 2 ], simplex[ 3 ] - simplex[ 2 ] );

				if( glm::dot( triNormal, -simplex[ 3 ] ) > 0 )
				{
					//Origin is somewhere above triangle 123
					cross = glm::cross( triNormal, simplex[ 1 ] - simplex[ 3 ] );

					if( glm::dot( cross, -simplex[ 3 ] ) > 0 )
					{
						//Origin is closest to line 13
						cross = glm::cross( simplex[ 3 ] - simplex[ 1 ], -simplex[ 1 ] );
						cross = glm::cross( cross, simplex[ 3 ] - simplex[ 1 ] );
						*D = cross;
						temp1 = simplex[ 1 ];
						temp2 = simplex[ 3 ];
						simplex[ 0 ] = temp1;
						simplex[ 1 ] = temp2;
						*simplexIndex = 2;

						return false;
					}

					cross = glm::cross( triNormal, simplex[ 3 ] - simplex[ 2 ] );

					if( glm::dot( cross, -simplex[ 3 ] ) > 0 )
					{
						//Origin is closest to line 32
						cross = glm::cross( simplex[ 2 ] - simplex[ 3 ], -simplex[ 3 ] );
						cross = glm::cross( cross, simplex[ 2 ] - simplex[ 3 ] );
						*D = cross;
						temp1 = simplex[ 2 ];
						temp2 = simplex[ 3 ];
						simplex[ 0 ] = temp1;
						simplex[ 1 ] = temp2;
						*simplexIndex = 2;

						return false;
					}

					//Origin is closest to triangle 123
					*D = triNormal;
					temp1 = simplex[ 1 ];
					temp2 = simplex[ 2 ];
					temp3 = simplex[ 3 ];
					simplex[ 0 ] = temp1;
					simplex[ 1 ] = temp2;
					simplex[ 2 ] = temp3;
					*simplexIndex = 3;

					return false;
				}
				else
				{
					triNormal = glm::cross( simplex[ 3 ] - simplex[ 2 ], simplex[ 0 ] - simplex[ 2 ] );

					if( glm::dot( triNormal, -simplex[ 3 ] ) > 0 )
					{
						//Origin is somewhere above triangle 0-2-3
						cross = glm::cross( triNormal, simplex[ 2 ] - simplex[ 3 ] );

						if( glm::dot( cross, -simplex[ 3 ] ) > 0 )
						{
							//Origin is closest to line 2-3
							cross = glm::cross( simplex[ 3 ] - simplex[ 2 ], -simplex[ 2 ] );
							cross = glm::cross( cross, simplex[ 3 ] - simplex[ 2 ] );
							*D = cross;
							temp1 = simplex[ 2 ];
							temp2 = simplex[ 3 ];
							simplex[ 0 ] = temp1;
							simplex[ 1 ] = temp2;
							*simplexIndex = 2;

							return false;
						}

						cross = glm::cross( triNormal, simplex[ 3 ] - simplex[ 0 ] );

						if( glm::dot( cross, -simplex[ 3 ] ) > 0 )
						{
							//Origin is closest to line 3-0
							cross = glm::cross( simplex[ 0 ] - simplex[ 3 ], -simplex[ 3 ] );
							cross = glm::cross( cross, simplex[ 0 ] - simplex[ 3 ] );
							*D = cross;
							temp1 = simplex[ 0 ];
							temp2 = simplex[ 3 ];
							simplex[ 0 ] = temp1;
							simplex[ 1 ] = temp2;
							*simplexIndex = 2;

							return false;
						}

						//Origin is closest to triangle 0-2-3
						*D = triNormal;
						temp1 = simplex[ 0 ];
						temp2 = simplex[ 2 ];
						temp3 = simplex[ 3 ];
						simplex[ 0 ] = temp1;
						simplex[ 1 ] = temp2;
						simplex[ 2 ] = temp3;
						*simplexIndex = 3;

						return false;
					}
					else
					{
						triNormal = glm::cross( simplex[ 0 ] - simplex[ 1 ], simplex[ 3 ] - simplex[ 1 ] );

						if( glm::dot( triNormal, -simplex[ 3 ] ) > 0 )
						{
							//Origin is somewhere above triangle 0-1-3
							cross = glm::cross( triNormal, simplex[ 0 ] - simplex[ 3 ] );

							if( glm::dot( cross, -simplex[ 3 ] ) > 0 )
							{
								//Origin is closest to line 0-3
								cross = glm::cross( simplex[ 3 ] - simplex[ 0 ], -simplex[ 0 ] );
								cross = glm::cross( cross, simplex[ 3 ] - simplex[ 0 ] );
								*D = cross;
								temp1 = simplex[ 0 ];
								temp2 = simplex[ 3 ];
								simplex[ 0 ] = temp1;
								simplex[ 1 ] = temp2;
								*simplexIndex = 2;

								return false;
							}

							cross = glm::cross( triNormal, simplex[ 3 ] - simplex[ 1 ] );

							if( glm::dot( cross, -simplex[ 3 ] ) > 0 )
							{
								//Origin is closest to line 3-1
								cross = glm::cross( simplex[ 1 ] - simplex[ 3 ], -simplex[ 3 ] );
								cross = glm::cross( cross, simplex[ 1 ] - simplex[ 3 ] );
								*D = cross;
								temp1 = simplex[ 1 ];
								temp2 = simplex[ 3 ];
								simplex[ 0 ] = temp1;
								simplex[ 1 ] = temp2;
								*simplexIndex = 2;

								return false;
							}

							//Origin is closest to triangle 0-1-3
							*D = triNormal;
							temp1 = simplex[ 0 ];
							temp2 = simplex[ 1 ];
							temp3 = simplex[ 3 ];
							simplex[ 0 ] = temp1;
							simplex[ 1 ] = temp2;
							simplex[ 2 ] = temp3;
							*simplexIndex = 3;

							return false;
						}
						else
							return true;	//Origin is in Tetrahedron
					}
				}
			}
			//invalid index, simplex is a 4-size array
			std::cout << "\nError: unexpected index of " << simplexIndex << " in doSimplex function.\n";
			break;
		}
		return false;
	}

	//Returns the translation that will push this block out of the face
	virtual glm::vec3* gjkCollision( Block& other, bool* edgeCase, int& csoSearchCount )
	{
		glm::vec3 D( *mMoveDirectionPtr );
		glm::vec3 S = supportGJK( *this, other, D );
		CSOFacet closestFacet;

		if( D.x == 0 && D.y == 0 && D.z == 0 )
			return nullptr;

		glm::vec3* value = nullptr;

		glm::vec3* simplex = new glm::vec3[ 4 ];
		int simplexIndex( 1 );
		simplex[ 0 ] = S;
		D = -S;

		int count( 0 );
		while( count < 10 )
		{
			S = supportGJK( *this, other, D );
			if( glm::dot( S, D ) < 0 )
			{
				//No Intersection is possible
				break;
			}
			simplex[ simplexIndex++ ] = S;
			if( doSimplex( simplex, &simplexIndex, &D ) )
			{
				//Origin was found in simplex
				value = new glm::vec3( );

				if( containsOrigin( simplex, 4 ) )
				{
					closestFacet = penetrationDepth( simplex, simplexIndex, *this, other, edgeCase, csoSearchCount );

					if( closestFacet.getDistanceSquared( ) != 0.0f )
						*value = ( -glm::normalize( closestFacet.getNormal( ) ) ) * ( sqrt( closestFacet.getDistanceSquared( ) ) + 1.0f );
					else
						*value = glm::vec3( );
				}

				delete[ ] simplex;
				simplex = nullptr;

				return value;
			}
			count++;

		}
		if( count == 10 )
		{
			std::cout << "\nGJK took too long to find the origin, assume no overlap.\n";
			//printCSOFace( other );
			//gjkCollisionFace( other );		//Debug
		}

		delete[ ] simplex;
		simplex = nullptr;

		return nullptr;
	}
	// Clear
	virtual glm::vec3 supportGJK( const Block& block, const Block& other, const glm::vec3& dir ) const
	{
		float max = -INFINITY;
		int index1 = 0;
		int index2 = 0;
		unsigned int size1 = block.getVertexCount( );
		unsigned int size2 = other.getVertexCount( );

		for( unsigned int i = 0; i < size1; i++ )
		{

			float dot = glm::dot( block.getVertexVec3At( i ), dir );
			if( dot > max )
			{
				max = dot;
				index1 = i;
			}
		}

		max = -INFINITY;

		for( unsigned int i = 0; i < size2; i++ )
		{
			float dot = glm::dot( other.getVertexVec3At( i ), -dir );
			if( dot > max )
			{
				max = dot;
				index2 = i;
			}
		}

		return block.getVertexVec3At( index1 ) - other.getVertexVec3At( index2 );
	}
	
	virtual CSOFacet penetrationDepth( glm::vec3* simplex, int simplexIndex, const Block& block, const Block& other, bool* edgeCase, int& csoSearchCount )
	{
		CSOFacet closestFacet;
		CSOFacet temp;
		std::priority_queue< CSOFacet, std::vector< CSOFacet >, CSOQueueCompare > facetQueue;

		switch( simplexIndex )
		{
		case 4:				//Tetrahedron

			temp.setData( simplex[ 0 ], simplex[ 1 ], simplex[ 2 ] );
			facetQueue.push( temp );
			temp.setData( simplex[ 1 ], simplex[ 2 ], simplex[ 3 ] );
			facetQueue.push( temp );
			temp.setData( simplex[ 0 ], simplex[ 1 ], simplex[ 3 ] );
			facetQueue.push( temp );
			temp.setData( simplex[ 0 ], simplex[ 2 ], simplex[ 3 ] );
			facetQueue.push( temp );

			glm::vec3 closestTri[ 3 ];
			glm::vec3 normal;
			glm::vec3 triCenter;
			glm::vec3 triPoint;
			glm::vec3 p1, p2;
			glm::vec3 edgeNormal, edgeSplitter;
			CSOFacet facet;

			//Loop will look for the facet of the Minkowski difference ( CSO ) that's closest to the origin.

			int count( 0 );
			while( count < 100 )
			{
				count++;

				facet = facetQueue.top( );
				facetQueue.pop( );

				normal = facet.getNormal( );
				triCenter = supportGJK( block, other, normal );
				triPoint = facet.getPoint( 0 );

				//Check if we've found the facet of the Minkowski difference that's closest to the origin.
				if( !aboveTri( normal, triPoint, triCenter ) )
				{
					csoSearchCount = count;
					return facet;
				}

				for( int i = 0; i < 3; i++ )
					closestTri[ i ] = facet.getPoint( i );

				for( int i = 0; i < 3; i++ )	//Iterate through the Tri's edges to see what new facets should be added
				{
					p1 = closestTri[ i ];
					p2 = closestTri[ ( i + 1 ) % 3 ];

					edgeNormal = glm::cross( triCenter - p1, p2 - p1 );
					if( glm::dot( edgeNormal, p1 ) < 0.0f )
						edgeNormal = -edgeNormal;
					edgeSplitter = supportGJK( block, other, edgeNormal );

					if( !aboveTri( edgeNormal, triCenter, edgeSplitter ) )		//EdgeSplitter is either p1, p2, or at the same distance along edgeNormal as them.
					{															//The Edge is on the boundary of the CSO and doesn't need to be split 
						
						temp.setData( p1, p2, triCenter );
						facetQueue.push( temp );
					}
					else
					{
						CSOFacet temp1 = CSOFacet( p1, p2, triCenter );
						CSOFacet temp2 = CSOFacet( p1, edgeSplitter, triCenter );
						CSOFacet temp3 = CSOFacet( p2, edgeSplitter, triCenter );
						CSOFacet temp4 = CSOFacet( p1, p2, edgeSplitter );

						facetQueue.push( temp2 );
						facetQueue.push( temp3 );
					}
				}
			}
			std::cout << "\nPenetration Depth failed to find the closest CSO Face.";
			csoSearchCount = count;

			*edgeCase = true;
		}

		return CSOFacet( );
	}

	void printCSOFace( const Face& face ) const
	{
		std::cout << "\nVertices of CSO: ";

		for( int i = 0; i < mVertexCount; i++ )
		{
			glm::vec3 v1 = getVertexVec3At( i );

			for( int j = 0; j < 3; j++ )
			{
				glm::vec3 v2 = face.getVec3At( j );

				std::cout << "\n  ( " << ( v1.x - v2.x ) / 10 << ", " << ( v1.y - v2.y ) / 10 << ", " << ( v1.z - v2.z ) / 10 << " )";  //Divided by 10 to fit in blender
			}
		}
	}

	~Block( );
	Block( );
	Block( const char* filePath );
	Block( const Block& other );
	Block( const std::vector< Vertex >& vertices, const int* tempV, const int* tempN, 
		   const std::vector< Face >& faces, const std::vector< glm::vec3 >& normals, const glm::vec3& blockCenter, const std::string& filePath );

protected:
	void setFacets( );
	void setPlaneFacet( );

	int mID;

	BlockTransform mTransform;
	std::vector< Vertex > mVertices;
	std::vector< Face > mFaces;

	// UV array and indices
	std::vector< int > mUVIndices;
	std::vector< float > mUVArray;

	// Vertex Normals, Applied to the Vertices in the order described by mIndices
	std::vector< glm::vec3 > mNormals;
	int mNormalsCount;

	int mVertexCount;
	int mFaceCount;

	bool mStatic;	//The application is responsible for checking if a block is static before calling updateTransform.

	// NEW 7/2/2022: The application is responsible for creating Renderer data, not the Block

	BoundingSphere mSphere;
	AABoundingBox mAABB;

	glm::vec3* mMoveDirectionPtr;
	PhysicsProperties* mPhysicsPtr;
	const Block* mGround;

	std::vector< Material > mMaterials;

	std::vector< Facet > mFacets;

	std::string mName;
};

class BlockCapsule : public Block
{
public:

	struct CSOFacet override;

	bool gjkCollision( const Block& b, const glm::vec3& dir, glm::vec3& outMove );
	glm::vec3 supportGJKCapsule( const std::vector< glm::vec3 >& collidablePoints, const glm::vec3& dir ) const;
	glm::vec3 supportGJKCapsuleInner( const std::vector< glm::vec3 >& collidablePoints, const glm::vec3& dir ) const;
	Block::CSOFacet penetrationDepth( glm::vec3* simplex, int simplexIndex, const Block& block, const Block& other, bool* edgeCase, int& csoSearchCount ) override;
	void updateTransform( const glm::vec4& t, const Quaternion& r, const float& s ) override;

	glm::vec3 penetrationDepth( glm::vec3* simplex, const glm::vec3& outDir, const std::vector< int >& collidablePointsIndices,
		std::vector< Facet >& collidableFacets, const Block& b ) const;
	glm::vec3 complexCollision( std::vector< Facet >& collidableFacets, const glm::vec3& outDir ) const;
	bool checkGrounded( ) const;

	inline glm::vec3 getTopPoint( void ) const { return mTransform.getTranslationVec3( ) + glm::vec3( 0, mCylinderHalfHeight, 0 ); }
	inline glm::vec3 getBotPoint( void ) const { return mTransform.getTranslationVec3( ) - glm::vec3( 0, mCylinderHalfHeight, 0 ); }
	inline float getRadius( void ) const { return mRadius; }
	inline float getHalfHeight( void ) const { return mCylinderHalfHeight; }
	inline bool getSimpleCollision( ) const { return mSimpleCollision; }
	glm::vec3 getGroundNormal( const Block& b ) const;

	void inline setSimpleCollision( bool s ) { mSimpleCollision = s; }

	static void sphereRayIntersect( const glm::vec3& sCenter, float sRadius, const glm::vec3& linePoint, const glm::vec3& lineDir, glm::vec3* p1, glm::vec3* p2 )
	{
		glm::vec3 halfPoint = linePoint + glm::dot( sCenter - linePoint, lineDir ) * lineDir;
		float dist = glm::length( sCenter - halfPoint );
		//float angle = glm::asin( dist / sRadius );    // 1/22/2023 Alternate way of getting x 
		//float x = sRadius * glm::cos( angle );
		float x = glm::sqrt( sRadius * sRadius - dist * dist );

		if( dist <= sRadius )
		{
			if( p1 )
				*p1 = halfPoint - x * lineDir;
			if( p2 )
				*p2 = halfPoint + x * lineDir;
		}
		else
		{
			p1 = nullptr;
			p2 = nullptr;
		}
	}

	BlockCapsule( const char* fileName, float height, float radius );
	BlockCapsule( );
protected:
	float mCylinderHalfHeight;
	float mRadius;

	bool mSimpleCollision;
};


#include "Block.h"
#include "DAEFileParser.h"
#include <unordered_map>
#include <queue>
#include <cassert>


#define SIMILAR_DIRECTION( a, b ) glm::dot( a, b ) > 0 ? true : false
#define DIFFERENT_DIRECTION( a, b ) glm::dot( a, b ) < 0 ? true : false
#define VECTOR_EQUALS( a, b ) glm::abs( a.x - b.x ) + glm::abs( a.y - b.y ) + glm::abs( a.z - b.z ) < 0.001f ? true : false
#define VECTOR_EQUALS_EXACT( a, b ) a.x == b.x &&  a.y == b.y &&  a.z == b.z  ? true : false


void addIfUniqueVec3( const glm::vec3& point, std::vector< glm::vec3 >& vec )
{
	for( unsigned int i = 0; i < vec.size( ); i++ )
	{
		if( VECTOR_EQUALS( vec.at( i ), point ) )
			return;
	}
	vec.emplace_back( point );
}


bool BlockCapsule::gjkCollision( const Block& b, const glm::vec3& moveDir, glm::vec3& outMove )
{
	// First, Iterate over other's mFaces to make a vector of points that could collide with this Capsule.

	std::vector< Facet > collidableFacets;
	collidableFacets.reserve( b.getFacets( ).size( ) / 2 );
	std::vector< int > vertexIndices;
	vertexIndices.reserve( b.getVertexCount( ) / 2 );

	const std::vector< Facet > facets( b.getFacets( ) );
	bool unique;
	for( Facet f : facets )
	{
		if( DIFFERENT_DIRECTION( moveDir, f.getNormal( ) ) )
		{
			collidableFacets.emplace_back( f );
			for( int vertexIndex : f.getHull( ) )
			{
				unique = true;
				for( int ind : vertexIndices )
				{
					if( vertexIndex == ind )
						unique = false;
				}
				if( unique )
					vertexIndices.emplace_back( vertexIndex );
			}
		}
	}
	if( vertexIndices.size( ) == 0 )
	{
		for( Facet f : facets )
		{
			if( DIFFERENT_DIRECTION( moveDir, f.getNormal( ) ) )
			{
				collidableFacets.emplace_back( f );
				for( int vertexIndex : f.getHull( ) )
				{
					unique = true;
					for( int ind : vertexIndices )
					{
						if( vertexIndex == ind )
							unique = false;
					}
					if( unique )
						vertexIndices.emplace_back( vertexIndex );
				}
			}
		}
	}

	// GJK Support
	glm::vec3 D( moveDir );
	glm::vec3 S;
	float max = -INFINITY;
	int index1 = 0;
	int index2 = 0;
	float dot;
	unsigned int size2 = vertexIndices.size( );
	glm::vec3 selfPoint;

	if( D.y < 0 )
		selfPoint = getBotPoint( ) + glm::normalize( D ) * mRadius;

	else if( D.y > 0 )
		selfPoint = getTopPoint( ) + glm::normalize( D ) * mRadius;

	else
		selfPoint = mTransform.getTranslationVec3( ) + glm::normalize( D ) * mRadius;

	for( unsigned int i = 0; i < size2; i++ )
	{
		dot = glm::dot( b.getVertices( )[ vertexIndices[ i ] ].getCoordinateVec3( ), -D );
		if( dot > max )
		{
			max = dot;
			index2 = i;
		}
	}
	S = selfPoint - b.getVertices( )[ vertexIndices[ index2 ] ].getCoordinateVec3( );

	glm::vec3 simplex[ 4 ];
	int simplexIndex( 1 );
	simplex[ 0 ] = S;
	D = -S;

	int count = 0;
	while( count < 100 )
	{
		count++;

		// GJK Support
		max = -INFINITY;
		index1 = 0;
		index2 = 0;
		if( D.y < 0 )
			selfPoint = getBotPoint( ) + glm::normalize( D ) * mRadius;
		else if( D.y > 0 )
			selfPoint = getTopPoint( ) + glm::normalize( D ) * mRadius;
		else
			selfPoint = mTransform.getTranslationVec3( ) + glm::normalize( D ) * mRadius;

		for( unsigned int i = 0; i < size2; i++ )
		{
			dot = glm::dot( b.getVertices( )[ vertexIndices[ i ] ].getCoordinateVec3( ), -D );
			if( dot > max )
			{
				max = dot;
				index2 = i;
			}
		}
		S = selfPoint - b.getVertices( )[ vertexIndices[ index2 ] ].getCoordinateVec3( );
		if( glm::dot( S, D ) < 0 )
		{
			//No Intersection is possible
			return false;
		}
		simplex[ simplexIndex++ ] = S;
		if( doSimplex( simplex, &simplexIndex, &D ) )
		{
			if( containsOrigin( simplex, 4 ) )
			{

				glm::vec3 outDir = -glm::normalize( moveDir );
				outMove = penetrationDepth( simplex, outDir, vertexIndices, collidableFacets, b );

				return true;
			}

			return false;
		}
	}
	std::cout << "GJK Failed to find origin, assume no overlap.\n";

	return false;
}


glm::vec3 BlockCapsule::supportGJKCapsule( const std::vector< glm::vec3 >& collidablePoints, const glm::vec3& dir ) const
{
	float max = -INFINITY;
	int index1 = 0;
	int index2 = 0;
	int size1 = mVertexCount;
	unsigned int size2 = collidablePoints.size( );
	glm::vec3 selfPoint;

	if( dir.y < 0 )
		selfPoint = getBotPoint( ) + glm::normalize( dir ) * mRadius;
	
	else if( dir.y > 0 )
		selfPoint = getTopPoint( ) + glm::normalize( dir ) * mRadius;
	
	else
		selfPoint = mTransform.getTranslationVec3( ) + glm::normalize( dir ) * mRadius;
	

	for( unsigned int i = 0; i < size2; i++ )
	{
		float dot = glm::dot( collidablePoints.at( i ), -dir );
		if( dot > max )
		{
			max = dot;
			index2 = i;
		}
	}

	return selfPoint - collidablePoints.at( index2 );
}

glm::vec3 BlockCapsule::supportGJKCapsuleInner( const std::vector< glm::vec3 >& collidablePoints, const glm::vec3& dir ) const
{
	float max = -INFINITY;
	int index1 = 0;
	int index2 = 0;
	unsigned int size = collidablePoints.size( );
	glm::vec3 selfPoint;

	if( dir.y > 0 )
		selfPoint = getBotPoint( );

	else if( dir.y < 0 )
		selfPoint = getTopPoint( );

	else
		selfPoint = mTransform.getTranslationVec3( );


	for( unsigned int i = 0; i < size; i++ )
	{
		float dot = glm::dot( collidablePoints.at( i ), dir );
		if( dot > max )
		{
			max = dot;
			index2 = i;
		}
	}

	return collidablePoints.at( index2 ) - selfPoint;
}

glm::vec3 BlockCapsule::penetrationDepth(glm::vec3* simplex, const glm::vec3& outDir, const std::vector< int >& collidablePointsIndices,
	std::vector< Facet >& collidableFacets, const Block& b) const
{
	// NEW Idea 7/22/2022: Instead of adding and defining every possible facet of the CSO, just define
	// every possible Normal in the CSO and use that with the cloud of points in the CSO to test the region
	// the Origin is in.

	std::vector< int > edgeIndices; // Facet made by sweeping of b along mHeight in CSO
	edgeIndices.reserve( 2 );

	glm::vec3 projPoint;
	glm::vec3 edgeNormal;
	std::vector< int > edgePoints;
	edgePoints.reserve( b.getVertexCount( ) );

	glm::vec3 normal;
	float max, tempMax, min, tempMin;

	// Add to edgeConvexHulls all pseuedo "Facets" of faces formed by the sweeping of the edges of Block along mCylinder height.
	// Only necessary if the Capsule is travelling in the XZ plane
	if( outDir.x != 0 || outDir.z != 0 )
	{
		// First, get EdgePoints

		// 1/5/2023: Consider giving Block a vector of indices of vertices that are edgePoints, gets recalculated in updateTransform
		float normalY;
		bool up, down, center;
		const int* triIndices;
		for( int i : collidablePointsIndices )
		{
			up = false, down = false, center = false;
			for( Face f : b.getFaces( ) )
			{
				triIndices = f.getIndices( );

				for( int j = 0; j < 3; j++ )
				{
					if( triIndices[ j ] == i )
					{
						normalY = f.getNormal( ).y;
						if( normalY > 0.0f )
							up = true;
						else if( normalY < 0.0f )
							down = true;
						else
							center = true;
						break;
					}
				}
			}
			if( ( up && down ) || center )
				edgePoints.emplace_back( i );
		}
		// Next, organize the edgePoints in the correct order, from left to right

		glm::vec3 leftDir( outDir.z, 0, -outDir.x );
		glm::vec3 leftMost, rightMost;
		glm::vec3 excludeDir;
		float maxL = -INFINITY;
		float maxR = -INFINITY;
		float tempMaxL;
		float tempMaxR;
		unsigned int indexL = 0, indexR = 0, count = 0;
		for( unsigned int i = 0; i < edgePoints.size( ); i++ )
		{
			tempMaxL = glm::dot( leftDir, b.getVertexVec3At( edgePoints[ i ] ) );
			tempMaxR = glm::dot( -leftDir, b.getVertexVec3At( edgePoints[ i ] ) );

			if( tempMaxL >= maxL )
			{
				maxL = tempMaxL;
				indexL = i;
			}
			if( tempMaxR >= maxR )
			{
				maxR = tempMaxR;
				indexR = i;
			}
		}
		leftMost = b.getVertexVec3At( edgePoints[ indexL ] );
		rightMost = b.getVertexVec3At( edgePoints[ indexR ] );
		excludeDir = glm::cross( glm::vec3( 0, 1, 0 ), leftMost - rightMost );
		std::vector< glm::vec3 > blockVertices;
		b.fillVertices( blockVertices );

		auto compare = [ leftDir, blockVertices]( int lhs, int rhs )
		{ 
			glm::vec3 lhComp = blockVertices[ lhs ];
			glm::vec3 rhComp = blockVertices[ rhs ];
			if( glm::abs( leftDir.x ) < 0.0001f || glm::abs( leftDir.y ) < 0.0001f || glm::abs( leftDir.z ) < 0.0001f )
			{
				double lhDot = ( double ) leftDir.x * ( double ) lhComp.x + ( double ) leftDir.y * ( double ) lhComp.y + ( double ) leftDir.z * ( double ) lhComp.z;
				double rhDot = ( double ) leftDir.x * ( double ) rhComp.x + ( double ) leftDir.y * ( double ) rhComp.y + ( double ) leftDir.z * ( double ) rhComp.z;

				return lhDot < rhDot;
			}

			return glm::dot( leftDir, lhComp ) < glm::dot( leftDir, rhComp );
		};
		std::priority_queue< int, std::vector< int >, decltype( compare ) > edgePointsLeftToRight( compare );
		for( int ep : edgePoints )
		{
			if( glm::dot( b.getVertexVec3At( ep ) - rightMost, excludeDir ) <= 0.0f ) // ep is in front of the Line between leftMost and rightMost
			{
				edgePointsLeftToRight.push( ep );
			}
		}

		// Using the edges from Left to Right, Create and add ConvexHulls to edgeConvexHulls
		if( !edgePointsLeftToRight.empty( ) )
		{
			int ind1 = edgePointsLeftToRight.top( ); edgePointsLeftToRight.pop( );
			int ind2;
			glm::vec3 p1( b.getVertexVec3At( ind1 ) ); 
			glm::vec3 p2;
			const Facet* facetAdr = nullptr;
			while( !edgePointsLeftToRight.empty( ) )
			{
				ind2 = edgePointsLeftToRight.top( ); edgePointsLeftToRight.pop( );
				p2 = b.getVertexVec3At( ind2 ); 
				normal = p2 - p1;

				// NEW 11/14/2022: check if the previous point added is directly above or below, don't add if so
				if( glm::abs( normal.x ) > 0.01f || glm::abs( normal.z ) > 0.01f )
				{
					normal = glm::normalize( glm::vec3( normal.z, 0, -normal.x ) );
					facetAdr = b.getFacetPtrWithNormal( normal );
					if( !facetAdr )
					{
						edgeIndices.emplace_back( ind1 );
						edgeIndices.emplace_back( ind2 );

						collidableFacets.emplace_back( normal, edgeIndices, b.getVertices( ) );
						edgeIndices.clear( );
					}
				}
				ind1 = ind2;
				p1 = p2;
			}
		}
	}

	// Capsule will exit the block straight out the way it came.
	if( !mSimpleCollision )
		return complexCollision( collidableFacets, outDir );

	// Find the Facet that has the greatest distance from the origin after subtracting the proper selfPoint
	int facetIndex = -1;
	max = -INFINITY;
	glm::vec3 planePoint;
	for( unsigned int i = 0; i < collidableFacets.size( ); i++ )
	{
		planePoint = b.getVertexVec3At( collidableFacets[ i ].getHull( )[ 0 ] );
		normal = collidableFacets[ i ].getNormal( );
		if( normal.y > 0.0f )
			planePoint = planePoint - getBotPoint( );
		else if( normal.y < 0.0f )
			planePoint = planePoint - getTopPoint( );
		else
			planePoint = planePoint - mTransform.getTranslationVec3( );
		if( DIFFERENT_DIRECTION( normal, planePoint ) ) // Check if Origin is above plane
		{
			planePoint = glm::dot( normal, planePoint ) * normal;
			tempMax = glm::dot( planePoint, planePoint );
			if( tempMax > max )
			{
				max = tempMax;
				facetIndex = i;
			}
		}
	}
	// Origin might be "behind" all the planes in the CSO; use the plane with the shortest distance from origin 12/6/22
	if( facetIndex == -1 )
	{
		min = INFINITY;
		for( unsigned int i = 0; i < collidableFacets.size( ); i++ )
		{
			planePoint = b.getVertexVec3At( collidableFacets[ i ].getHull( )[ 0 ] );
			normal = collidableFacets[ i ].getNormal( );
			if( normal.y > 0.0f )
				planePoint = planePoint - getBotPoint( );
			else if( normal.y < 0.0f )
				planePoint = planePoint - getTopPoint( );
			else
				planePoint = planePoint - mTransform.getTranslationVec3( );
			planePoint = glm::dot( normal, planePoint ) * normal;
			tempMin = glm::dot( planePoint, planePoint );
			if( tempMin < min )
			{
				min = tempMin;
				facetIndex = i;
			}
		}
	}
	// Fill a vector with the points in the Inner CSO that we will test projPoint against, set normal to be that facet normal
	std::vector< glm::vec3 > innerCSO;
	glm::vec3 selfPoint;

	// DEBUG
	assert( facetIndex != -1 );

	normal = collidableFacets[ facetIndex ].getNormal( );
	if( normal.y > 0.0f )
	{
		selfPoint = getBotPoint( );
		innerCSO.reserve( collidableFacets[ facetIndex ].getHull( ).size( ) );
		for( int i : collidableFacets[ facetIndex ].getHull( ) )
			innerCSO.emplace_back( b.getVertexVec3At( i ) - selfPoint );
	}
	else if( normal.y < 0.0f )
	{
		selfPoint = getTopPoint( );
		innerCSO.reserve( collidableFacets[ facetIndex ].getHull( ).size( ) );
		for( int i : collidableFacets[ facetIndex ].getHull( ) )
			innerCSO.emplace_back( b.getVertexVec3At( i ) - selfPoint );
	}

	else    // Adding points to innerCSO is more involved if normal.y == 0, need to check each edge
	{   
		Facet* facet = &collidableFacets[ facetIndex ];
		uint32_t hullSize = collidableFacets[ facetIndex ].getHull( ).size( );
		innerCSO.reserve( hullSize + 2 );
		if( hullSize > 2 )
		{
			glm::vec3 p1, p2;
			uint32_t hullIndex = 1;
			p1 = b.getVertexVec3At( facet->getIndexAt( 0 ) );
			p2 = b.getVertexVec3At( facet->getIndexAt( 1 ) );
			float normalY = glm::cross( normal, p2 - p1 ).y;
			float prevY = normalY;
			if( normalY > 0 )
				innerCSO.emplace_back( p1 - getBotPoint( ) );
			else if( normalY < 0 )
				innerCSO.emplace_back( p1 - getTopPoint( ) );
			else
			{
				if( p1.y > p2.y )
				{
					innerCSO.emplace_back( p1 - getBotPoint( ) );
					innerCSO.emplace_back( p2 - getTopPoint( ) );
				}
				else
				{
					innerCSO.emplace_back( p1 - getTopPoint( ) );
					innerCSO.emplace_back( p2 - getBotPoint( ) );
				}
			}

			for( ; hullIndex < hullSize; hullIndex++ )
			{
				p1 = p2;
				p2 = b.getVertexVec3At( facet->getIndexAt( ( hullIndex + 1 ) % hullSize ) );
				normalY = glm::cross( normal, p2 - p1 ).y;
				if( normalY > 0 )
				{
					if( prevY < 0.0f )
						innerCSO.emplace_back( p1 - getTopPoint( ) );
					innerCSO.emplace_back( p1 - getBotPoint( ) );
				}
				else if( normalY < 0 )
				{
					if( prevY < 0.0f )
						innerCSO.emplace_back( p1 - getBotPoint( ) );
					innerCSO.emplace_back( p1 - getTopPoint( ) );
				}
				else
				{
					if( p1.y > p2.y )
						innerCSO.emplace_back( p1 - getBotPoint( ) );
					else
						innerCSO.emplace_back( p1 - getTopPoint( ) );
				}
				prevY = normalY;
			}
		}
		else   // Colliding with an edge, normal.y = 0
		{
			int p1 = collidableFacets[ facetIndex ].getIndexAt( 0 );
			int p2 = collidableFacets[ facetIndex ].getIndexAt( 1 );

			innerCSO.emplace_back( b.getVertexVec3At( p1 ) - getBotPoint( ) );
			innerCSO.emplace_back( b.getVertexVec3At( p1 ) - getTopPoint( ) );
			innerCSO.emplace_back( b.getVertexVec3At( p2 ) - getTopPoint( ) );
			innerCSO.emplace_back( b.getVertexVec3At( p2 ) - getBotPoint( ) );
		}
	}

	projPoint = glm::dot( ( -innerCSO[ 0 ] ), normal ) * ( -normal );

	// Check every edge in innerCSO against projPoint. 
	int edgeIndex = -1;
	max = -INFINITY;
	glm::vec3 edgeProj;
	glm::vec3 finalEdgeProj;
	for( unsigned int i = 0; i < innerCSO.size( ); i++ )
	{
		edgeNormal = glm::cross( normal, innerCSO[ ( i + 1 ) % innerCSO.size( ) ] - innerCSO[ i ] );
		if( DIFFERENT_DIRECTION( edgeNormal, innerCSO[ i ] - projPoint ) )
		{
			edgeNormal = glm::normalize( edgeNormal );
			edgeProj = projPoint + glm::dot( edgeNormal, projPoint - innerCSO[ i ] ) * ( -edgeNormal );
			tempMax = glm::dot( edgeProj, edgeProj );
			if( tempMax > max )
			{
				max = tempMax;
				edgeIndex = i;
				finalEdgeProj = edgeProj;
			}
		}
	}
	if( edgeIndex == -1 ) // Origin is over Facet
	{
		float distanceFromPlane = glm::sqrt( glm::dot( projPoint, projPoint ) );

		return ( mRadius + 1.0f - distanceFromPlane ) * normal;
	}
	else
	{
		if( DIFFERENT_DIRECTION( innerCSO[ ( edgeIndex + 1 ) % innerCSO.size( ) ] - innerCSO[ edgeIndex ], projPoint - innerCSO[ edgeIndex ] ) ) // Origin's nearest a corner
		{
			glm::vec3 cornerToOrigin = -innerCSO[ edgeIndex ];
			float distanceToLeave = mRadius + 1.0f - glm::sqrt( glm::dot( cornerToOrigin, cornerToOrigin ) );

			return glm::normalize( cornerToOrigin ) * distanceToLeave;
		}
		else if( DIFFERENT_DIRECTION( innerCSO[ edgeIndex ] - innerCSO[ ( edgeIndex + 1 ) % innerCSO.size( ) ], projPoint - innerCSO[ ( edgeIndex + 1 ) % innerCSO.size( ) ] ) )// Origin's nearest a corner
		{
			glm::vec3 cornerToOrigin = -innerCSO[ ( edgeIndex + 1 ) % innerCSO.size( ) ];
			float distanceToLeave = mRadius + 1.0f - glm::sqrt( glm::dot( cornerToOrigin, cornerToOrigin ) );

			return glm::normalize( cornerToOrigin ) * distanceToLeave;
		}
		else // Origin's nearest edgeProj
		{
			float distanceFromEdgeProj = glm::sqrt( glm::dot( finalEdgeProj, finalEdgeProj ) );

			return glm::normalize( -finalEdgeProj ) * ( mRadius + 1.0f - distanceFromEdgeProj );
		}
	}
	return glm::vec3( );
}

glm::vec3 BlockCapsule::complexCollision( std::vector< Facet >& collidableFacets, const glm::vec3& outDir ) const // NEW 1/20/2023
{
	glm::vec3 dir = glm::normalize( outDir );
	glm::vec3 planeIntersect;
	glm::vec3 planePoint;
	glm::vec3 selfPoint;
	glm::vec3 normal;
	glm::vec3 minIntersect;
	glm::vec3 minSelfPoint;
	float min = INFINITY;
	float tempMin;
	uint32_t minInd = -1;

	for( uint32_t i = 0; i < collidableFacets.size( ); i++ )
	{
		const Facet& f ( collidableFacets[ i ] );
		normal = f.getNormal( );
		if( normal.y > 0.0f )
			selfPoint = getBotPoint( );
		else if( normal.y < 0.0f )
			selfPoint = getTopPoint( );
		else
			selfPoint = getTranslation( );

		if( glm::dot( dir, normal ) > 0.0f )
		{
			// planePoint is in the outerCSO, that's why we add normal * mRadius
			planePoint = f.getCoordAt( 0 ) - selfPoint + normal * mRadius;
			planeIntersect = glm::dot( normal, planePoint ) / glm::dot( normal, dir ) * dir;
			
			// planeIntersect = line from origin to planeIntersect ie planeIntersect - vec3( 0 )
			if( f.isOver( planeIntersect, nullptr, -selfPoint ) ) 
				return planeIntersect + glm::normalize( planeIntersect );

			tempMin = glm::dot( planeIntersect, planeIntersect );
			if( tempMin < min )
			{
				min = tempMin;
				minIntersect = planeIntersect;
				minInd = i;
				minSelfPoint = selfPoint;
			}
		}
	}
	glm::vec3 planeToPoint;
	glm::vec3 spherePoint;
	collidableFacets[ minInd ].isOver( minIntersect, &planeToPoint, -minSelfPoint );
	spherePoint = minIntersect - planeToPoint; // Gives the point on the Facet closest to the outerCSO plane intersect
	Util::sphereRayIntersect( spherePoint, mRadius, glm::vec3( 0.0f ), glm::normalize( minIntersect ), nullptr, &minIntersect );

	std::cout << "Complex Collision Sphere Check used\n";
	return minIntersect + glm::normalize( minIntersect );
}

bool BlockCapsule::checkGrounded( ) const
{
	if( mGround )
	{
		glm::vec3 spherePoint = getBotPoint( );
		spherePoint.y -= 2.0f;

		std::vector< Facet > facets = mGround->getFacets( );
		glm::vec3 normal;
		glm::vec3 planeToPoint;

		for( uint32_t i = 0; i < facets.size( ); i++ )
		{
			normal = facets[ i ].getNormal( );

			if( normal.y > 0 )
			{
				facets[ i ].isOver( spherePoint, &planeToPoint, glm::vec3( ) );
				//if( facets[ i ].isOver( spherePoint, &planeToPoint ) )
				//{
					if( glm::dot( planeToPoint, planeToPoint ) < mRadius * mRadius )
					{
						return true;
					}
				//}
			}
		}
	}

	return false;
}

glm::vec3 BlockCapsule::getGroundNormal( const Block& b ) const
{
	int groundIndex = 0;
	std::vector< Face > faces = b.getFaces( );
	glm::vec3 spherePoint = getBotPoint( );
	glm::vec3 norm;
	glm::vec3 normProj;
	float max = -INFINITY;
	float distaceFromPlaneSquared;

	for( int i = 0; i < b.getFaceCount( ); i++ )
	{
		norm = faces[ i ].getNormal( );

		if( norm.y > 0 && SIMILAR_DIRECTION( norm, spherePoint - faces[ i ].getVec3At( 0 ) ) )
		{
			normProj = glm::dot( spherePoint - faces[ i ].getVec3At( 0 ), norm ) * norm;
			distaceFromPlaneSquared = glm::dot( normProj, normProj );

			if( distaceFromPlaneSquared > max )
			{
				max = distaceFromPlaneSquared;
				groundIndex = i;
			}
		}
	}
	if( max == -INFINITY )
	{
		max = INFINITY;
		for( int i = 0; i < b.getFaceCount( ); i++ )
		{
			norm = faces[ i ].getNormal( );

			if( norm.y > 0 )
			{
				normProj = glm::dot( spherePoint - faces[ i ].getVec3At( 0 ), norm ) * norm;
				distaceFromPlaneSquared = glm::dot( normProj, normProj );

				if( distaceFromPlaneSquared < max )
				{
					max = distaceFromPlaneSquared;
					groundIndex = i;
				}
			}
		}
	}

	return faces[ groundIndex ].getNormal( );
}

Block::CSOFacet BlockCapsule::penetrationDepth( glm::vec3* simplex, int simplexIndex, const Block& block, const Block& other, bool* edgeCase, int& csoSearchCount )
{

	return CSOFacet( );
}

void BlockCapsule::updateTransform( const glm::vec4& t, const Quaternion& r, const float& s )
{
	glm::mat4 transform( 1.0f );

	glm::vec3 oldTranslation = mTransform.getTranslationVec3( );
	glm::mat4 oldRotation = r.toMat4( );
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
	mSphere.setCenter( mTransform.getTranslationVec3( ) );
	if( oldScale != s )
		mSphere.setRadius( mSphere.getRadius( ) * s );
}

BlockCapsule::BlockCapsule( const char* fileName, float height, float radius ) : mCylinderHalfHeight( height ), mRadius( radius ), mSimpleCollision( true )
{
	mPhysicsPtr = new PhysicsProperties( );
	mGround = nullptr;
	mMoveDirectionPtr = new glm::vec3( );
	mID = 0;

	DAEFileParser parser( fileName );
	std::vector< float > vertexElements;
	std::vector< float > normalsElements;
	std::queue< int > faceTexIndices;
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
	std::vector< int* > tempFaceNormInd;
	std::vector< int* > tempUVInd;
	std::vector< int > arrLengths;

	for( unsigned int i = 0; i < mMaterials.size( ); i++ )
	{
		int* tempV;
		int* tempF;
		int* tempUV;
		int tempLen = parser.getIndexCount( );
		parser.initIndexArrays( &tempV, &tempF, &tempUV, i, nullptr );

		tempVertInd.push_back( tempV );
		tempFaceNormInd.push_back( tempF );
		tempUVInd.push_back( tempUV );
		arrLengths.push_back( parser.getIndexCount( ) - tempLen );
		faceTexIndices.push( parser.getIndexCount( ) - tempLen );
	}

	std::vector< int > vertexIndices;
	vertexIndices.reserve( parser.getIndexCount( ) );
	mUVIndices.reserve( parser.getIndexCount( ) );

	for( unsigned int i = 0; i < mMaterials.size( ); i++ )
	{
		for( int j = 0; j < arrLengths.at( i ); j++ )
		{
			vertexIndices.emplace_back( tempVertInd.at( i )[ j ] );
			mUVIndices.emplace_back( tempUVInd.at( i )[ j ] );
		}
		delete[ ] tempVertInd.at( i );
		tempVertInd.at( i ) = nullptr;

		delete[ ] tempFaceNormInd.at( i );
		tempFaceNormInd.at( i ) = nullptr;

		delete[ ] tempUVInd.at( i );
		tempUVInd.at( i ) = nullptr;
	}


	//Init Face Array
	mFaceCount = parser.getIndexCount( ) / 3;

	mFaces.reserve( mFaceCount );
	std::vector< int > faceIndices;
	faceIndices.reserve( 3 );

	// Fill Face Array
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

	// Init BoundingSphere for self 
	float max = -INFINITY;
	float lengthSquared;
	glm::vec3 vert;
	for( int i = 0; i < mVertexCount; i++ )
	{
		vert = mVertices[ i ].getCoordinateVec3( );
		lengthSquared = vert.x * vert.x + vert.y * vert.y + vert.z * vert.z;
		if( lengthSquared > max )
			max = lengthSquared;
	}
	max = glm::sqrt( max );
	mSphere = BoundingSphere( glm::vec3( ), max );
}

BlockCapsule::BlockCapsule( ) : mCylinderHalfHeight( 0 ), mRadius( 0 )
{

}








// Penetration Depth function for a capsule and a Face

// TODO 7/10/2022: Rewrite function
//TODO: Bugfix PenatrationDepth, fails to find CSO Face. May need to redo algorithm, or maybe support function is wrong. 8/29/2021
/*
Block::CSOFacet BlockCapsule::penetrationDepth( glm::vec3* simplex, int simplexIndex, const Block& block, const Block& other, bool* edgeCase, int& csoSearchCount )
{
	Block::CSOFacet value = Block::CSOFacet( );
	float dist;
	glm::vec3 dir;
	float distanceSquared = INFINITY, temp;
	int index;
	glm::vec3 points[ 6 ];
	int oldIndex;

	glm::vec3 topNormal, botNormal, aNormal, bNormal, cNormal;
	glm::vec3 linePoint, lineDir;

	glm::vec3 upper = mTransform.getTranslationVec3( ) + glm::vec3( 0, mCylinderHalfHeight, 0 );
	glm::vec3 lower = mTransform.getTranslationVec3( ) - glm::vec3( 0, mCylinderHalfHeight, 0 );

	topNormal = other.getNormal( );
	botNormal = -topNormal;


	if( topNormal.y >= 0.0f )
	{
		points[ 0 ] = upper - other.getVec3At( 0 );
		points[ 1 ] = upper - other.getVec3At( 1 );
		points[ 2 ] = upper - other.getVec3At( 2 );
		points[ 3 ] = lower - other.getVec3At( 0 );
		points[ 4 ] = lower - other.getVec3At( 1 );
		points[ 5 ] = lower - other.getVec3At( 2 );

		if( topNormal.y == 0 )	//Special case, will return here
		{
			glm::vec3 leftDir( glm::cross( topNormal, glm::vec3( 0, 1, 0 ) ) );
			glm::vec3 rightDir( glm::cross( topNormal, glm::vec3( 0, -1, 0 ) ) );

			int line1 = -1, line2;
			int left, right;
			float maxL = -INFINITY, maxR = -INFINITY;

			for( int i = 0; i < 3; i++ )
			{
				if( glm::dot( points[ i ], leftDir ) > maxL )
				{
					maxL = glm::dot( points[ i ], leftDir );
					left = i;
				}
				if( glm::dot( points[ i ], rightDir ) > maxR )
				{
					maxR = glm::dot( points[ i ], rightDir );
					right = i;
				}
			}

			aNormal = glm::cross( topNormal, points[ 0 ] - points[ 2 ] );
			bNormal = glm::cross( topNormal, points[ 1 ] - points[ 0 ] );
			cNormal = glm::cross( topNormal, points[ 2 ] - points[ 1 ] );

			if( glm::dot( aNormal, -points[ 0 ] ) > 0.0f && aNormal.y >= 0.0f )
			{
				line1 = 0;
				line2 = 2;
			}
			else if( glm::dot( bNormal, -points[ 0 ] ) > 0.0f && bNormal.y >= 0.0f )
			{
				line1 = 0;
				line2 = 1;
			}
			else if( glm::dot( cNormal, -points[ 1 ] ) > 0.0f && cNormal.y >= 0.0f )
			{
				line1 = 1;
				line2 = 2;
			}
			else if( glm::dot( aNormal, -points[ 3 ] ) > 0.0f && aNormal.y <= 0.0f )
			{
				line1 = 3;
				line2 = 5;
			}
			else if( glm::dot( bNormal, -points[ 3 ] ) > 0.0f && bNormal.y <= 0.0f )
			{
				line1 = 3;
				line2 = 4;
			}
			else if( glm::dot( cNormal, -points[ 4 ] ) > 0.0f && cNormal.y <= 0.0f )
			{
				line1 = 4;
				line2 = 5;
			}
			else if( glm::dot( leftDir, -points[ left ] ) > 0.0f )
			{
				line1 = left;
				line2 = left + 3;
			}
			else if( glm::dot( rightDir, -points[ right ] ) > 0.0f )
			{
				line1 = right;
				line2 = right + 3;
			}

			if( line1 == -1 )	//Origin is over the flat shape
			{
				dir = topNormal * glm::dot( topNormal, -points[ 0 ] );
			}
			else
			{
				if( points[ line1 ].x == points[ line2 ].x && points[ line1 ].z == points[ line2 ].z && line2 != line1 + 3 )
				{
					int topInd, botInd;
					if( points[ line1 ].y > points[ line2 ].y )
					{
						topInd = line1;
						botInd = ( line2 + 3 ) % 6;
					}
					else
					{
						topInd = line2;
						botInd = ( line1 + 3 ) % 6;
					}
					line1 = topInd;
					line2 = botInd;
				}

				if( glm::dot( points[ line1 ] - points[ line2 ], -points[ line2 ] ) < 0.0f )
				{
					dir = -points[ line2 ];
				}
				else if( glm::dot( points[ line2 ] - points[ line1 ], -points[ line1 ] ) < 0.0f )
				{
					dir = -points[ line1 ];
				}
				else
				{
					lineDir = glm::normalize( points[ line2 ] - points[ line1 ] );
					linePoint = points[ line1 ];

					dir = -linePoint - glm::dot( -linePoint, lineDir ) * lineDir;
				}
			}

			dist = mRadius - glm::sqrt( dir.x * dir.x + dir.y * dir.y + dir.z * dir.z );
			value.setNormal( dir );
			value.setDistanceSquared( dist * dist );

			return value;
		}
	}
	else
	{
		points[ 0 ] = lower - other.getVec3At( 0 );
		points[ 1 ] = lower - other.getVec3At( 1 );
		points[ 2 ] = lower - other.getVec3At( 2 );
		points[ 3 ] = upper - other.getVec3At( 0 );
		points[ 4 ] = upper - other.getVec3At( 1 );
		points[ 5 ] = upper - other.getVec3At( 2 );
	}

	aNormal = glm::cross( points[ 2 ] - points[ 3 ], points[ 0 ] - points[ 3 ] );
	bNormal = glm::cross( points[ 0 ] - points[ 4 ], points[ 1 ] - points[ 4 ] );
	cNormal = glm::cross( points[ 1 ] - points[ 5 ], points[ 2 ] - points[ 5 ] );

	//Find the Point the origin is closest to
	for( int i = 0; i < 6; i++ )
	{
		temp = points[ i ].x * points[ i ].x + points[ i ].y * points[ i ].y + points[ i ].z * points[ i ].z;
		if( temp < distanceSquared )
		{
			distanceSquared = temp;
			index = i;
		}
	}
	if( index < 3 && glm::dot( -points[ index ], topNormal ) < 0.0f )
	{
		oldIndex = index;
		distanceSquared = INFINITY;
		for( int i = 3; i < 6; i++ )
		{
			temp = points[ i ].x * points[ i ].x + points[ i ].y * points[ i ].y + points[ i ].z * points[ i ].z;
			if( temp < distanceSquared )
			{
				distanceSquared = temp;
				index = i;
			}
		}
		if( glm::dot( -points[ index ], botNormal ) < 0.0f )
			index = oldIndex;
	}
	else if( index > 2 && glm::dot( -points[ index ], botNormal ) < 0.0f )
	{
		oldIndex = index;
		distanceSquared = INFINITY;
		for( int i = 0; i < 3; i++ )
		{
			temp = points[ i ].x * points[ i ].x + points[ i ].y * points[ i ].y + points[ i ].z * points[ i ].z;
			if( temp < distanceSquared )
			{
				distanceSquared = temp;
				index = i;
			}
		}
		if( glm::dot( -points[ index ], topNormal ) < 0.0f )
			index = oldIndex;
	}
	//Do Plane Tests and Line Tests on area surrounding Point
	switch( index )
	{
	case 0:
		glm::vec3 n02a (glm::cross( aNormal, points[ 2 ] - points[ 0 ] ) );
		glm::vec3 n03a ( glm::cross( aNormal, points[ 0 ] - points[ 3 ] ) );
		glm::vec3 n01b ( glm::cross( bNormal, points[ 0 ] - points[ 1 ] ) );
		glm::vec3 n03b ( glm::cross( bNormal, points[ 3 ] - points[ 0 ] ) );
		glm::vec3 n02Top ( glm::cross( topNormal, points[ 0 ] - points[ 2 ] ) );
		glm::vec3 n01Top ( glm::cross( topNormal, points[ 1 ] - points[ 0 ] ) );

		//Check if Over a
		if( glm::dot( aNormal, -points[ index ] ) > 0.0f &&
			glm::dot( n02a, -points[ index ] ) < 0.0f &&
			glm::dot( n03a, -points[ index ] ) < 0.0f )
		{
			aNormal = glm::normalize( aNormal );
			dir = glm::dot( aNormal, -points[ index ] ) * aNormal;
		}
		//Check if Over b
		else if( glm::dot( bNormal, -points[ index ] ) > 0.0f &&
			glm::dot( n01b, -points[ index ] ) < 0.0f &&
			glm::dot( n03b, -points[ index ] ) < 0.0f )
		{
			bNormal = glm::normalize( bNormal );
			dir = glm::dot( bNormal, -points[ index ] ) * bNormal;
		}
		//Check if Over Top
		else if( glm::dot( topNormal, -points[ index ] ) > 0.0f &&
			glm::dot( n02Top, -points[ index ] ) < 0.0f &&
			glm::dot( n01Top, -points[ index ] ) < 0.0f )
		{
			topNormal = glm::normalize( topNormal );
			dir = glm::dot( topNormal, -points[ index ] ) * topNormal;
		}
		//Check if Over Line 0->2
		else if( glm::dot( n02Top, -points[ index ] ) > 0.0f &&
			glm::dot( n02a, -points[ index ] ) > 0.0f &&
			glm::dot( points[ 2 ] - points[ 0 ], -points[ index ] ) > 0.0f )
		{
			lineDir = glm::normalize( points[ 2 ] - points[ 0 ] );
			linePoint = points[ index ];

			dir = -linePoint - glm::dot( -linePoint, lineDir ) * lineDir;
		}
		//Check if Over Line 0->1
		else if( glm::dot( n01Top, -points[ index ] ) > 0.0f &&
			glm::dot( n01b, -points[ index ] ) > 0.0f &&
			glm::dot( points[ 1 ] - points[ 0 ], -points[ index ] ) > 0.0f )
		{
			lineDir = glm::normalize( points[ 1 ] - points[ 0 ] );
			linePoint = points[ index ];

			dir = -linePoint - glm::dot( -linePoint, lineDir ) * lineDir;
		}
		//Check if Over Line 0->3
		else if( glm::dot( n03a, -points[ index ] ) > 0.0f &&
			glm::dot( n03b, -points[ index ] ) > 0.0f &&
			glm::dot( points[ 3 ] - points[ 0 ], -points[ index ] ) > 0.0f )
		{
			lineDir = glm::normalize( points[ 3 ] - points[ 0 ] );
			linePoint = points[ index ];

			dir = -linePoint - glm::dot( -linePoint, lineDir ) * lineDir;
		}
		else    //Over point 0
		{
			dir = -points[ index ];
		}
		break;

	case 1:
		glm::vec3 n10b( glm::cross( bNormal, points[ 0 ] - points[ 1 ] ) );
		glm::vec3 n14b( glm::cross( bNormal, points[ 1 ] - points[ 4 ] ) );
		glm::vec3 n12c( glm::cross( cNormal, points[ 1 ] - points[ 2 ] ) );
		glm::vec3 n14c( glm::cross( cNormal, points[ 4 ] - points[ 1 ] ) );
		glm::vec3 n10Top( glm::cross( topNormal, points[ 1 ] - points[ 0 ] ) );
		glm::vec3 n12Top( glm::cross( topNormal, points[ 2 ] - points[ 1 ] ) );

		//Check if Over b
		if( glm::dot( bNormal, -points[ index ] ) > 0.0f &&
			glm::dot( n10b, -points[ index ] ) < 0.0f &&
			glm::dot( n14b, -points[ index ] ) < 0.0f )
		{
			bNormal = glm::normalize( bNormal );
			dir = glm::dot( bNormal, -points[ index ] ) * bNormal;
		}
		//Check if Over c
		else if( glm::dot( cNormal, -points[ index ] ) > 0.0f &&
			glm::dot( n12c, -points[ index ] ) < 0.0f &&
			glm::dot( n14c, -points[ index ] ) < 0.0f )
		{
			cNormal = glm::normalize( cNormal );
			dir = glm::dot( cNormal, -points[ index ] ) * cNormal;
		}
		//Check if Over Top
		else if( glm::dot( topNormal, -points[ index ] ) > 0.0f &&
			glm::dot( n10Top, -points[ index ] ) < 0.0f &&
			glm::dot( n12Top, -points[ index ] ) < 0.0f )
		{
			topNormal = glm::normalize( topNormal );
			dir = glm::dot( topNormal, -points[ index ] ) * topNormal;
		}
		//Check if Over Line 1->0
		else if( glm::dot( n10Top, -points[ index ] ) > 0.0f &&
			glm::dot( n10b, -points[ index ] ) > 0.0f &&
			glm::dot( points[ 0 ] - points[ 1 ], -points[ index ] ) > 0.0f )
		{
			lineDir = glm::normalize( points[ 0 ] - points[ 1 ] );
			linePoint = points[ index ];

			dir = -linePoint - glm::dot( -linePoint, lineDir ) * lineDir;
		}
		//Check if Over Line 1->2
		else if( glm::dot( n12Top, -points[ index ] ) > 0.0f &&
			glm::dot( n12c, -points[ index ] ) > 0.0f &&
			glm::dot( points[ 2 ] - points[ 1 ], -points[ index ] ) > 0.0f )
		{
			lineDir = glm::normalize( points[ 2 ] - points[ 1 ] );
			linePoint = points[ index ];

			dir = -linePoint - glm::dot( -linePoint, lineDir ) * lineDir;
		}
		//Check if Over Line 1->4
		else if( glm::dot( n14b, -points[ index ] ) > 0.0f &&
			glm::dot( n14c, -points[ index ] ) > 0.0f &&
			glm::dot( points[ 4 ] - points[ 1 ], -points[ index ] ) > 0.0f )
		{
			lineDir = glm::normalize( points[ 4 ] - points[ 1 ] );
			linePoint = points[ index ];

			dir = -linePoint - glm::dot( -linePoint, lineDir ) * lineDir;
		}
		else    //Over point 1
		{
			dir = -points[ index ];
		}
		break;

	case 2:
		glm::vec3 n21c( glm::cross( cNormal, points[ 1 ] - points[ 2 ] ) );
		glm::vec3 n25c( glm::cross( cNormal, points[ 2 ] - points[ 5 ] ) );
		glm::vec3 n20a( glm::cross( aNormal, points[ 2 ] - points[ 0 ] ) );
		glm::vec3 n25a( glm::cross( aNormal, points[ 5 ] - points[ 2 ] ) );
		glm::vec3 n21Top( glm::cross( topNormal, points[ 2 ] - points[ 1 ] ) );
		glm::vec3 n20Top( glm::cross( topNormal, points[ 0 ] - points[ 2 ] ) );

		//Check if Over c
		if( glm::dot( cNormal, -points[ index ] ) > 0.0f &&
			glm::dot( n21c, -points[ index ] ) < 0.0f &&
			glm::dot( n25c, -points[ index ] ) < 0.0f )
		{
			cNormal = glm::normalize( cNormal );
			dir = glm::dot( cNormal, -points[ index ] ) * cNormal;
		}
		//Check if Over a
		else if( glm::dot( aNormal, -points[ index ] ) > 0.0f &&
			glm::dot( n20a, -points[ index ] ) < 0.0f &&
			glm::dot( n25a, -points[ index ] ) < 0.0f )
		{
			aNormal = glm::normalize( aNormal );
			dir = glm::dot( aNormal, -points[ index ] ) * aNormal;
		}
		//Check if Over Top
		else if( glm::dot( topNormal, -points[ index ] ) > 0.0f &&
			glm::dot( n21Top, -points[ index ] ) < 0.0f &&
			glm::dot( n20Top, -points[ index ] ) < 0.0f )
		{
			topNormal = glm::normalize( topNormal );
			dir = glm::dot( topNormal, -points[ index ] ) * topNormal;
		}
		//Check if Over Line 2->1
		else if( glm::dot( n21c, -points[ index ] ) > 0.0f &&
			glm::dot( n21Top, -points[ index ] ) > 0.0f &&
			glm::dot( points[ 1 ] - points[ 2 ], -points[ index ] ) > 0.0f )
		{
			lineDir = glm::normalize( points[ 1 ] - points[ 2 ] );
			linePoint = points[ index ];

			dir = -linePoint - glm::dot( -linePoint, lineDir ) * lineDir;
		}
		//Check if Over Line 2->0
		else if( glm::dot( n20Top, -points[ index ] ) > 0.0f &&
			glm::dot( n20a, -points[ index ] ) > 0.0f &&
			glm::dot( points[ 0 ] - points[ 2 ], -points[ index ] ) > 0.0f )
		{
			lineDir = glm::normalize( points[ 0 ] - points[ 2 ] );
			linePoint = points[ index ];

			dir = -linePoint - glm::dot( -linePoint, lineDir ) * lineDir;
		}
		//Check if Over Line 2->5
		else if( glm::dot( n25a, -points[ index ] ) > 0.0f &&
			glm::dot( n25c, -points[ index ] ) > 0.0f &&
			glm::dot( points[ 5 ] - points[ 2 ], -points[ index ] ) > 0.0f )
		{
			lineDir = glm::normalize( points[ 5 ] - points[ 2 ] );
			linePoint = points[ index ];

			dir = -linePoint - glm::dot( -linePoint, lineDir ) * lineDir;
		}
		else    //Over point 2
		{
			dir = -points[ index ];
		}
		break;
	case 3:
		glm::vec3 n34b( glm::cross( bNormal, points[ 4 ] - points[ 3 ] ) );
		glm::vec3 n30b( glm::cross( bNormal, points[ 3 ] - points[ 0 ] ) );
		glm::vec3 n35a( glm::cross( aNormal, points[ 3 ] - points[ 5 ] ) );
		glm::vec3 n30a( glm::cross( aNormal, points[ 0 ] - points[ 3 ] ) );
		glm::vec3 n34Bot( glm::cross( botNormal, points[ 3 ] - points[ 4 ] ) );
		glm::vec3 n35Bot( glm::cross( botNormal, points[ 5 ] - points[ 3 ] ) );

		//Check if Over b
		if( glm::dot( bNormal, -points[ index ] ) > 0.0f &&
			glm::dot( n34b, -points[ index ] ) < 0.0f &&
			glm::dot( n30b, -points[ index ] ) < 0.0f )
		{
			bNormal = glm::normalize( bNormal );
			dir = glm::dot( bNormal, -points[ index ] ) * bNormal;
		}
		//Check if Over a
		else if( glm::dot( aNormal, -points[ index ] ) > 0.0f &&
			glm::dot( n35a, -points[ index ] ) < 0.0f &&
			glm::dot( n30a, -points[ index ] ) < 0.0f )
		{
			aNormal = glm::normalize( aNormal );
			dir = glm::dot( aNormal, -points[ index ] ) * aNormal;
		}
		//Check if Over Bot
		else if( glm::dot( botNormal, -points[ index ] ) > 0.0f &&
			glm::dot( n34Bot, -points[ index ] ) < 0.0f &&
			glm::dot( n35Bot, -points[ index ] ) < 0.0f )
		{
			botNormal = glm::normalize( botNormal );
			dir = glm::dot( botNormal, -points[ index ] ) * botNormal;
		}
		//Check if Over Line 3->4
		else if( glm::dot( n34b, -points[ index ] ) > 0.0f &&
			glm::dot( n34Bot, -points[ index ] ) > 0.0f &&
			glm::dot( points[ 4 ] - points[ 3 ], -points[ index ] ) > 0.0f )
		{
			lineDir = glm::normalize( points[ 4 ] - points[ 3 ] );
			linePoint = points[ index ];

			dir = -linePoint - glm::dot( -linePoint, lineDir ) * lineDir;
		}
		//Check if Over Line 3->5
		else if( glm::dot( n35Bot, -points[ index ] ) > 0.0f &&
			glm::dot( n35a, -points[ index ] ) > 0.0f &&
			glm::dot( points[ 5 ] - points[ 3 ], -points[ index ] ) > 0.0f )
		{
			lineDir = glm::normalize( points[ 5 ] - points[ 3 ] );
			linePoint = points[ index ];

			dir = -linePoint - glm::dot( -linePoint, lineDir ) * lineDir;
		}
		//Check if Over Line 3->0
		else if( glm::dot( n30a, -points[ index ] ) > 0.0f &&
			glm::dot( n30b, -points[ index ] ) > 0.0f &&
			glm::dot( points[ 0 ] - points[ 3 ], -points[ index ] ) > 0.0f )
		{
			lineDir = glm::normalize( points[ 0 ] - points[ 3 ] );
			linePoint = points[ index ];

			dir = -linePoint - glm::dot( -linePoint, lineDir ) * lineDir;
		}
		else    //Over point 3
		{
			dir = -points[ index ];
		}
		break;
	case 4:
		glm::vec3 n45c( glm::cross( cNormal, points[ 5 ] - points[ 4 ] ) );
		glm::vec3 n41c( glm::cross( cNormal, points[ 4 ] - points[ 1 ] ) );
		glm::vec3 n43b( glm::cross( bNormal, points[ 4 ] - points[ 3 ] ) );
		glm::vec3 n41b( glm::cross( bNormal, points[ 1 ] - points[ 4 ] ) );
		glm::vec3 n45Bot( glm::cross( botNormal, points[ 4 ] - points[ 5 ] ) );
		glm::vec3 n43Bot( glm::cross( botNormal, points[ 3 ] - points[ 4 ] ) );

		//Check if Over c
		if( glm::dot( cNormal, -points[ index ] ) > 0.0f &&
			glm::dot( n45c, -points[ index ] ) < 0.0f &&
			glm::dot( n41c, -points[ index ] ) < 0.0f )
		{
			cNormal = glm::normalize( cNormal );
			dir = glm::dot( cNormal, -points[ index ] ) * cNormal;
		}
		//Check if Over b
		else if( glm::dot( bNormal, -points[ index ] ) > 0.0f &&
			glm::dot( n43b, -points[ index ] ) < 0.0f &&
			glm::dot( n41b, -points[ index ] ) < 0.0f )
		{
			bNormal = glm::normalize( bNormal );
			dir = glm::dot( bNormal, -points[ index ] ) * bNormal;
		}
		//Check if Over Bot
		else if( glm::dot( botNormal, -points[ index ] ) > 0.0f &&
			glm::dot( n45Bot, -points[ index ] ) < 0.0f &&
			glm::dot( n43Bot, -points[ index ] ) < 0.0f )
		{
			botNormal = glm::normalize( botNormal );
			dir = glm::dot( botNormal, -points[ index ] ) * botNormal;
		}
		//Check if Over Line 4->5
		else if( glm::dot( n45c, -points[ index ] ) > 0.0f &&
			glm::dot( n45Bot, -points[ index ] ) > 0.0f &&
			glm::dot( points[ 5 ] - points[ 4 ], -points[ index ] ) > 0.0f )
		{
			lineDir = glm::normalize( points[ 5 ] - points[ 4 ] );
			linePoint = points[ index ];

			dir = -linePoint - glm::dot( -linePoint, lineDir ) * lineDir;
		}
		//Check if Over Line 4->3
		else if( glm::dot( n43Bot, -points[ index ] ) > 0.0f &&
			glm::dot( n43b, -points[ index ] ) > 0.0f &&
			glm::dot( points[ 3 ] - points[ 4 ], -points[ index ] ) > 0.0f )
		{
			lineDir = glm::normalize( points[ 3 ] - points[ 4 ] );
			linePoint = points[ index ];

			dir = -linePoint - glm::dot( -linePoint, lineDir ) * lineDir;
		}
		//Check if Over Line 4->1
		else if( glm::dot( n41c, -points[ index ] ) > 0.0f &&
			glm::dot( n41b, -points[ index ] ) > 0.0f &&
			glm::dot( points[ 1 ] - points[ 4 ], -points[ index ] ) > 0.0f )
		{
			lineDir = glm::normalize( points[ 1 ] - points[ 4 ] );
			linePoint = points[ index ];

			dir = -linePoint - glm::dot( -linePoint, lineDir ) * lineDir;
		}
		else    //Over point 4
		{
			dir = -points[ index ];
		}
		break;
	case 5:
		glm::vec3 n53a( glm::cross( aNormal, points[ 3 ] - points[ 5 ] ) );
		glm::vec3 n52a( glm::cross( aNormal, points[ 5 ] - points[ 2 ] ) );
		glm::vec3 n54c( glm::cross( cNormal, points[ 5 ] - points[ 4 ] ) );
		glm::vec3 n52c( glm::cross( cNormal, points[ 2 ] - points[ 5 ] ) );
		glm::vec3 n53Bot( glm::cross( botNormal, points[ 5 ] - points[ 3 ] ) );
		glm::vec3 n54Bot( glm::cross( botNormal, points[ 4 ] - points[ 5 ] ) );

		//Check if Over a
		if( glm::dot( aNormal, -points[ index ] ) > 0.0f &&
			glm::dot( n53a, -points[ index ] ) < 0.0f &&
			glm::dot( n52a, -points[ index ] ) < 0.0f )
		{
			aNormal = glm::normalize( aNormal );
			dir = glm::dot( aNormal, -points[ index ] ) * aNormal;
		}
		//Check if Over c
		else if( glm::dot( cNormal, -points[ index ] ) > 0.0f &&
			glm::dot( n54c, -points[ index ] ) < 0.0f &&
			glm::dot( n52c, -points[ index ] ) < 0.0f )
		{
			cNormal = glm::normalize( cNormal );
			dir = glm::dot( cNormal, -points[ index ] ) * cNormal;
		}
		//Check if Over Bot
		else if( glm::dot( botNormal, -points[ index ] ) > 0.0f &&
			glm::dot( n53Bot, -points[ index ] ) < 0.0f &&
			glm::dot( n54Bot, -points[ index ] ) < 0.0f )
		{
			botNormal = glm::normalize( botNormal );
			dir = glm::dot( botNormal, -points[ index ] ) * botNormal;
		}
		//Check if Over Line 5->3
		else if( glm::dot( n53a, -points[ index ] ) > 0.0f &&
			glm::dot( n53Bot, -points[ index ] ) > 0.0f &&
			glm::dot( points[ 3 ] - points[ 5 ], -points[ index ] ) > 0.0f )
		{
			lineDir = glm::normalize( points[ 3 ] - points[ 5 ] );
			linePoint = points[ index ];

			dir = -linePoint - glm::dot( -linePoint, lineDir ) * lineDir;
		}
		//Check if Over Line 5->4
		else if( glm::dot( n54Bot, -points[ index ] ) > 0.0f &&
			glm::dot( n54c, -points[ index ] ) > 0.0f &&
			glm::dot( points[ 4 ] - points[ 5 ], -points[ index ] ) > 0.0f )
		{
			lineDir = glm::normalize( points[ 4 ] - points[ 5 ] );
			linePoint = points[ index ];

			dir = -linePoint - glm::dot( -linePoint, lineDir ) * lineDir;
		}
		//Check if Over Line 5->2
		else if( glm::dot( n52a, -points[ index ] ) > 0.0f &&
			glm::dot( n52c, -points[ index ] ) > 0.0f &&
			glm::dot( points[ 2 ] - points[ 5 ], -points[ index ] ) > 0.0f )
		{
			lineDir = glm::normalize( points[ 2 ] - points[ 5 ] );
			linePoint = points[ index ];

			dir = -linePoint - glm::dot( -linePoint, lineDir ) * lineDir;
		}
		else    //Over point 5
		{
			dir = -points[ index ];
		}
		break;
	}

	dist = mRadius - glm::sqrt( dir.x * dir.x + dir.y * dir.y + dir.z * dir.z );
	value.setNormal( dir );
	value.setDistanceSquared( dist * dist );

	return value;
}*/


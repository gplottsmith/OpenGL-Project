
#include "BinarySpacePartition.h"
#include "Block/Block.h"


BinarySpacePartitionElement::BinarySpacePartitionElement( ) :
	mCenter( ), mHalfWidth( 0 ), mBlocks( ),
	mSubSpace( ), mDepth( 0 )
{
}

BinarySpacePartitionElement::BinarySpacePartitionElement( const glm::vec3& c, float r ) :
	mCenter( c ), mHalfWidth( r ), mBlocks( ), mSubSpace( ), mDepth( 0 )
{
}

BinarySpacePartitionElement::BinarySpacePartitionElement( float x, float y, float z, float r ) :
	mCenter( x, y, z ), mHalfWidth( r ), mBlocks( ), mSubSpace( ), mDepth( 0 )
{
}

BinarySpacePartitionElement::~BinarySpacePartitionElement( )
{
}

void BinarySpacePartitionElement::initSubSpace( int maxDepth, int currentDepth, const std::vector< const Block* >& blocks, const BlockCapsule& player )
{
	if( currentDepth + 1 == maxDepth ) // This Node is a leaf, init mBlocks
	{
		// Just a guess at how many blocks will need to be added on avg 12/19/2022
		mBlocks.reserve( blocks.size( ) / ( 2 * ( currentDepth + 1 ) ) );

		for( unsigned int i = 0; i < blocks.size( ); i++ )
		{
			const AABoundingBox* box ( blocks[ i ]->getAABBAdr( ) );
			if( boxOverlaps( *box, player ) )
				mBlocks.emplace_back( blocks.at( i ) );
		}
	}
	else
	{
		std::vector< const Block* > partialBlocks;
		partialBlocks.reserve( blocks.size( ) / ( 2 * ( currentDepth + 1 ) ) );
		for( unsigned int i = 0; i < blocks.size( ); i++ )
		{
			const AABoundingBox* box( blocks[ i ]->getAABBAdr( ) );
			if( boxOverlaps( *box, player ) )
				partialBlocks.emplace_back( blocks[ i ] );
		}
		if( partialBlocks.empty( ) )
			return;

		float offset = mHalfWidth / 2;
		mSubSpace.reserve( 8 );
		mSubSpace.emplace_back( mCenter.x - offset, mCenter.y + offset, mCenter.z - offset, offset );
		mSubSpace.emplace_back( mCenter.x - offset, mCenter.y + offset, mCenter.z + offset, offset );
		mSubSpace.emplace_back( mCenter.x - offset, mCenter.y - offset, mCenter.z - offset, offset );
		mSubSpace.emplace_back( mCenter.x - offset, mCenter.y - offset, mCenter.z + offset, offset );
		mSubSpace.emplace_back( mCenter.x + offset, mCenter.y + offset, mCenter.z - offset, offset );
		mSubSpace.emplace_back( mCenter.x + offset, mCenter.y + offset, mCenter.z + offset, offset );
		mSubSpace.emplace_back( mCenter.x + offset, mCenter.y - offset, mCenter.z - offset, offset );
		mSubSpace.emplace_back( mCenter.x + offset, mCenter.y - offset, mCenter.z + offset, offset );

		for( int i = 0; i < 8; i++ )
		{
			mSubSpace[ i ].initSubSpace( maxDepth, currentDepth + 1, partialBlocks, player );
			//mSubSpace[ i ]->printData( );
		}
	}
}

bool BinarySpacePartitionElement::isContactingSphere( const glm::vec3& sCenter, float radius ) const
{
	if( mCenter == sCenter )
		return true;

	// pos is a point on the Sphere Edge on the line from its center to Cube center
	glm::vec3 pos = glm::normalize( mCenter - sCenter ) * radius + sCenter;

	// Case where Edge of Sphere intersects Cube
	if( inCube( pos ) )
		return true;

	// Case where Sphere Encloses Cube
	if( glm::dot( pos - sCenter, pos - sCenter ) > glm::dot( mCenter - sCenter, mCenter - sCenter ) )
		return true;

	return false;
}

bool BinarySpacePartitionElement::boxOverlaps( const AABoundingBox& box, const BlockCapsule& player ) const
{
	float xzOverSpill = player.getRadius( );
	float yOverSpill = player.getRadius( ) + player.getHalfHeight( );

	glm::vec3 bCenter = box.getCenter( );
	glm::vec3 bMax = box.getMax( ) + bCenter;
	glm::vec3 bMin = box.getMin( ) + bCenter;

	if( bMax.x > mCenter.x - ( mHalfWidth + xzOverSpill ) )
	{
		if( bMin.x < mCenter.x + ( mHalfWidth + xzOverSpill ) )
		{
			if( bMax.z > mCenter.z - ( mHalfWidth + xzOverSpill ) )
			{
				if( bMin.z < mCenter.z + ( mHalfWidth + xzOverSpill ) )
				{
					if( bMax.y > mCenter.y - ( mHalfWidth + yOverSpill ) )
					{
						if( bMin.y < mCenter.y + ( mHalfWidth + yOverSpill ) )
						{
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}

bool BinarySpacePartitionElement::inCube( const glm::vec3& point ) const
{
	float edge( mCenter.x - mHalfWidth );

	if( point.x >= edge )
	{
		edge = mCenter.x + mHalfWidth;

		if( point.x <= edge )
		{
			edge = mCenter.y - mHalfWidth;

			if( point.y >= edge )
			{
				edge = mCenter.y + mHalfWidth;

				if( point.y <= edge )
				{
					edge = mCenter.z - mHalfWidth;

					if( point.z >= edge )
					{
						edge = mCenter.z + mHalfWidth;

						if( point.z <= edge )
						{
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}

std::vector< const Block* > BinarySpacePartitionElement::getBlocksInLeaf( const glm::vec3& point ) const
{
	if( mSubSpace.size( ) == 0 )
	{
		if( inCube( point ) )
		{
			return mBlocks;
		}

		return std::vector< const Block* >( ); // Block is not in BSP
	}

	for( int i = 0; i < 8; i++ )
	{
		if( mSubSpace[ i ].inCube( point ) )
			return mSubSpace[ i ].getBlocksInLeaf( point );
	}

	return std::vector< const Block* >( ); // Block is not in BSP
}

void BinarySpacePartitionElement::printData( ) const
{
	std::cout << "Corner Positions: \n"
		<< "#1: ( " << mCenter.x - mHalfWidth << ", " << mCenter.y + mHalfWidth << ", " << mCenter.z - mHalfWidth << " )\n"
		<< "#2: ( " << mCenter.x - mHalfWidth << ", " << mCenter.y + mHalfWidth << ", " << mCenter.z + mHalfWidth << " )\n"
		<< "#3: ( " << mCenter.x - mHalfWidth << ", " << mCenter.y - mHalfWidth << ", " << mCenter.z - mHalfWidth << " )\n"
		<< "#4: ( " << mCenter.x - mHalfWidth << ", " << mCenter.y - mHalfWidth << ", " << mCenter.z + mHalfWidth << " )\n"
		<< "#5: ( " << mCenter.x + mHalfWidth << ", " << mCenter.y + mHalfWidth << ", " << mCenter.z - mHalfWidth << " )\n"
		<< "#6: ( " << mCenter.x + mHalfWidth << ", " << mCenter.y + mHalfWidth << ", " << mCenter.z + mHalfWidth << " )\n"
		<< "#7: ( " << mCenter.x + mHalfWidth << ", " << mCenter.y - mHalfWidth << ", " << mCenter.z - mHalfWidth << " )\n"
		<< "#8: ( " << mCenter.x + mHalfWidth << ", " << mCenter.y - mHalfWidth << ", " << mCenter.z + mHalfWidth << " )\n";

	std::cout << "Center Point: ( " << mCenter.x << ", " << mCenter.y << ", " << mCenter.z << " )\n\n";
	
	std::cout << "This element has " << mBlocks.size( ) << " Blocks inside it.\n";
}




void BinarySpacePartition::initAll( float halfWidth, int depth, const std::vector< const Block* >& blocks, const BlockCapsule& player )
{
	mWorldHalfWidth = halfWidth;
	mMaxDepth = depth;

	mRoot.setHalfWidth( halfWidth );
	mRoot.initSubSpace( mMaxDepth, 0, blocks, player );
}

std::vector< const Block* > BinarySpacePartition::getBlocksInLeaf( const glm::vec3& point ) const
{
	std::vector< const Block* > vec = mRoot.getBlocksInLeaf( point );

	return vec;
}

BinarySpacePartition::BinarySpacePartition( float worldHalfWidth, int maxDepth, const std::vector< const Block* >& blocks, const BlockCapsule& player ) :
	mWorldHalfWidth( worldHalfWidth ), mMaxDepth( maxDepth ), mRoot( glm::vec3( ), mWorldHalfWidth )
{

	mRoot.initSubSpace( mMaxDepth, 0, blocks, player );
}

BinarySpacePartition::BinarySpacePartition( ) :
	mWorldHalfWidth( 0.0f ), mMaxDepth( 0 ), mRoot( glm::vec3( ), mWorldHalfWidth )
{
}

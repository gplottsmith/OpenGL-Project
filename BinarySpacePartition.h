


#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


// NEW 7/6/2022: BSP Is a class full of BSPElements, which are cubes that store addresses of Blocks that intersect it.

class Block;
class BlockCapsule;
struct AABoundingBox;

class BinarySpacePartitionElement
{
public:
	BinarySpacePartitionElement( );
	BinarySpacePartitionElement( const glm::vec3& c, float r );
	BinarySpacePartitionElement( float x, float y, float z, float r );
	~BinarySpacePartitionElement( );

	void initSubSpace( int maxDepth, int currentDepth, const std::vector< const Block* >& blocks, const BlockCapsule& player );

	bool isContactingSphere( const glm::vec3& sCenter, float radius ) const;
	bool boxOverlaps( const AABoundingBox& box, const BlockCapsule& player ) const;
	bool inCube( const glm::vec3& point ) const;

	std::vector< const Block* > getBlocksInLeaf( const glm::vec3& point ) const;

	inline void setCenter( const glm::vec3& c ) { mCenter = c; }
	inline void setHalfWidth( float w ) { mHalfWidth = w; }

	inline glm::vec3 getCenter( void ) const { return mCenter; }
	inline float getHalfWidth( void ) const { return mHalfWidth; }

	void printData( ) const;

private:

	glm::vec3 mCenter;
	float mHalfWidth;
	
	std::vector< const Block* > mBlocks;

	std::vector< BinarySpacePartitionElement >mSubSpace;
	int mDepth;
};


class BinarySpacePartition
{
public:

	void initAll( float halfWidth, int depth, const std::vector< const Block* >& blocks, const BlockCapsule& player );

	std::vector< const Block* > getBlocksInLeaf( const glm::vec3& point ) const;

	BinarySpacePartition( float worldHalfWidth, int maxDepth, const std::vector< const Block* >& blocks, const BlockCapsule& player );
	BinarySpacePartition( );

private:

	float mWorldHalfWidth;
	int mMaxDepth;
	BinarySpacePartitionElement mRoot;
};
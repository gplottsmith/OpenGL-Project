#pragma once


#include "Block.h"


class GameObject
{
public:

	bool colliderRayIntersects( const glm::vec3& rayPoint, const glm::vec3& rayDir, glm::vec3* hit ) const;

	inline const Block& getModel( ) const { return mModel; }
	inline const std::vector< Block >& getColliders( ) const { return mColliders; }
	inline const Block* getColliderAddressAt( uint32_t index ) const { return &mColliders[ index ]; }
	inline uint32_t getCollidersSize( ) const { return mColliders.size( ); }
	inline glm::mat4 getTransform( ) const { return mColliders[ 0 ].getCombinedTransform( ); }
	inline glm::vec3 getTranslation( ) const { return mColliders[ 0 ].getTranslation( ); }
	inline const Quaternion& getRotation( ) const { return mColliders[ 0 ].getRotation( ); }
	inline float getScaleFactor( ) const { return mColliders[ 0 ].getScaleFactor( ); }

	uint32_t getCollidersVertexCount( ) const;

	void updateTransform( const glm::vec4& t, const Quaternion& r, const float& s );
	void setStatic( bool s );

	GameObject( );
	GameObject( const GameObject& other );
	GameObject( const std::string& modelFile, const std::string& colliderFile );
	GameObject( const std::string& colliderFile );
	GameObject( const std::vector< Block >& colliders );

protected:
	Block mModel;
	std::vector< Block > mColliders;

};
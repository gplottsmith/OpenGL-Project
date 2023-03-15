#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Quaternion
{
private:
	float x;
	float y;
	float z;
	float w;
public:
	Quaternion();
	Quaternion(float X, float Y, float Z, float W);
	Quaternion( const glm::vec3& axis, const float angle );
	Quaternion( const Quaternion& other );
	~Quaternion();

	Quaternion operator=( const Quaternion& that );
	bool operator==( const Quaternion& that ) const;
	bool operator!=( const Quaternion& that ) const;
	Quaternion operator*( const Quaternion& that ) const;

	inline glm::vec4 toVec4( ) const { return glm::vec4( x, y, z, w ); }

	void dX(const float amount);
	void dY(const float amount);
	void dZ(const float amount);
	void dW(const float amount);

	void composeWith( const Quaternion& q );

	void print() const;

	Quaternion getInverse() const;

	Quaternion hamiltonianProduct( const Quaternion& q ) const;

	glm::vec3 getImaginaryComponent( void ) const;

	inline float getX( ) const { return x; }
	inline float getY( ) const { return y; }
	inline float getZ( ) const { return z; }
	inline float getW( ) const { return w; }

	inline float getAngle( void ) const { return 2 * glm::acos( w ); }
	glm::vec3 getAxis( void ) const;

	inline void invert( ) { w = -w; }

	void rotate( glm::vec3& vec ) const;
	glm::vec4 rotate( const glm::vec4& pos ) const;
	glm::vec3 getRotatedVec3( const glm::vec3& vec );
	glm::mat4 toMat4() const;
};
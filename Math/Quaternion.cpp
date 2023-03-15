/* Made 6/23/19 */

#include "Quaternion.h"
#include <iostream>

Quaternion::Quaternion() :
	x( 0.0f ),
	y( 0.0f ),
	z( 0.0f ),
	w( 1.0f )
{
}

Quaternion::Quaternion(float X, float Y, float Z, float W) :
	x(X),
	y(Y),
	z(Z),
	w(W)
{
}

Quaternion::Quaternion( const glm::vec3& axis, const float angle ) :
	w( glm::cos( angle / 2 ) )
{
	glm::vec3 normalizedAxis ( glm::normalize( axis ) );

	x = normalizedAxis.x * glm::sin( angle / 2 );
	y = normalizedAxis.y * glm::sin( angle / 2 );
	z = normalizedAxis.z * glm::sin( angle / 2 );
}

Quaternion::Quaternion( const Quaternion& other ) :
	x( other.x ),
	y( other.y ),
	z( other.z ),
	w( other.w )
{

}

Quaternion::~Quaternion() 
{
}

Quaternion Quaternion::operator=( const Quaternion& that )
{
	x = that.x;
	y = that.y;
	z = that.z;
	w = that.w;

	return *this;
}

bool Quaternion::operator==( const Quaternion& that ) const
{
	return x == that.x && y == that.y && z == that.z && w == that.w;
}
bool Quaternion::operator!=( const Quaternion& that ) const
{
	return x != that.x || y != that.y || z != that.z || w != that.w;
}

Quaternion Quaternion::operator*( const Quaternion& that ) const
{
	//Rodrigue's Formula for composite rotations
	float alpha( getAngle( ) );
	float beta( that.getAngle( ) );
	float gamma( 0 );

	glm::vec3 A( getAxis( ) );
	glm::vec3 B( that.getAxis( ) );
	glm::vec3 C( 1.0f );

	glm::vec3 temp( 1.0f );
	temp = ( glm::tan( beta / 2 ) * B + glm::tan( alpha / 2 ) * A + tan( beta / 2 ) * tan( alpha / 2 ) * glm::cross( B, A ) ) /
					( 1 - glm::tan( beta / 2 ) * glm::tan( alpha / 2 ) * glm::dot( B, A ) );

	float tanGammaOverTwo = glm::sqrt( C.x * C.x + C.y * C.y + C.z * C.z );

	gamma = 2 * glm::atan( tanGammaOverTwo );
	C = glm::vec3( temp.x / tanGammaOverTwo, temp.y / tanGammaOverTwo, temp.z / tanGammaOverTwo );

	return Quaternion( C, gamma );//TODO: Test that multiplication of Quaternions returns Composite Rotations  2/23/20
}


glm::vec3 Quaternion::getImaginaryComponent( ) const
{
	return glm::vec3( x, y, z );
}

void Quaternion::dX(const float amount)
{
	x += amount;
	float length = sqrt(x * x + y * y + z * z + w * w);
	x /= length;
	y /= length;
	z /= length;
	w /= length;
}

void Quaternion::dY(const float amount)
{
	y += amount;
	float length = sqrt(x * x + y * y + z * z + w * w);
	x /= length;
	y /= length;
	z /= length;
	w /= length;
}

void Quaternion::dZ(const float amount)
{
	z += amount;
	float length = sqrt(x * x + y * y + z * z + w * w);
	x /= length;
	y /= length;
	z /= length;
	w /= length;
}

void Quaternion::dW(const float amount)
{
	w += amount;
	float length = sqrt(x * x + y * y + z * z + w * w);
	x /= length;
	y /= length;
	z /= length;
	w /= length;
}

void Quaternion::composeWith( const Quaternion& q )
{
	Quaternion composition = q.hamiltonianProduct( *this );
	*this = composition;
}

void Quaternion::print() const
{
	std::cout << 
		"\nX: " << x <<
		"\nY: " << y <<
		"\nZ: " << z <<
		"\nW: " << w <<
		"\nLength: " << sqrt(x * x + y * y + z * z + w * w) << std::endl;
}

Quaternion Quaternion::getInverse() const
{
	return Quaternion(-x, -y, -z, w);
}

Quaternion Quaternion::hamiltonianProduct( const Quaternion& q ) const
{
	//TODO 2/17/20: Incorporate quaternion Rotations using Hamilton Product into BlockTransform.
	float X = ( w * q.x + x * q.w + y * q.z - z * q.y );
	float Y = ( w * q.y - x * q.z + y * q.w + z * q.x );
	float Z = ( w * q.z + x * q.y - y * q.x + z * q.w );
	float W = ( w * q.w - x * q.x - y * q.y - z * q.z );

	return Quaternion( X, Y, Z, W );
}

glm::vec3 Quaternion::getAxis( void ) const
{
	float denom = glm::sin( getAngle( ) / 2 );

	//Denom will only be 0 if the angle of rotation is 0. In this case, we arbitrarily return an axis of rotation about the X axis,
	//since this quaternion doesn't represent a rotation.

	if( denom )
		return glm::vec3( x / denom, y / denom, z / denom );

	else
		return glm::vec3( 1.0f, 0.0f, 0.0f );
}

void Quaternion::rotate( glm::vec3& vec ) const
{
	Quaternion vecToQuaternion( vec.x, vec.y, vec.z, 0.0f );

	vecToQuaternion = vecToQuaternion.hamiltonianProduct( getInverse( ) );
	vecToQuaternion = hamiltonianProduct( vecToQuaternion );

	vec = vecToQuaternion.getImaginaryComponent( );
}

glm::vec3 Quaternion::getRotatedVec3( const glm::vec3& vec )
{
	Quaternion vecToQuaternion( vec.x, vec.y, vec.z, 0.0f );

	vecToQuaternion = vecToQuaternion.hamiltonianProduct( getInverse( ) );
	vecToQuaternion = hamiltonianProduct( vecToQuaternion );

	return vecToQuaternion.getImaginaryComponent( );
}

glm::vec4 Quaternion::rotate( const glm::vec4& vec ) const
{ 
	Quaternion vecToQuaternion( vec.x, vec.y, vec.z, 0.0f );

	vecToQuaternion = hamiltonianProduct( vecToQuaternion );
	vecToQuaternion = vecToQuaternion.hamiltonianProduct( getInverse( ) );

	return glm::vec4( vecToQuaternion.getImaginaryComponent( ), vec.w );
}

glm::mat4 Quaternion::toMat4() const
{
	return glm::mat4(1.0f - 2.0f * y * y - 2.0f * z * z, 2.0f * x * y - 2.0f * z * w, 2.0f * x * z + 2.0f * y * w, 0.0f,
					 2.0f * x * y + 2.0f * z * w, 1.0f - 2.0f * x * x - 2.0f * z * z, 2.0f * y * z - 2.0f * x * w, 0.0f,
					 2.0f * x * z - 2.0f * y * w, 2.0f * y * z + 2.0f * x * w, 1.0f - 2.0f * x * x - 2.0f * y * y, 0.0f,
					 0.0f,                        0.0f,                        0.0f,                               1.0f);
}






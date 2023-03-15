
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>

namespace Util
{
	static void printVec3( const glm::vec3& vec )
	{
		std::cout << "( " << vec.x << ", " << vec.y << ", " << vec.z << " )";
	}

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
			if( p1 )
				*p1 = glm::vec3( INFINITY );
			if( p2 )
				*p2 = glm::vec3( INFINITY );
		}
	}
}

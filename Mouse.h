
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glfw3.h"

// Created 2/9/2023
class Mouse
{
public:
	Mouse( );
	~Mouse( );

	void update( GLFWwindow* window );
	inline bool isLPressed( ) const { return mLeftState == GLFW_PRESS && !mLHeld; }
	inline bool isRPressed( ) const { return mRightState == GLFW_PRESS && !mRHeld; }
	inline bool isLHeld( ) const { return mLHeld; }
	inline bool isRHeld( ) const { return mRHeld; }
	inline const glm::vec2& getPosition( ) const { return mPos; }
	inline float getX( ) const { return mPos.x; }
	inline float getY( ) const { return mPos.y; }

private:
	// Mouse Position is normalized, with 1 being the right side of the screen, 
	// -1 being the left side, 1 being the top, and -1 being the bottom.
	glm::vec2 mPos;
	int mLeftState;
	int mRightState;
	bool mLHeld;
	bool mRHeld;
};

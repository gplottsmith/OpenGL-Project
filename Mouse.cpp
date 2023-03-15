#include "Mouse.h"

Mouse::Mouse( ) :
	mPos( ), mLeftState( GLFW_RELEASE ), mRightState( GLFW_RELEASE ), mLHeld( false ), mRHeld( false )
{
}

Mouse::~Mouse( )
{
}

void Mouse::update( GLFWwindow* window )
{
	double xMouse, yMouse;
	glfwGetCursorPos( window, &xMouse, &yMouse );
	int width, height;
	glfwGetWindowSize( window, &width, &height );

	mPos.x = ( float ) xMouse - width / 2;
	mPos.y = -( float ) yMouse + height / 2;

	int state = glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_LEFT );
	if( state == GLFW_PRESS )
	{
		if( mLeftState == GLFW_RELEASE )
			mLeftState = GLFW_PRESS;
		else
			mLHeld = true;
	}
	else
	{
		mLeftState = GLFW_RELEASE;
		mLHeld = false;
	}
	state = glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_RIGHT );
	if( state == GLFW_PRESS )
	{
		if( mRightState == GLFW_RELEASE )
			mRightState = GLFW_PRESS;
		else
			mRHeld = true;
	}
	else
	{
		mRightState = GLFW_RELEASE;
		mRHeld = false;
	}
}


#pragma once

#include "glm/glm.hpp"
#include <iostream>

//Cubic Bezier curve

template < class T >

class BezierCurve
{
public:


	// t must be a value between 1 and 0
	T interpolation( const float t ) const
	{
		if( t <= 1 && t >= 0 )
			return ( std::powf( 1.0f - t, 3 ) * p0 ) + ( 3 * std::powf( 1.0f - t, 2 ) * t * p1 ) 
				 + ( 3 * ( 1.0f - t ) * std::powf( t, 2 ) * p2 ) + ( std::powf( t, 3 ) * p3 );

		std::cout << "\nError: Bezier Interpolation recieved out of bounds time step.\n";
		return T( );
	}

	BezierCurve< T >( const T& point0, const T& point1, const T& point2, const T& point3 ) :
		p0( point0 ), p1( point1 ), p2( point2 ), p3( point3 )
	{
	}

	BezierCurve( ) : p0( ), p1( ), p2( ), p3( )
	{
	}

	~BezierCurve( )
	{

	}

protected:
	T p0;
	T p1;
	T p2;
	T p3;
};
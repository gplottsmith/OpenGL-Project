#pragma once
#include  <iostream>
#include <fstream>
#include <sys/stat.h>
#include <queue>
#include "math.h"

#define ZLIB_WINAPI
#include "Vendor/zlib/zlib.h"


	// NEW 7/3/2022: Contains data needed for constructing a Texture or Color
struct Material
{
public:
	enum MatType { image, color };

	Material( const glm::vec4& c ) :
		mColor( c ),
		mImgName( ),
		mType( color ),
		mCount( 0 )
	{
	}
	Material( const std::string& s ) :
		mColor( ),
		mImgName( s ),
		mType( image ),
		mCount( 0 )
	{
	}
	inline glm::vec4 getColor( void ) const { return mColor; }
	inline std::string getImgName( void ) const { return mImgName; }
	inline MatType getType( void ) const { return mType; }
	inline int getCount( void ) const { return mCount; }

	inline void setCount( int c ) { mCount = c; }

protected:
	glm::vec4 mColor;
	std::string mImgName;
	MatType mType;
	int mCount;  // Count of Faces using this material
};

class DAEFileParser
{


public:
	DAEFileParser( const char* filepath ) :
		mFilePath( filepath ),
		mBuffer( nullptr ),
		mBufferSize( 0 ),
		mVertexBufferLength( 0 ),
		mIndexBufferLength( 0 ),
		mNormalElementCount( 0 ),
		mTextureCount( 0 )
	{
		//Inits buffer with the entire .dae file
		std::ifstream file( mFilePath, std::ios::in, std::ios::binary );
		struct stat results;
		if( stat( mFilePath, &results ) != 0 )
			std::cout << "Error: could not find fileSize.";
		else
			mBufferSize = results.st_size;
		mBuffer = new char[ mBufferSize ];
		file.seekg( 0 );
		file.read( mBuffer, mBufferSize );
		file.close( );
	}

	int getMeshCount( ) const
	{
		int i = 0;
		int meshCount = 0;

		while( i < mBufferSize )
		{
			i = findStringEnd( "<mesh>", 6, i );
			meshCount++;
		}

		return meshCount - 1;
	}

	void initUVArray( std::vector< float >& uvCoords, int arrayCount )
	{
		initFloatArray( "-mesh-map-0-array", 17, uvCoords, nullptr, arrayCount );
	}			

	void initVertexElements( std::vector< float >& vertices, int arrayCount )
	{
		initFloatArray( "-mesh-positions-array", 21, vertices, &mVertexBufferLength, arrayCount );
	}

	void initNormalArray( std::vector< float >& normals, int arrayCount )
	{
		initFloatArray( "-mesh-normals-array", 19, normals, nullptr, arrayCount );
	}

	//The indices for the vertex array, faceNormals array, and UV array are all retrieved at once.
	void initIndexArrays( int** vertexIndices, int** faceNormalsIndices, int** uvIndices, int matIndex, int* thisIndexCount ) //matIndex is the # of <p> array
	{
		std::queue< char > indChar;
		std::queue< int > indInt;

		int strEnd = 0;
		for( int j = 0; j < matIndex + 1; j++ )
			strEnd = findStringEnd( "<p>", 3, strEnd );

		//Fill indInt with buffer of Indices
		for( ; mBuffer[ strEnd ] != '<'; strEnd++ )
		{
			if( mBuffer[ strEnd ] == ' ' )
				indInt.push( charToInt( indChar ) );
			
			else
				indChar.push( mBuffer[ strEnd ] );
		}
		//One last index needs to be added
		indInt.push( charToInt( indChar ) );

		//Index Array, faceNormalsIndices, and uvIndices is initialized and filled with data here
		int indexBufferSize = indInt.size( );
		int* indexBuffer = new int[ indexBufferSize ];

		for( int i = 0; i < indexBufferSize; i++ )
		{
			indexBuffer[ i ] = indInt.front( );
			indInt.pop( );
		}
		( *vertexIndices ) = new int[ indexBufferSize / 3 ];

		//NEW 6/18/2022: There can be multple index buffers in the file, every time one gets initialized the total index buffer length is increased here.
		mIndexBufferLength += indexBufferSize / 3;

		// NEW 12/22/2022: Some objects care about this particular index buffer size, otherwise pass in nullptr.
		if( thisIndexCount )
			*thisIndexCount = indexBufferSize / 3;

		( *faceNormalsIndices ) = new int[ indexBufferSize / 3 ];

		( *uvIndices ) = new int[ indexBufferSize / 3 ];

		for( int i = 0; i < indexBufferSize; i += 3 )		//Every 3 elements in indexBuffer starting from 0 is a Vertex index Component
			( *vertexIndices )[ i / 3 ] = indexBuffer[ i ];

		for( int i = 1, j = 0; i < indexBufferSize; i += 3, j++ )		//Every 3 elements in indexBuffer starting from 1 is a Face Normals index Component
			( *faceNormalsIndices )[ j ] = indexBuffer[ i ];

		for( int i = 2, j = 0; i < indexBufferSize; i += 3, j++ )		
			( *uvIndices )[ j ] = indexBuffer[ i ];
	}

	// There can be multiple different param arrays in a DAE file; arrayCount is the number of the array we want, starting from 0
	void initFloatArray( const char* paramStr, int strLen, std::vector< float >& param, int* count, int arrayCount )
	{
		int paramSize;
		std::queue< char > num;

		int i = 0;
		for( int j = 0; j <= arrayCount; j++ )
		{
			i = findStringEnd( "<float_array", 12, i );
			i = findStringEnd( paramStr, strLen, i );
		}

		i = findStringEnd( "count=", 6, i ) + 1;	//i now points at the count of the Param elements
		int temp = i;
		while( mBuffer[ i ] != '"' )
		{
			num.push( mBuffer[ i++ ] );
		}
		paramSize = charToInt( num );

		i = findStringEnd( ">", 1, i );			// i points to the begining of the Param data

		// Param buffer with size init here
		param.reserve( paramSize );
		if( count )
			*count = paramSize;
		paramSize = 0;

		//Stores all the Param Elements in ( *param )
		while( mBuffer[ i ] != '<' )
		{
			//Store the Param element in num
			while( mBuffer[ i ] != ' ' && mBuffer[ i ] != '<' )
			{
				num.push( mBuffer[ i ] );
				i++;
			}

			//Set i to point at the begining of the next number in the index data, or the '/' in '</p>'
			i++;

			param.emplace_back( charToFloat( num ) );

			if( mBuffer[ i ] == '/' )
				break;
		}
	}

	void initMaterials( std::vector< Material >& matVec )
	{
		int i = findStringEnd( "<library_effects>", 17, 0 );
		int matVecIndex = 0;
		int matEnd = findStringEnd( "</library_effects>", 18, i );
		int col;
		int tex;

		const char* imgPrefix( "res/Image Files/" );
		std::string imgName;
		imgName += imgPrefix;
		std::queue < char > numChar;
		std::queue < float > numFloat;

		while( i < matEnd )
		{
			col = findStringEnd( "<color sid=\"diffuse\">", 21, i );
			tex = findStringEnd( "<init_from>", 11, i );
			if( tex > matEnd )
				tex = INT_MAX; // The string "<init_from>" can be found elsewhere in the file

			if( tex < col )
			{
				i = tex;

				while( mBuffer[ i ] != '<' )
				{
					imgName += mBuffer[ i ];
					i++;
				}

				int j = imgName.size( ) - 1;	// 6/30/2022 Needs to be .filetype, not _filetype
				while( imgName[ j ] != '_' )
					j--;

				imgName[ j ] = '.';   

				matVec.push_back( Material( imgName ) );
				imgName.clear( );
				imgName += imgPrefix;
				mTextureCount++;	// mTexCount incremented here
			}
			else if( col < tex )
			{
				i = col;

				while( mBuffer[ i ] != '<' )
				{
					if( mBuffer[ i ] != ' ' )
						numChar.push( mBuffer[ i ] );

					else
						numFloat.push( charToFloat( numChar ) );
					i++;
				}
				numFloat.push( charToFloat( numChar ) );
				float x = numFloat.front( ); numFloat.pop( );
				float y = numFloat.front( ); numFloat.pop( );
				float z = numFloat.front( ); numFloat.pop( );
				float w = numFloat.front( ); numFloat.pop( );
				glm::vec4 color = glm::vec4( x, y, z, w );
				matVec.push_back( Material( color ) );
			}
			else
				i = INT_MAX;
		}

		if( matVec.empty( ) )  // No Material was found, assign default
			matVec.push_back( Material( glm::vec4( 0.8f, 0.8f, 0.8f, 1.0f ) ) );

		// Next, we have to set the mCount of each material in matVec. This is the count of faces that use that material.
		// The <p> arrays giving tis count are in the same order as they are declared in the materials list
		i = 0;
		i = findStringEnd( "<triangles", 10, i );
		i = findStringEnd( "count=\"", 7, i );
		while( i != INT_MAX )
		{
			while( mBuffer[ i ] != '"' )
			{
				numChar.push( mBuffer[ i ] );
				i++;
			}
			matVec.at( matVecIndex ).setCount( charToInt( numChar ) );
			matVecIndex++;

			i = findStringEnd( "<triangles", 10, i );
			i = findStringEnd( "count=\"", 7, i );
		}
	}

	int findStringEnd( const char* string, int strLength, int currentIndex ) const
	{
		int j, count;
		for( int i = currentIndex; i < mBufferSize; i++ )
		{
			j = i;
			count = 0;
			char c = mBuffer[ j ];
			while( c == string[ count ] )
			{
				j++;
				count++;
				c = mBuffer[ j ];
			}
			if( count == strLength )
				return i + strLength;
		}

		return INT_MAX;
	}

	//NEW VERSION 5/31/2022
	//6/7/2022 Added Scientific notation conversion
	float charToFloat( std::queue< char >& num )
	{
		float val( 0 ), powerOfTen( 0 ), digit( 0 ), e( 0 ); 
		bool neg( false ), beforeDec( true );
		char c;
		std::queue< char > temp;

		if( num.front( ) == '-' )
		{
			neg = true;
			num.pop( );
		}

		while( !num.empty( ) )
		{
			c = num.front( );
			if( c != '.' )
				temp.push( c );
			num.pop( );
			if( c == '.' )
				beforeDec = false;
			if( beforeDec )
				powerOfTen++;
		}
		while( !temp.empty( ) && temp.front( ) != 'e' )
		{

			digit = ( float ) temp.front( ) - '0';
			temp.pop( );
			val += ( float ) pow( 10, powerOfTen - 1 ) * digit;
			powerOfTen--;
		}
		if( !temp.empty( ) )
		{
			temp.pop( );
			e = charToFloat( temp );
		}
		val *= ( float )pow( 10, e );

		if( neg )
			return -val;

		return val;
	}

	int charToInt( std::queue< char >& num )
	{
		int val( 0 ), powerOfTen( num.size( ) ), digit( 0 );
		bool neg( false );

		if( num.front( ) == '-' )
		{
			neg = true;
			powerOfTen--;
			num.pop( );
		}
		while( !num.empty( ) )
		{
			digit = num.front( ) - '0';
			num.pop( );
			val += ( int ) pow( 10, powerOfTen - 1 ) * digit;
			powerOfTen--;
		}
		if( neg )
			val = -val;

		return val;
	}

	int getVertexComponentCount( void ) const
	{
		return mVertexBufferLength;
	}

	int getIndexCount( void ) const
	{
		return mIndexBufferLength;
	}

	int getNormalElementCount( void ) const
	{
		return mNormalElementCount;
	}

	void print( )
	{
		for( int i = 0; i < mBufferSize; i++ )
			std::cout << mBuffer[ i ];
	}

	~DAEFileParser( )
	{
		delete[ ] mBuffer;
		mBuffer = nullptr;
	}

private:
	const char* mFilePath;
	char* mBuffer;
	int mBufferSize;
	int mVertexBufferLength;
	int mIndexBufferLength;
	int mNormalElementCount;
	int mTextureCount;
};



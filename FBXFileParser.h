#pragma once
#include  <iostream>
#include <fstream>
#include <sys/stat.h>

#define ZLIB_WINAPI
#include "Vendor/zlib/zlib.h"

class FBXFileParser
{

	struct param
	{
	public:
		param( const char* n, unsigned int b, unsigned int e, char& c ) :
			name( n ),
			begin( b ),
			end( e ),
			length( e - b ),
			typeCode( c ),
			data( nullptr )
		{
		}
		void setData( char* arr )
		{
			data = &arr;
		}
		void setData( double* arr )
		{

		}
		~param( )
		{
			delete[ ] data;
			data = nullptr;
		}

	private:
		const char* name;
		unsigned int begin;
		unsigned int end;
		unsigned int length;
		char typeCode;
		void* data;
	};


public:
	FBXFileParser( const char* filepath ) :
		mFilePath( filepath ),
		mVertexBufferLength( 0 ),
		mIndexBufferLength( 0 ),
		mNormalBufferLength( 0 )
	{
	}

	void initVertexArray( double** vertices, char* buffer, unsigned int fileSize )
	{
		printAllWords( buffer, fileSize );

		for ( unsigned int i = 0; i < fileSize; i++ )
		{
			std::cout << buffer[ i ] << " ";
			if( i % 14 == 0 )
				std::cout << std::endl;
		}

		unsigned int i = findStringEnd( buffer, fileSize, "Vertices", 8, 0 );
		unsigned int uncomprSize;
		char* comprData( nullptr );
		char* uncomprData( nullptr );
		char* charTemp( nullptr );
		int decompressStatus( 0 );

		char typeCode = buffer[ i ];
		//In the fbx file structure, typecode is a 1 byte char describing the layout of the proceeding data
		switch( typeCode )
		{
		case 'd':
			unsigned int arrayLength, encoding, compressedLength;
			charTemp = new char[ 4 ]{ buffer[ ++i ], buffer[ ++i ], buffer[ ++i ], buffer[ ++i ] };
			memcpy( &arrayLength, &*charTemp, sizeof( arrayLength ) );
			delete[ ] charTemp;

			charTemp = new char[ 4 ]{ buffer[ ++i ], buffer[ ++i ], buffer[ ++i ], buffer[ ++i ] };
			memcpy( &encoding, &*charTemp, sizeof( encoding ) );
			delete[ ] charTemp;

			charTemp = new char[ 4 ]{ buffer[ ++i ], buffer[ ++i ], buffer[ ++i ], buffer[ ++i ] };
			memcpy( &compressedLength, &*charTemp, sizeof( compressedLength ) );

			uncomprSize = arrayLength * sizeof( double );
			comprData = new char[ compressedLength ];
			uncomprData = new char[ uncomprSize ];

			for( unsigned int index = 0, j = ++i; j < i + compressedLength; j++, index++ )
				comprData[ index ] = buffer[ j ];
			//Member array initialized here
			( *vertices ) = new double[ arrayLength ];
			mVertexBufferLength = arrayLength;
			if( encoding )
			{
				decompressStatus = decompressData( comprData, compressedLength, ( BYTE* ) uncomprData, uncomprSize );
			}
			else     //The Vertex data doesn't need decompression
			{
				for( unsigned int index = 0; index < compressedLength; index++ )
					uncomprData[ index ] = comprData[ index ];
			}
			break;
		default:
			std::cout << "\nError: unexpected value for typeCode.\n";
			break;
		};
		//copy uncomprData to the member array after decompression
		for( unsigned int j = 0; j < uncomprSize; j += sizeof( double ) )
		{
			char cd[ 8 ] = { uncomprData[ j ], uncomprData[ j + 1 ], uncomprData[ j + 2 ], uncomprData[ j + 3 ],
							uncomprData[ j + 4 ], uncomprData[ j + 5 ], uncomprData[ j + 6 ], uncomprData[ j + 7 ] };
			double vector3Component;
			memcpy( &vector3Component, &cd, sizeof( double ) );
			( *vertices )[ j / sizeof( double ) ] = vector3Component;
		}
		delete[ ] comprData;
		comprData = nullptr;
		delete[ ] uncomprData;
		uncomprData = nullptr;
		delete[ ] charTemp;
		charTemp = nullptr;
	}

	void initIndexArray( unsigned int** indices, char* buffer, unsigned int fileSize )
	{
		unsigned int i = findStringEnd( buffer, fileSize, "PolygonVertexIndex", 18, 0 );
		unsigned int uncomprSize;
		char* comprData( nullptr );
		char* uncomprData( nullptr );
		char* charTemp( nullptr );
		int decompressStatus( 0 );

		char typeCode = buffer[ i ];
		//In the fbx file structure, typecode is a 1 byte char describing the layout of the proceeding data
		switch( typeCode )
		{
		case 'i':
			unsigned int arrayLength, encoding, compressedLength;
			charTemp = new char[ 4 ]{ buffer[ ++i ], buffer[ ++i ], buffer[ ++i ], buffer[ ++i ] };
			memcpy( &arrayLength, &*charTemp, sizeof( arrayLength ) );
			delete[ ] charTemp;

			charTemp = new char[ 4 ]{ buffer[ ++i ], buffer[ ++i ], buffer[ ++i ], buffer[ ++i ] };
			memcpy( &encoding, &*charTemp, sizeof( encoding ) );
			delete[ ] charTemp;

			charTemp = new char[ 4 ]{ buffer[ ++i ], buffer[ ++i ], buffer[ ++i ], buffer[ ++i ] };
			memcpy( &compressedLength, &*charTemp, sizeof( compressedLength ) );

			uncomprSize = arrayLength * sizeof( unsigned int );
			comprData = new char[ compressedLength ];
			uncomprData = new char[ uncomprSize ];

			for( unsigned int index = 0, j = ++i; j < i + compressedLength; j++, index++ )
				comprData[ index ] = buffer[ j ];
			//Member array initialized here
			( *indices ) = new unsigned int[ arrayLength ];
			mIndexBufferLength = arrayLength;
			if( encoding )
			{
				decompressStatus = decompressData( comprData, compressedLength, ( BYTE* ) uncomprData, uncomprSize );
			}
			else     //The Index data doesn't need decompression
			{
				for( unsigned int index = 0; index < compressedLength; index++ )
					uncomprData[ index ] = comprData[ index ];
			}
			break;
		default:
			std::cout << "\nError: unexpected value for typeCode.\n";
			break;
		};
		//copy uncomprData to the member array after decompression
		//No Quads, only Triangles for now. Every 3 indices correspond to 1 face.
		for( unsigned int j = 0; j < uncomprSize; j += sizeof( int ) )
		{
			char ci[ 4 ] = { uncomprData[ j ], uncomprData[ j + 1 ], uncomprData[ j + 2 ], uncomprData[ j + 3 ] };
			int index( 0 );
			memcpy( &index, &ci, sizeof( int ) );
			if( index < 0 )
				index = -index - 1;
			( *indices )[ j / sizeof( int ) ] = index;
		}
		delete[ ] comprData;
		comprData = nullptr;
		delete[ ] uncomprData;
		uncomprData = nullptr;
		delete[ ] charTemp;
		charTemp = nullptr;
	}
	//   Get Normals from the file
	void initNormalArray( double** normals, char* buffer, unsigned int fileSize )
	{
		//Mapping information type can be ByVertex, ByPolygonVertex, or a few others, each of which must be dealt with differently
		// Can read about it: https://banexdevblog.wordpress.com/2014/06/23/a-quick-tutorial-about-the-fbx-ascii-format/
		unsigned int k = findStringEnd( buffer, fileSize, "MappingInformationType", 22, 0 );

		char* charTemp( nullptr );
		/*							Reads the buffer as chars, ints, floats, and doubles
		float f; int integer; double d;
		for( unsigned int i = k; i < k + 200; i++ )
		{
			charTemp = new char[ 4 ]{ buffer[ i ], buffer[ i + 1 ], buffer[ i + 2 ], buffer[ i + 3 ] };
			memcpy( &f, &*charTemp, sizeof( float ) );
			memcpy( &integer, &*charTemp, sizeof( int ) );
			charTemp = new char[ 8 ]{ buffer[ i ], buffer[ i + 1 ], buffer[ i + 2 ], buffer[ i + 3 ],
				buffer[ i + 4 ], buffer[ i + 5 ], buffer[ i + 6 ], buffer[ i + 7 ] };
			memcpy( &d, &*charTemp, sizeof( double ) );

			std::cout << "char: " << buffer[ i ] << " float: " << f << " int: " << integer << " double: " << d << std::endl;
		} */
		unsigned int i = findStringEnd( buffer, fileSize, "Normals", 7, 0 );
		unsigned int uncomprSize;
		char* comprData( nullptr );
		char* uncomprData( nullptr );
		int decompressStatus( 0 );

		char typeCode = buffer[ i ];
		//In the fbx file structure, typecode is a 1 byte char describing the layout of the proceeding data
		switch( typeCode )
		{
		case 'd':
			unsigned int arrayLength, encoding, compressedLength;
			charTemp = new char[ 4 ]{ buffer[ ++i ], buffer[ ++i ], buffer[ ++i ], buffer[ ++i ] };
			memcpy( &arrayLength, &*charTemp, sizeof( arrayLength ) );
			delete[ ] charTemp;

			charTemp = new char[ 4 ]{ buffer[ ++i ], buffer[ ++i ], buffer[ ++i ], buffer[ ++i ] };
			memcpy( &encoding, &*charTemp, sizeof( encoding ) );
			delete[ ] charTemp;

			charTemp = new char[ 4 ]{ buffer[ ++i ], buffer[ ++i ], buffer[ ++i ], buffer[ ++i ] };
			memcpy( &compressedLength, &*charTemp, sizeof( compressedLength ) );

			uncomprSize = arrayLength * sizeof( double );
			comprData = new char[ compressedLength ];
			uncomprData = new char[ uncomprSize ];

			for( unsigned int index = 0, j = ++i; j < i + compressedLength; j++, index++ )
				comprData[ index ] = buffer[ j ];
			//Member array initialized here
			( *normals ) = new double[ arrayLength ];
			mNormalBufferLength = arrayLength;
			if( encoding )
			{
				decompressStatus = decompressData( comprData, compressedLength, ( BYTE* ) uncomprData, uncomprSize );
			}
			else     //The Normal data doesn't need decompression
			{
				for( unsigned int index = 0; index < compressedLength; index++ )
					uncomprData[ index ] = comprData[ index ];
			}
			break;
		default:
			std::cout << "\nError: unexpected value for typeCode.\n";
			break;
		};
		//copy uncomprData to the member array after decompression
		//No Quads, only Triangles for now. Every 3 indices correspond to 1 face.
		for( unsigned int j = 0; j < uncomprSize; j += sizeof( double ) )
		{
			char cd[ 8 ] = { uncomprData[ j ], uncomprData[ j + 1 ], uncomprData[ j + 2 ], uncomprData[ j + 3 ],
				uncomprData[ j + 4 ], uncomprData[ j + 5 ], uncomprData[ j + 6 ], uncomprData[ j + 7 ] };
			double normal( 0 );
			memcpy( &normal, &cd, sizeof( double ) );
			( *normals )[ j / sizeof( double ) ] = normal;
		}
		delete[ ] comprData;
		comprData = nullptr;
		delete[ ] uncomprData;
		uncomprData = nullptr;
		delete[ ] charTemp;
		charTemp = nullptr;
	}

	unsigned int findStringEnd( char* buffer, int bufferLength, const char* string, int strLength, int currentIndex )
	{
		int i;
		for( i = currentIndex; i < bufferLength; i++ )
		{
			int j = i, count = 0;
			while( buffer[ j ] == string[ count ] )
			{
				j++;
				count++;
			}
			if( count == strLength )
				return i + strLength;
		}
		std::cout << "\nError: string " << string << " not found\n";
		return 0;
	}

	int decompressData( char* comprData, int comprSize, BYTE* uncomprData, int uncomprSize )
	{
		//zlib setup
		z_stream zInfo = { 0 };
		zInfo.total_in = zInfo.avail_in = comprSize;
		zInfo.total_out = zInfo.avail_out = uncomprSize;
		zInfo.next_in = ( BYTE* ) comprData;
		zInfo.next_out = uncomprData;

		int nErr, nRet = -1;
		nErr = inflateInit( &zInfo );               // zlib function
		if( nErr == Z_OK ) {
			nErr = inflate( &zInfo, Z_FINISH );     // zlib function
			if( nErr == Z_STREAM_END ) {
				nRet = zInfo.total_out;
			}
		}
		inflateEnd( &zInfo );   // zlib function
		return( nRet ); // -1 or len of output
	}

	void printAllWords( char* buffer, unsigned int fileSize )
	{
		int letter;
		for( unsigned int i = 0; i < fileSize; i++ )
		{
			letter = ( int ) buffer[ i ];
			if( ( 97 <= letter && letter <= 122 ) || ( 65 <= letter && letter <= 90 ) )
			{
				std::cout << std::endl;
				unsigned int j = i;
				while( ( ( 97 <= letter && letter <= 122 ) || ( 65 <= letter && letter <= 90 ) ) && j < fileSize )
				{
					std::cout << buffer[ j++ ];
					letter = ( int ) buffer[ j ];
				}
				i = j;
			}
		}
	}

	unsigned int getVertexComponentCount( void )
	{
		return mVertexBufferLength;
	}

	unsigned int getIndexCount( void )
	{
		return mIndexBufferLength;
	}

	unsigned int getNormalComponentCount( void )
	{
		return mNormalBufferLength;
	}

	void initVertexIndexNormalsArrays( double** vertices, unsigned int** indices, double** normals )
	{
		unsigned int fileSize( 0 );
		std::ifstream file( mFilePath, std::ios::in, std::ios::binary );
		struct stat results;
		if( stat( mFilePath, &results ) != 0 )
			std::cout << "Error: could not find fileSize.";
		else
			fileSize = results.st_size;
		char* buffer = new char[ fileSize ];
		file.seekg( 0 );
		file.read( buffer, fileSize );
		file.close( );

		char c;

		for( unsigned int i = 0; i < fileSize; i++ )
		{
			c = *( buffer + i );
			std::cout << c << " ";
			if( i % 14 == 0 )
				std::cout << std::endl;
		}

		printAllWords(buffer, fileSize);

		char* charTemp( nullptr );
		//							Reads the buffer as chars, ints, floats, and doubles
		float f; int integer; double d;
		for( unsigned int i = 0; i < 2000; i++ )
		{
			charTemp = new char[ 4 ]{ buffer[ i ], buffer[ i + 1 ], buffer[ i + 2 ], buffer[ i + 3 ] };
			memcpy( &f, &*charTemp, sizeof( float ) );
			memcpy( &integer, &*charTemp, sizeof( int ) );
			charTemp = new char[ 8 ]{ buffer[ i ], buffer[ i + 1 ], buffer[ i + 2 ], buffer[ i + 3 ],
				buffer[ i + 4 ], buffer[ i + 5 ], buffer[ i + 6 ], buffer[ i + 7 ] };
			memcpy( &d, &*charTemp, sizeof( double ) );

			std::cout << "char: " << buffer[ i ] << " float: " << f << " int: " << integer << " double: " << d << std::endl;
		} 

		initVertexArray( vertices, buffer, fileSize );
		initIndexArray( indices, buffer, fileSize );
		initNormalArray( normals, buffer, fileSize );

		delete[ ] buffer;
		buffer = nullptr;
	}

	~FBXFileParser( )
	{
	}

private:
	const char* mFilePath;
	unsigned int mVertexBufferLength;
	unsigned int mIndexBufferLength;
	unsigned int mNormalBufferLength;
};




/*
	Function to read a generic parameter from the FBX file.

	void initParam( param** p, char* buffer, int bufferLength, const char* paramName, int strLength )
	{
		unsigned int i = findStringEnd( buffer, bufferLength, paramName, strLength, 0 );

		for (unsigned int j = i - strLength; j < i + 500; j++)
		{
			std::cout << buffer[j];
		}

		char tc = buffer[ i++ ];
		switch (tc)
		{
		case 'd':

			break;
		case 'S':
		{
			char c[4] = { buffer[i++], buffer[i++], buffer[i++], buffer[i++] };
			unsigned int paramLength(0);
			memcpy(&paramLength, &c, sizeof(unsigned int));
			(*p) = new param(paramName, i, i + paramLength, tc);
			char* str = NULL;
			//Deleted in param destructor
			str = new char[paramLength];
			for (unsigned int count = 0; count < paramLength; i++, count++)
			{
				str[count] = buffer[i];
				std::cout << buffer[i];
			}
			(*p)->setData(str);

			while (buffer[i] != 'D')
				i++;
			double d;
			char d1[8] = { buffer[++i], buffer[++i], buffer[++i], buffer[++i],
				buffer[++i], buffer[++i], buffer[++i], buffer[++i] };
			memcpy(&d, &d1, sizeof(double));
			std::cout << d;
			i++;
			char d2[8] = { buffer[++i], buffer[++i], buffer[++i], buffer[++i],
				buffer[++i], buffer[++i], buffer[++i], buffer[++i] };
			memcpy(&d, &d1, sizeof(double));
			std::cout << d;
			i++;
			char d3[8] = { buffer[++i], buffer[++i], buffer[++i], buffer[++i],
				buffer[++i], buffer[++i], buffer[++i], buffer[++i] };
			memcpy(&d, &d1, sizeof(double));
			std::cout << d;

			break;
		}
		default:
			std::cout << "\nError: unexpected value for typeCode.\n";
			break;
		}
	}
	*/
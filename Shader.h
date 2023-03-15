#pragma once

#include <string>
#include <unordered_map>
#include "Vendor/glm/glm.hpp"
#include "Renderer.h"

#include "Math/Quaternion.h"


class VertexArray;
class VertexBuffer;
class IndexBuffer;
class Shader;
class Texture;
class Block;
class GameObject;

struct ShaderProgramSource
{
	std::string vertexSource;
	std::string fragmentSource;
};

struct VertexBufferModifier
{
public:
	std::vector< glm::mat4 > matrices;
	std::vector< uint32_t > positionOffsets;
};

class Shader
{
public:
	Shader(const std::string& filepath);
	~Shader();

	void bind() const;
	void unbind() const;

	void setUniform4d( const std::string& name, float d0, float d1, float d2, float d3 );
	void setUniform4d( const std::string& name, const glm::vec4& v );
	void setUniform4f( const std::string& name, Quaternion& q );
	void setUniform1d (const std::string& name, float d0 );
	void setUniform1i( const std::string& name, int i0 );
	void setUniform1iv( const std::string& name, int size, int* indices );
	void setUniformMat4f( const std::string& name, const glm::mat4 matrix );
	void setUniformMat4fv( const std::string& name, glm::mat4* matrix, uint32_t length );

	void initRenderDataStaticModel( const std::vector< Block >& blockArray, uint32_t arrayLength );
	void initRenderDataDynamicModel( const std::vector< Block >& blockArray, uint32_t arrayLength, glm::mat4* transforms );
	void initRenderDataStaticCollider( const std::vector< GameObject >& objArray );
	void initRenderDataDynamicCollider( const std::vector< GameObject >& objArray, glm::mat4* transforms );
	void initRenderDataWireSphere( float r, float t, uint32_t div, const glm::vec3& pos, const glm::vec4& col );

	void drawStaticModel( const glm::mat4& projTimesView );
	void drawDynamicModel( const glm::mat4& projTimesView, glm::mat4* transforms );
	void drawStaticCollider( const glm::mat4& projTimesView );
	void drawDynamicCollider( const glm::mat4& projTimesView, glm::mat4* transforms );
	void drawWireSphere( const glm::mat4& mvp, const glm::vec4& color );

	inline unsigned int getID( void ) const { return mRendererID; }

private:
	int getUniformLocation(const std::string& name);
	ShaderProgramSource parseShader(const std::string& filepath);
	unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader);
	unsigned int compileShader(unsigned int type, const std::string& source);

	std::unique_ptr< VertexArray > mVA;
	std::unique_ptr< VertexBuffer > mVB;
	int* mRenderIndices;
	uint32_t mRenderIndicesCount;

	VertexBufferModifier mModifier;

	Texture** mTextures;
	uint32_t mTexCount;
	std::string mFilePath;
	unsigned int mRendererID;
	std::unordered_map<std::string, int> mUniformLocationCache;

};
#pragma once

#include <iostream>
#include <string_view>
#include <string>
#include <fstream>
#include <sstream>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

class ShaderProgram
{
private:
	unsigned int m_ShaderProgramID;

	class Shader
	{
	public:
		enum class ShaderType { VERTEX, FRAGMENT };
		ShaderType m_ShaderType;
		unsigned int m_ShaderID;

	public:
		Shader() = delete;
		Shader(ShaderType, std::string_view);
		~Shader();
	};

	Shader VertexShaderObject, FragmentShaderObject;
	void Create();
	
public:
	const unsigned int & ID;

	ShaderProgram() = delete;
	ShaderProgram(std::string_view, std::string_view);
	// ShaderProgram(Shader&, Shader&);
	~ShaderProgram();

	void Use();

	void setBool(const unsigned int, std::string_view, bool) const;
	void setInt(const unsigned int, std::string_view, int) const;
	void setFloat(const unsigned int, std::string_view, float) const;
	void setVec3(const unsigned int, std::string_view, const glm::vec3&) const;
	void setMat4(const unsigned int, std::string_view, const glm::mat4&) const;
};


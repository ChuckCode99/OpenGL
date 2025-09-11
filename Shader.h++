#pragma once

#include <iostream>
#include <string_view>
#include <string>
#include <fstream>
#include <sstream>

#include "glad/glad.h"

class Shader
{
public:
	enum class ShaderType { VERTEX, FRAGMENT };
	const unsigned int & ID;

private:
	ShaderType m_ShaderType;
	unsigned int m_ShaderID;

public:
	Shader() = delete;
	Shader(ShaderType, std::string_view);
	~Shader();

	void setBool(const unsigned int, std::string_view, bool) const;
	void setInt(const unsigned int, std::string_view, int) const;
	void setFloat(const unsigned int, std::string_view, float) const;
};
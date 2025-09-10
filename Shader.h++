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
	static enum class ShaderType { VERTEX, FRAGMENT };
	const unsigned int const & ID;

private:
	ShaderType m_ShaderType;
	unsigned int m_ShaderID;

public:
	Shader() = delete;
	Shader(ShaderType, std::string_view);
	~Shader();

	void setBool(const std::string&, bool) const;
	void setInt(const std::string&, int) const;
	void setFloat(std::string_view, float) const;
};
#pragma once

#include "Shader.h++"

class ShaderProgram
{
private:
	unsigned int m_ShaderProgramID;
public:
	const unsigned int const& ID;

	ShaderProgram();
	ShaderProgram(Shader&, Shader&);
	~ShaderProgram();

	void Create(Shader&, Shader&);
	void Use();
};


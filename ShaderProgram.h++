#pragma once

#include "Shader.h++"

class ShaderProgram
{
private:
	unsigned int m_ShaderProgramID;
public:
	ShaderProgram();
	ShaderProgram(Shader&, Shader&);
	~ShaderProgram();

	void Create(Shader&, Shader&);
	void Use();
};


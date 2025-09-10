#include "ShaderProgram.h++"

ShaderProgram::ShaderProgram()
	: m_ShaderProgramID{ glCreateProgram() }
{}

ShaderProgram::ShaderProgram(Shader& VertexShader, Shader& FragmenShader)
	: m_ShaderProgramID{ glCreateProgram() }
{
	Create(VertexShader, FragmenShader);
}

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(m_ShaderProgramID);
}

void ShaderProgram::Create(Shader& VertexShader, Shader& FragmenShader)
{
	int  success;
	char infoLog[512];

	glAttachShader(m_ShaderProgramID, VertexShader.ID);
	glAttachShader(m_ShaderProgramID, FragmenShader.ID);
	glLinkProgram(m_ShaderProgramID);

	try
	{
		glGetProgramiv(m_ShaderProgramID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(m_ShaderProgramID, 512, NULL, infoLog);
			throw std::runtime_error(infoLog);
		}
	}
	catch (const std::exception& exception)
	{
		std::cout << "ERROR::PROGRAM::LINK_FAILED\n" << exception.what() << std::endl;
	}
}

void ShaderProgram::Use()
{
	glUseProgram(m_ShaderProgramID);
}
#include "Shader.h++"

Shader::Shader(ShaderType Type, std::string_view ShaderDataPath)
	: m_ShaderType{ Type }, ID{ m_ShaderID }
{
	std::ifstream ShaderFile;
	std::string ShaderString;
	const char* ShaderCode;
	int  success;
	char infoLog[512];

	if (m_ShaderType == ShaderType::VERTEX)
		m_ShaderID = glCreateShader(GL_VERTEX_SHADER);
	if (m_ShaderType == ShaderType::FRAGMENT)
		m_ShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// ensure ifstream objects can throw exceptions:
	ShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		std::stringstream ShaderStream;

		// open files
		ShaderFile.open(ShaderDataPath.data());

		// read file's buffer contents into streams
		ShaderStream << ShaderFile.rdbuf();

		// close file handlers
		ShaderFile.close();

		// convert stream into const char*
		ShaderString = ShaderStream.str();
		ShaderCode = ShaderString.c_str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::";
		std::cout << (m_ShaderType == ShaderType::VERTEX ? "VERTEX" : "FRAGMENT");
		std::cout << "::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}


	try
	{
		glShaderSource(m_ShaderID, 1, &ShaderCode, NULL);
		glCompileShader(m_ShaderID);
		glGetShaderiv(m_ShaderID, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(m_ShaderID, 512, NULL, infoLog);
			throw std::runtime_error(infoLog);
		}
	}
	catch (const std::exception& exception)
	{
		std::cout << "ERROR::SHADER::";
		std::cout << (m_ShaderType == ShaderType::VERTEX ? "VERTEX" : "FRAGMENT");
		std::cout << "::COMPILATION_FAILED\n" << exception.what() << std::endl;
	}

}

Shader::~Shader()
{
	glDeleteShader(m_ShaderID);
}

void Shader::setBool(const unsigned int programID, std::string_view name, bool value) const
{
	glUniform1i(glGetUniformLocation(programID, name.data()), (int)value);
}

void Shader::setInt(const unsigned int programID, std::string_view name, int value) const
{
	glUniform1i(glGetUniformLocation(programID, name.data()), value);
}

void Shader::setFloat(const unsigned int programID, std::string_view name, float value) const
{
	glUniform1f(glGetUniformLocation(programID, name.data()), value);
}

void Shader::setMat4(const unsigned int programID, std::string_view name, const glm::mat4& value) const
{
	glUniformMatrix4fv(glGetUniformLocation(programID, name.data()), 1, GL_FALSE, &value[0][0]);
}
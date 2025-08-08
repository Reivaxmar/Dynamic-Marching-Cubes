#include "GLwrap/ComputeShader.h"

namespace GLwrap
{
	
	// Constructor that builds the Shader Program
	ComputeShader::ComputeShader(std::string compFile)
	{
		// Get the code in the file
		std::string compSource = get_file_contents(compFile);
		const char* compCode = compSource.c_str();
	
		// Create the compute shader
		GLuint compShader = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(compShader, 1, &compCode, NULL);
		glCompileShader(compShader);
		compileErrors(compShader, "COMPUTE");
	
		// Create the whole shader program
		ID = glCreateProgram();
		glAttachShader(ID, compShader);
		glLinkProgram(ID);
		compileErrors(ID, "PROGRAM");
	
		// Delete the shaders
		glDeleteShader(compShader);
	}
	
	// Activates the Shader Program
	void ComputeShader::Activate()
	{
		glUseProgram(ID);
	}
	
	// Deletes the Shader Program
	void ComputeShader::Delete()
	{
		glDeleteProgram(ID);
	}
	
	void ComputeShader::Run(glm::ivec3 dispatches) {
		glDispatchCompute(dispatches.x, dispatches.y, dispatches.z);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}
	
	ComputeShader::~ComputeShader() {
		Delete();
	}
	
	// Checks if the different Shaders have compiled properly
	void ComputeShader::compileErrors(unsigned int shader, const char* type)
	{
		// Stores status of compilation
		GLint hasCompiled;
		// Character array to store error message in
		char infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
			if (hasCompiled == GL_FALSE)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n" << infoLog << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
			if (hasCompiled == GL_FALSE)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "SHADER_LINKING_ERROR for:" << type << "\n" << infoLog << std::endl;
			}
		}
	}
	
} // namespace GLwrap


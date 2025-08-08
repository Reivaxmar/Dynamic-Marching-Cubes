#include "GLwrap/Shader.h"

namespace GLwrap
{
    
    // Constructor that builds the Shader Program
    Shader::Shader(std::string vertFile, std::string fragFile)
    {
        // Get the code in the files
        std::string vertSource = get_file_contents(vertFile);
        std::string fragSource = get_file_contents(fragFile);
        const char* vertCode = vertSource.c_str();
        const char* fragCode = fragSource.c_str();
    
        // Create the vertex shader
        GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertShader, 1, &vertCode, NULL);
        glCompileShader(vertShader);
        compileErrors(vertShader, "VERTEX");
        // Create the fragment shader
        GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragShader, 1, &fragCode, NULL);
        glCompileShader(fragShader);
        compileErrors(fragShader, "FRAGMENT");
    
        // Create the whole shader program
        ID = glCreateProgram();
        glAttachShader(ID, vertShader);
        glAttachShader(ID, fragShader);
        glLinkProgram(ID);
        compileErrors(ID, "PROGRAM");
    
        // Delete the shaders
        glDeleteShader(vertShader);
        glDeleteShader(fragShader);
    }
    
    // Activates the Shader Program
    void Shader::Activate()
    {
        glUseProgram(ID);
    }
    
    // Deletes the Shader Program
    void Shader::Delete()
    {
        glDeleteProgram(ID);
    }
    
    Shader::~Shader() {
        Delete();
    }
    
    // Checks if the different Shaders have compiled properly
    void Shader::compileErrors(unsigned int shader, const char* type)
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

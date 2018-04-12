#include "stdafx.h"
#include "ShaderProgram.h"
#include <fstream>
#include <sstream>

namespace snes
{
	ShaderProgram::~ShaderProgram()
	{
		glDeleteShader(m_programID);
	}

	bool ShaderProgram::SetGlUniformMat4(const char* name, const glm::mat4& value)
	{
		GLuint position = FindUniformPositionFromName(name);
		glUniformMatrix4fv(position, 1, GL_FALSE, &value[0][0]);
		return true;
	}

	bool ShaderProgram::SetGlUniformVec3(const char* name, const glm::vec3& value)
	{
		GLuint position = FindUniformPositionFromName(name);
		glUniform3f(position, value.x, value.y, value.z);
		return true;
	}

	bool ShaderProgram::SetGlUniformVec2(const char* name, const glm::vec2& value)
	{
		GLuint position = FindUniformPositionFromName(name);
		glUniform2f(position, value.x, value.y);
		return true;
	}

	bool ShaderProgram::SetGlUniformFloat(const char* name, float value)
	{
		GLuint position = FindUniformPositionFromName(name);
		glUniform1f(position, value);
		return true;
	}

	bool ShaderProgram::SetGlUniformInt(const char* name, int value)
	{
		GLuint position = FindUniformPositionFromName(name);
		glUniform1i(position, value);
		return true;
	}

	bool ShaderProgram::SetGlUniformSampler2D(const char* name, GLuint value)
	{
		GLuint position = FindUniformPositionFromName(name);
		glUniform1i(position, value);
		return true;
	}

	bool ShaderProgram::SetGlUniformBool(const char* name, bool value)
	{
		GLuint position = FindUniformPositionFromName(name);
		glUniform1i(position, value);
		return true;
	}

	GLuint ShaderProgram::FindUniformPositionFromName(const char* name)
	{
		// If it's part of an array, find it manually
		std::string nameStr = name;
		GLuint pos = -1;
		if (nameStr.find('[') != std::string::npos)
		{
			pos = glGetUniformLocation(m_programID, name);
			if (pos != -1)
			{
				return pos;
			}
		}
		else
		{
			// Otherwise, return the stored position of the uniform if it exists
			for (const auto& uniform : m_uniforms)
			{
				if (uniform.m_name == name)
				{
					return uniform.m_position;
				}
			}
		}

		std::cout << "Error: No uniform found with name " << name << ". It may have been optimised out." << std::endl;		
		return pos;
	}
	
	std::string ShaderProgram::GetShaderFilePaths(ShaderName shaderName)
	{
		switch (shaderName)
		{
		case SOLID_COLOUR:
			return "src/rendering/shaders/SolidColour";
		case UNLIT_TEXTURED:
			return "src/rendering/shaders/UnlitTextured";
		case LIT_COLOUR:
			return "src/rendering/shaders/LitColour";
		case LIT_TEXTURED:
			return "src/rendering/shaders/LitTextured";
		case BILLBOARD:
			return "src/rendering/shaders/Billboard";
		case TESSELLATED_TEXTURED:
			return "src/rendering/shaders/PhongTessellated";
		case SILHOUETTE_TESSELLATED_TEXTURED:
			return "src/rendering/shaders/SilhouettePhongTessellated";
		case SHADOW_SOLID:
			return "src/rendering/shaders/ShadowPassSolid";
		case SHADOW_TEXTURED:
			return "src/rendering/shaders/ShadowPassTextured";
		case SHADOW_TESSELLATED:
			return "src/rendering/shaders/ShadowPassTessellated";
		case DEFERRED_MODEL:
			return "src/rendering/shaders/DeferredModel";
		case DEFERRED_LIGHTING_PASS:
			return "src/rendering/shaders/DeferredLightingPass";
		default:
			return "src/rendering/shaders/SolidColour";
		}
	}

	GLuint ShaderProgram::LoadShaders(ShaderName shaderName)
	{
		// Get file paths from shader name
		// Load shaders (see MeshRenderer.cpp)
		std::string filePath = GetShaderFilePaths(shaderName);
		
		// Create the shaders
		GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		GLuint tessControlShaderID = glCreateShader(GL_TESS_CONTROL_SHADER);
		GLuint tessEvaluationShaderID = glCreateShader(GL_TESS_EVALUATION_SHADER);
		GLuint geometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
		bool hasTessControl = false;
		bool hasTessEval = false;
		bool hasGeometry = false;

		// Try to load the vertex shader (Needed)
		std::string vertexShaderSource = LoadShaderFromFile((filePath + ".vs").c_str());
		if (CompileShader(vertexShaderID, vertexShaderSource) == GL_FALSE)
		{
			return 0;
		}

		// Try to load the fragment shader (Needed)
		std::string fragShaderSource = LoadShaderFromFile((filePath + ".fs").c_str());
		if (CompileShader(fragmentShaderID, fragShaderSource) == GL_FALSE)
		{
			return 0;
		}

		// Try to load the tessellation control shader (optional)
		std::string tessControlShaderSource = LoadShaderFromFile((filePath + ".tcs").c_str());
		if (tessControlShaderSource.length() > 0)
		{
			hasTessControl = (CompileShader(tessControlShaderID, tessControlShaderSource) != GL_FALSE);
		}

		// Try to load the tessellation evaluation shader (optional)
		std::string tessEvalShaderSource = LoadShaderFromFile((filePath + ".tes").c_str());
		if (tessEvalShaderSource.length() > 0)
		{
			hasTessEval = (CompileShader(tessEvaluationShaderID, tessEvalShaderSource) != GL_FALSE);
		}

		// Try to load the geometry shader (optional)
		std::string geometryShaderSource = LoadShaderFromFile((filePath + ".gs").c_str());
		if (geometryShaderSource.length() > 0)
		{
			//hasGeometry = (CompileShader(geometryShaderID, geometryShaderSource) != GL_FALSE);
		}


		// Link the program
		printf("Linking program\n");
		m_programID = glCreateProgram();
		glAttachShader(m_programID, vertexShaderID);
		glAttachShader(m_programID, fragmentShaderID);
		if (hasTessControl)
		{
			glAttachShader(m_programID, tessControlShaderID);
		}
		if (hasTessEval)
		{
			glAttachShader(m_programID, tessEvaluationShaderID);
		}
		if (hasGeometry)
		{
			glAttachShader(m_programID, geometryShaderID);
		}
		glLinkProgram(m_programID);

		GLint result = GL_FALSE;
		int infoLogLength;

		// Check the program
		glGetProgramiv(m_programID, GL_LINK_STATUS, &result);
		glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			char programErrorMessage[256];
			glGetProgramInfoLog(m_programID, infoLogLength, NULL, programErrorMessage);
			std::cout << programErrorMessage << std::endl;
		}

		glDetachShader(m_programID, vertexShaderID);
		glDetachShader(m_programID, fragmentShaderID);
		if (hasTessControl)
		{
			glDetachShader(m_programID, tessControlShaderID);
		}
		if (hasTessEval)
		{
			glDetachShader(m_programID, tessEvaluationShaderID);
		}
		if (hasGeometry)
		{
			glDetachShader(m_programID, geometryShaderID);
		}

		glDeleteShader(vertexShaderID);
		glDeleteShader(fragmentShaderID);
		if (hasTessControl)
		{
			glDeleteShader(tessControlShaderID);
		}
		if (hasTessEval)
		{
			glDeleteShader(tessEvaluationShaderID);
		}
		if (hasGeometry)
		{
			glDeleteShader(geometryShaderID);
		}

		glUseProgram(m_programID);

		// Find and store uniform locations
		RetrieveUniformLocations();

		return m_programID;
	}

	std::string ShaderProgram::LoadShaderFromFile(const char* filePath)
	{
		std::string shader;
		std::ifstream shaderStream(filePath, std::ios::in);
		if (shaderStream.is_open())
		{
			std::string line;
			while (getline(shaderStream, line))
			{
				shader += "\n" + line;

				// Extract any uniforms from the shader
				CreateUniformFromLine(line);
				
			}
			shaderStream.close();
		}
		else
		{
			std::cout << "Could not open shader source from " << filePath << std::endl;
			return std::string();
		}
		//std::cout << shader << std::endl;
		return shader;
	}

	GLint ShaderProgram::CompileShader(GLuint& shaderID, std::string& shader)
	{
		// Compile shader
		char const * sourcePointer = shader.c_str();
		glShaderSource(shaderID, 1, &sourcePointer, NULL);
		glCompileShader(shaderID);

		GLint result = GL_TRUE;

	#if DEBUG
		// Check shader
		int infoLogLength;
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			char errorMessage[256];
			glGetShaderInfoLog(shaderID, infoLogLength, NULL, errorMessage);
			std::cout << errorMessage << std::endl;
		}
	#endif

		return result;
	}

	void ShaderProgram::CreateUniformFromLine(std::string& line)
	{
		std::istringstream lineStream(line);
		std::string isUniform;
		std::getline(lineStream, isUniform, ' ');
		if (isUniform == "uniform")
		{
			std::string type;
			std::string name;

			// This is a uniform, find its type
			std::getline(lineStream, type, ' ');

			// Find its name
			if (lineStream.str().find(" =") != std::string::npos)
			{
				std::getline(lineStream, name, ' ');
			}
			else if (lineStream.str().find("=") != std::string::npos)
			{
				std::getline(lineStream, name, '=');
			}
			else
			{
				std::getline(lineStream, name, ';');
			}

			// Create the ShaderUniform
			ShaderUniform uniform;
			uniform.m_name = name;
			m_uniforms.push_back(uniform);
		}
	}

	void ShaderProgram::RetrieveUniformLocations()
	{
		for (auto& uniform : m_uniforms)
		{
			uniform.m_position = glGetUniformLocation(m_programID, uniform.m_name.c_str());
		}
	}
}

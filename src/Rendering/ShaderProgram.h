#pragma once
#include <GL/glew.h>
#include <glm\glm.hpp>
#include <map>

namespace snes
{
	enum ShaderName
	{
		NONE,
		SOLID_COLOUR,
		LIT_COLOUR,
		UNLIT_TEXTURED,
		LIT_TEXTURED,
		BILLBOARD,
		TESSELLATED_TEXTURED,
		SHADOW_SOLID,
		SHADOW_TEXTURED,
		SHADOW_TESSELLATED,
		DEFERRED_MODEL,
		DEFERRED_LIGHTING_PASS
	};

	struct ShaderUniform
	{
		GLuint m_position;
		std::string m_name;
	};

	/** Shader Program
	  * Loads and compiles a shader.
	  * Uniforms can be set through this object, with error checking. */
	class ShaderProgram
	{
	public:
		ShaderProgram() { m_programID = 0; }
		ShaderProgram(ShaderName shaderName) { m_programID = LoadShaders(shaderName); }
		~ShaderProgram();

		/** Set the value of the uniform with the given name
		  * @return true if successful */
		bool SetGlUniformMat4(const char* name, const glm::mat4& value);
		bool SetGlUniformVec2(const char* name, const glm::vec2& value);
		bool SetGlUniformVec3(const char* name, const glm::vec3& value);
		bool SetGlUniformFloat(const char* name, float value);
		bool SetGlUniformInt(const char* name, int value);
		bool SetGlUniformSampler2D(const char* name, GLuint value);
		bool SetGlUniformBool(const char* name, bool value);

		GLuint GetProgramID() { return m_programID; }
		void Load(ShaderName shaderName) { m_programID = LoadShaders(shaderName); }

	private:
		/** @return the file path (without file extensions) of the associated shaders
		  * This assumes that all associated shaders share a file name, e.g. "test.vs" and "test.fs" */
		static std::string GetShaderFilePaths(ShaderName shaderName);
		static GLint CompileShader(GLuint& shaderID, std::string& shader);

		/** Loads all shaders associated with shaderName, and returns a program ID */
		GLuint LoadShaders(ShaderName shaderName);
		std::string LoadShaderFromFile(const char* filePath);
		void CreateUniformFromLine(std::string& line);
		void RetrieveUniformLocations();
		GLuint FindUniformPositionFromName(const char* name);

		GLuint m_programID;
		std::vector<ShaderUniform> m_uniforms;


	};
}

#pragma once
#include "ShaderProgram.h"
#include <map>

namespace snes
{
	// Derive from this to make your material
	class Material
	{
	public:
		~Material();

		/** Called before rendering, set uniforms in here */
		virtual void PrepareForRendering();

		/** Set the model, view, and projection uniforms (these must be present in every material shader) */
		void ApplyTransformUniforms(glm::mat4& model, glm::mat4& view, glm::mat4& proj);

	public:
		static std::shared_ptr<Material> CreateMaterial(const char* matPath);

	protected:
		Material(ShaderName shaderName);

		/** Set a uniform of the given type with the given name to the given value */
		void SetUniformMat4(const char* name, glm::mat4 value);
		void SetUniformVec2(const char* name, glm::vec2 value);
		void SetUniformVec3(const char* name, glm::vec3 value);
		void SetUniformFloat(const char* name, float value);
		void SetUniformSampler2D(const char* name, GLuint value);

	protected:
		/** Cache of all loaded shaders */
		static std::map<ShaderName, std::weak_ptr<ShaderProgram>> m_shaders;

	private:
		ShaderName m_shaderName;

		std::shared_ptr<ShaderProgram> m_shader;

		std::map<std::string, glm::vec3> m_vec3s;
		std::map<std::string, glm::vec2> m_vec2s;
		std::map<std::string, glm::mat4> m_mat4s;
		std::map<std::string, float> m_floats;
		std::map<std::string, GLuint> m_sampler2Ds;

		static ShaderName m_currentShader;
	};
}

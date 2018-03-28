#pragma once
#include "ShaderProgram.h"
#include <Components\Transform.h>
#include <map>

namespace snes
{
	class Camera;
	class Transform;

	// Derive from this to make your material
	class Material
	{
	public:
		~Material();

		/** Called before rendering, set uniforms in here */
		virtual void PrepareForRendering();
		virtual void PrepareForRendering(Transform& transform, Camera& camera) { this->PrepareForRendering(); }

		/** Set the model, view, and projection uniforms (these must be present in every material shader) */
		virtual void ApplyTransformUniforms(glm::mat4& model, glm::mat4& view, glm::mat4& proj);

		bool GetUsePatches() { return m_usePatches; }

	public:
		static std::shared_ptr<Material> CreateMaterial(const char* matPath);
		static std::shared_ptr<Material> CreateShadowMaterial(const char* matPath);
		static void ResetCurrentShader() { m_currentShader = NONE; }

	protected:
		Material(ShaderName shaderName);

		/** Set a uniform of the given type with the given name to the given value */
		void SetUniformMat4(const char* name, glm::mat4 value);
		void SetUniformVec2(const char* name, glm::vec2 value);
		void SetUniformVec3(const char* name, glm::vec3 value);
		void SetUniformFloat(const char* name, float value);
		void SetUniformSampler2D(const char* name, GLuint value);
		void SetUniformBool(const char* name, bool value);

		bool m_usePatches = false;

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
		std::map<std::string, bool> m_bools;

		static ShaderName m_currentShader;
	};
}

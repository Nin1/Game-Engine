#include "stdafx.h"
#include "Material.h"
#include "Materials/DiscoMat.h"
#include "Materials/LitColourMat.h"
#include "Materials/LitTexturedMat.h"
#include "Materials/SolidColourMat.h"
#include "Materials/UnlitTexturedMat.h"
#include <fstream>

namespace snes
{
	std::map<ShaderName, std::weak_ptr<ShaderProgram>> Material::m_shaders;
	ShaderName Material::m_currentShader = NONE;

	Material::Material(ShaderName shaderName)
	{
		m_shaderName = shaderName;

		// If the shader is not cached yet, load it now and cache it
		if (m_shaders[shaderName].expired())
		{
			m_shader = std::make_shared<ShaderProgram>(shaderName);
			m_shaders[shaderName] = m_shader;
		}
		else
		{
			m_shader = m_shaders[shaderName].lock();
		}
	}

	Material::~Material()
	{
	}

	/**********************************
	** Create new material from file **
    **********************************/

	std::shared_ptr<Material> Material::CreateMaterial(const char* matPath)
	{
		std::ifstream params(matPath, std::ios::in);

		if (!params)
		{
			std::cout << "Error opening file: " << matPath << std::endl;
			return std::make_shared<SolidColourMat>();
		}

		std::shared_ptr<Material> material;

		// Determine what material to make, and construct it
		std::string line;
		std::getline(params, line);

		if (line == "SOLID_COLOUR")
		{
			material = std::make_shared<SolidColourMat>(params);
		}
		else if (line == "LIT_COLOUR")
		{
			material = std::make_shared<LitColourMat>(params);
		}
		else if (line == "UNLIT_TEXTURED")
		{
			material = std::make_shared<UnlitTexturedMat>(params);
		}
		else if (line == "LIT_TEXTURED")
		{
			material = std::make_shared<LitTexturedMat>(params);
		}
		else if (line == "DISCO")
		{
			material = std::make_shared<DiscoMat>(params);
		}

		params.close();

		return material;
	}

	/******************************
	** Apply all shader uniforms **
	******************************/

	void Material::PrepareForRendering()
	{
		if (m_currentShader != m_shaderName)
		{
			// Only swap shaders if necessary
			glUseProgram(m_shader->GetProgramID());
			m_currentShader = m_shaderName;
		}

		for (const auto& uniform : m_mat4s)
		{
			m_shader->SetGlUniformMat4(uniform.first.c_str(), uniform.second);
		}
		for (const auto& uniform : m_vec3s)
		{
			m_shader->SetGlUniformVec3(uniform.first.c_str(), uniform.second);
		}
		for (const auto& uniform : m_vec2s)
		{
			m_shader->SetGlUniformVec2(uniform.first.c_str(), uniform.second);
		}
		for (const auto& uniform : m_floats)
		{
			m_shader->SetGlUniformFloat(uniform.first.c_str(), uniform.second);
		}
		for (const auto& uniform : m_sampler2Ds)
		{
			m_shader->SetGlUniformSampler2D(uniform.first.c_str(), uniform.second);
		}
	}

	void Material::ApplyTransformUniforms(glm::mat4& model, glm::mat4& view, glm::mat4& proj)
	{
		m_shader->SetGlUniformMat4("modelMat", model);
		m_shader->SetGlUniformMat4("viewMat", view);
		m_shader->SetGlUniformMat4("projMat", proj);
	}

	void Material::SetUniformMat4(const char* name, glm::mat4 value)
	{
		m_mat4s[name] = value;
	}

	void Material::SetUniformVec2(const char* name, glm::vec2 value)
	{
		m_vec2s[name] = value;
	}

	void Material::SetUniformVec3(const char* name, glm::vec3 value)
	{
		m_vec3s[name] = value;
	}

	void Material::SetUniformFloat(const char* name, float value)
	{
		m_floats[name] = value;
	}

	void Material::SetUniformSampler2D(const char* name, GLuint value)
	{
		m_sampler2Ds[name] = value;
	}
}

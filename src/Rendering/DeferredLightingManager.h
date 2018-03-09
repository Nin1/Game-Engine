#pragma once
#include <GL/glew.h>
#include "ShaderProgram.h"
#include <Components\DirectionalLight.h>
#include <Components\PointLight.h>

namespace snes
{
	/** Deferred Lighting Manager
	  * Handles the deferred lighting pipeline */
	class DeferredLightingManager
	{
	public:
		DeferredLightingManager();
		~DeferredLightingManager();

		/** Initialise the deferred framebuffers and deferred lighting shader */
		void Init();
		/** Set up a new geometry pass */
		void PrepareNewGeometryPass();
		/** Set up a new shadow pass */
		void PrepareNewShadowPass();
		/** Render the contents of the framebuffer, lit by the pointlights */
		void RenderLighting(std::shared_ptr<Camera> camera, std::vector<std::weak_ptr<PointLight>> pointLights, std::shared_ptr<DirectionalLight> directionalLight);

	private:
		/** Render a quad to the screen */
		void RenderQuad();

		static const glm::mat4 BIAS_MATRIX;
		const uint MAX_POINT_LIGHTS = 32;

		ShaderProgram m_shader;
		GLuint m_quadVAO = 0;
		GLuint m_quadVBO = 0;

		/** Render buffers */
		GLuint m_buffer;
		GLuint m_position;
		GLuint m_normal;
		GLuint m_albedoSpecular;
		GLuint m_emissive;

		GLuint m_shadowFBO;
		GLuint m_shadowRT;
		GLuint m_shadowTexture;

		uint m_depth;
	};
}

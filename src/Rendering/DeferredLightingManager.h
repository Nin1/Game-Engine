#pragma once
#include <GL/glew.h>
#include "ShaderProgram.h"
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
		/** Set up a new rendering fram */
		void PrepareNewFrame();
		/** Render the contents of the framebuffer, lit by the pointlights */
		void RenderLighting(const Transform& camera, std::vector<std::weak_ptr<PointLight>> pointLights);

	private:
		/** Render a quad to the screen */
		void RenderQuad();

		const uint MAX_POINT_LIGHTS = 32;

		ShaderProgram m_shader;
		GLuint m_quadVAO = 0;
		GLuint m_quadVBO = 0;
		GLuint m_buffer;
		GLuint m_position;
		GLuint m_normal;
		GLuint m_albedoSpecular;
		GLuint m_emissive;
		uint m_depth;
	};
}

#pragma once
#include "Camera.h"
#include <Rendering\Material.h>
#include <Rendering\Mesh.h>

namespace snes
{
	class MeshRenderer : public Component
	{
	public:
		MeshRenderer(GameObject& gameObject);
		~MeshRenderer();

		void MainDraw(RenderPass renderPass, Camera& camera) override;

		/** Sets the mesh to be rendered */
		void SetMesh(const char* meshFile) { m_mesh = Mesh::GetMesh(meshFile); }
		/** Returns the mesh */
		const std::weak_ptr<Mesh> GetMesh() const { return m_mesh; }
		/** Sets the camera from which to render the mesh */
		void SetCamera(std::weak_ptr<Camera> camera) { m_camera = camera; }

		/** Creates a material for the mesh renderer and returns a weak_ptr to it */
		template <typename T>
		std::weak_ptr<T> SetMaterial()
		{
			std::shared_ptr<T> newMaterial = std::make_shared<T>();
			m_material = newMaterial;
			return newMaterial;
		}

		/** Sets the material of the mesh to the given material */
		template <typename T>
		void SetMaterial(std::shared_ptr<T> material) { m_material = material; }

	private:
		/** Prepare the transformation matrices (MVP) and send them to the shader */
		void PrepareTransformUniforms(Camera& camera);

		/** The mesh to be rendered */
		std::shared_ptr<Mesh> m_mesh;
		/** The camera to render the mesh from */
		std::weak_ptr<Camera> m_camera;
		/** The material to render the mesh with */
		std::shared_ptr<Material> m_material;
	};
}

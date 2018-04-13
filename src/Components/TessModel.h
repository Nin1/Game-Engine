#pragma once
#include <Core\Component.h>
#include <Rendering\Mesh.h>
#include <Rendering\Material.h>

namespace snes
{
	class Camera;

	class TessModel : public Component
	{
	public:
		TessModel(GameObject& gameObject) : Component(gameObject) { };
		~TessModel() { };

		/** Load meshes of all LODs starting with "meshName0.obj" */
		void Load(std::string modelName);

		/** Sets the camera from which to render the mesh */
		void SetCamera(std::weak_ptr<Camera> camera) { m_camera = camera; }
		void SetReferenceObject(std::weak_ptr<GameObject> object) { m_referenceObj = object; }

		void FixedLogic() override;
		void MainLogic() override;
		void MainDraw(RenderPass renderPass, Camera& camera) override;

		const std::weak_ptr<Mesh> GetMesh() const { return m_mesh; }
				
	private:
		/** Calculate the model/view/proj matrices and apply them to the material */
		void PrepareTransformUniforms(Camera& camera, Material* mat);

		/** Return the radius of the encapsulating sphere around the mesh in screen-space */
		float GetScreenSizeOfMesh(Camera& camera);

		/** The camera to render the mesh from */
		std::weak_ptr<Camera> m_camera;
		static std::weak_ptr<GameObject> m_referenceObj;
		static bool m_useReferenceObj;

		/**  */
		std::shared_ptr<Mesh> m_mesh;
		std::shared_ptr<Material> m_material;
		std::shared_ptr<Material> m_shadowMaterial;
	};
}

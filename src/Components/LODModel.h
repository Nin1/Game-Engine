#pragma once
#include <Core\Component.h>
#include <Rendering\Mesh.h>
#include <Rendering\Material.h>

namespace snes
{
	class Camera;

	class LODModel : public Component
	{
	public:
		LODModel(GameObject& gameObject) : Component(gameObject) {};
		~LODModel() {};

		/** Load meshes of all LODs starting with "meshName0.obj" */
		void Load(std::string modelName);

		/** Sets the camera from which to render the mesh */
		void SetCamera(std::weak_ptr<Camera> camera) { m_camera = camera; }

		void MainDraw() override;

		/** Return the index of the best LOD to show */
		int GetCurrentLOD() const;

		const std::weak_ptr<Mesh> GetMesh(uint lodLevel) const;

	private:
		/** Calculate the model/view/proj matrices and apply them to the material */
		void PrepareTransformUniforms(Camera& camera, Material& mat);

		/** The camera to render the mesh from */
		std::weak_ptr<Camera> m_camera;

		/** List of meshes
		  * Lower indexes represent higher-detailed meshes
		  * e.g. m_meshes[0] = LOD0 = highest detail */
		std::vector<std::shared_ptr<Mesh>> m_meshes;
		std::vector<std::shared_ptr<Material>> m_materials;

		/** Distance from camera that the lowest LOD is used */
		float m_distanceLow = 100;
		/** Distance from camera that the highest LOD is used */
		float m_distanceHigh = 10;
	};
}

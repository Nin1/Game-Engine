#pragma once
#include <Core\Component.h>
#include <Rendering\Mesh.h>
#include <Rendering\Material.h>

namespace snes
{
	class Camera;
	class LODModel;

	struct LODValue
	{
		LODModel* model;
		int meshIndex;
		float cost;
		float value;
	};

	class LODModel : public Component
	{
	public:
		LODModel(GameObject& gameObject) : Component(gameObject) { m_instanceCount++; };
		~LODModel() { m_instanceCount--; };

		/** Load meshes of all LODs starting with "meshName0.obj" */
		void Load(std::string modelName);

		/** Sets the camera from which to render the mesh */
		void SetCamera(std::weak_ptr<Camera> camera) { m_camera = camera; }
		void SetReferenceObject(std::weak_ptr<GameObject> object) { m_referenceObj = object; }

		void FixedLogic() override;
		void MainDraw() override;

		/** Return the index of the best LOD to show */
		int GetCurrentLOD() const;

		const std::weak_ptr<Mesh> GetMesh(uint lodLevel) const;

	public:
		static void StartNewFrame();
		static void SortAndSetLODValues();

	private:
		/** List of the value (benefit/cost) for every LOD of every mesh in the scene */
		static std::vector<LODValue> m_lodValues;
		/** A count of how many meshes exist total across all LODModels */
		static uint m_instanceCount;
		/** The total cost of all selected meshes so far */
		static float m_totalCost;
		/** The maximum total cost allowed */
		static float m_maxCost;

	private:
		/** Calculate the model/view/proj matrices and apply them to the material */
		void PrepareTransformUniforms(Camera& camera, Material& mat);
		/** Cost/Benefit method for finding the best LOD to show */
		int CalculateEachLODValue();
		float GetScreenSizeOfMesh(int index);
		void SetCurrentLOD(LODValue& lodValue);

		/** The camera to render the mesh from */
		std::weak_ptr<Camera> m_camera;
		static std::weak_ptr<GameObject> m_referenceObj;
		static bool m_useReferenceObj;

		/** List of meshes
		  * Lower indexes represent higher-detailed meshes
		  * e.g. m_meshes[0] = LOD0 = highest detail */
		std::vector<std::shared_ptr<Mesh>> m_meshes;
		std::vector<std::shared_ptr<Material>> m_materials;

		/** Distance from camera that the lowest LOD is used */
		float m_distanceLow = 100;
		/** Distance from camera that the highest LOD is used */
		float m_distanceHigh = 10;

		/** The index of the mesh selected to show */
		uint m_meshToShow = 0;
		/** The cost of the currently selected mesh */
		uint m_shownMeshCost = 0;
	};
}

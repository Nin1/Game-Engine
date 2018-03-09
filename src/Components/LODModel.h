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
		uint meshIndex;
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
		void MainLogic() override;
		void MainDraw(RenderPass renderPass, Camera& camera) override;

		/** Return the index of the best LOD to show */
		int GetCurrentLOD() const;

		const std::weak_ptr<Mesh> GetMesh(uint lodLevel) const;

	public:
		static void StartNewFrame();
		static void SortAndSetLODValues();

	private:
		const static float STIPPLE_PATTERN[16];
		/** The transition duration in seconds */
		const static float TRANSITION_DURATION_S;
		/** List of the value (benefit/cost) for every LOD of every mesh in the scene */
		static std::vector<LODValue> m_lodValues;
		/** A count of how many meshes exist total across all LODModels */
		static uint m_instanceCount;
		/** The total cost of all selected meshes so far */
		static float m_totalCost;
		/** The maximum total cost allowed */
		static float m_maxCost;

	private:
		void DrawCurrentMesh(RenderPass renderPass, Camera& camera);
		/** Draw the last mesh with a stipple effect fading out */
		void DrawLastMesh(RenderPass renderPass, Camera& camera);
		/** Generate a stipple pattern between 0 (invisible) and 1 (visible) */
		void GenerateStipplePattern(float opacity, GLubyte patternOut[128]);
		void InvertStipplePattern(GLubyte patternOut[128]);
		/** Calculate the model/view/proj matrices and apply them to the material */
		void PrepareTransformUniforms(Camera& camera, Material* mat);
		/** Cost/Benefit method for finding the best LOD to show */
		int CalculateEachLODValue();
		/** Very cheap and probably incorrect estimation of what LOD to show */
		void PickBestMesh();
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
		std::vector<std::shared_ptr<Material>> m_shadowMaterials;

		/** Distance from camera that the lowest LOD is used */
		float m_distanceLow = 100;
		/** Distance from camera that the highest LOD is used */
		float m_distanceHigh = 10;

		/** The index of the mesh selected to show */
		uint m_currentMesh = 0;
		/** The index of the last mesh selected to show */
		uint m_lastMesh = 0;

		/** The index of the last mesh actually rendered */
		uint m_lastRenderedMesh = 0;
		/** The index of the mesh being transitioned from */
		uint m_transitioningFromMesh = 0;

		GLubyte m_stipplePattern[128] = { 0 };

		/** The time remaining until the transition from one LOD to another is finished (in seconds) */
		float m_transitionRemainingS = 0.0f;
		/** The cost of the currently selected mesh */
		float m_shownMeshCost = 0;
		float m_lastMeshCost = 0;
	};
}

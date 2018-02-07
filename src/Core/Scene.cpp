#include "stdafx.h"
#include "Scene.h"
#include <Components\AABBCollider.h>
#include <Components\Camera.h>
#include <Components\CharController.h>
#include <Components\LODModel.h>
#include <Components\MeshRenderer.h>
#include <Components\Rigidbody.h>
#include <Components\TestComponent.h>

#include <Rendering\Mesh.h>
#include <Rendering\Materials\DiscoMat.h>
#include <Rendering\Materials\SolidColourMat.h>
#include <Rendering\Materials\UnlitTexturedMat.h>

namespace snes
{
	Scene::Scene()
	{
		m_root = std::make_shared<GameObject>(nullptr);
	}

	Scene::~Scene()
	{

	}

	void Scene::InitialiseScene()
	{
		m_deferredLightingMgr.Init();

		// Create camera
		m_camera = m_root->AddChild().lock();
		auto camera = m_camera->AddComponent<Camera>();
		m_camera->GetTransform().SetLocalPosition(glm::vec3(0, -4.0f, 18.0f));
		m_camera->GetTransform().SetLocalRotation(glm::vec3(-29.4f, 270.0f, 0.0f));

		std::shared_ptr<SolidColourMat> redMat = std::make_shared<SolidColourMat>();
		redMat->SetColour(glm::vec3(1.0f, 0.0f, 0.0f));
		std::shared_ptr<DiscoMat> discoMat = std::make_shared<DiscoMat>();
		std::shared_ptr<UnlitTexturedMat> texturedMat = std::make_shared<UnlitTexturedMat>();
		texturedMat->SetTexture("Jiggy.bmp");

		CreateJiggy(glm::vec3(15, 0, 0), camera.lock(), redMat);
		CreateJiggy(glm::vec3(-15, 0, 0), camera.lock(), discoMat);
		CreateJiggy(glm::vec3(0, 0, 15), camera.lock(), texturedMat);
		CreateLink(glm::vec3(0, 0, -15), camera.lock());
		CreateFloor(camera.lock());
		// Create a bunch of pretty lights on the floor
		CreatePointLight(*m_root, glm::vec3(10, -8, 0), glm::vec3(1, 0, 1));
		CreatePointLight(*m_root, glm::vec3(-10, -8, 0), glm::vec3(1, 1, 0));
		CreatePointLight(*m_root, glm::vec3(0, -8, 10), glm::vec3(0, 1, 1));
		CreatePointLight(*m_root, glm::vec3(0, -8, -10), glm::vec3(1, 0, 0));
		CreatePointLight(*m_root, glm::vec3(-10, -8, 10), glm::vec3(0, 1, 0));
		CreatePointLight(*m_root, glm::vec3(10, -8, 10), glm::vec3(0, 0, 1));
		CreatePointLight(*m_root, glm::vec3(10, -8, -10), glm::vec3(1, 1, 1));
		CreatePointLight(*m_root, glm::vec3(-10, -8, -10), glm::vec3(1, 1, 1));
		// Create a big light in the center of the scene
		GameObject& bigLight = CreatePointLight(*m_root, glm::vec3(0, -8, 0), glm::vec3(1, 1, 1));
		bigLight.GetComponent<PointLight>()->SetLinearAttenuation(0.01f);
		bigLight.GetComponent<PointLight>()->SetQuadraticAttenuation(0.02f);
	}

	void Scene::FixedLogic()
	{
		m_root->FixedLogic();
		
		// Generate list of all GameObjects
		std::vector<std::weak_ptr<GameObject>> allObjects = m_root->GetAllChildren();

		// Handle collision between all GameObjects once
		for (uint i = 0; i < allObjects.size(); i++)
		{
			auto currentObject = allObjects.at(i).lock();
			auto rigidbody = currentObject->GetComponent<Rigidbody>();

			if (!rigidbody)
			{
				continue;
			}

			for (uint j = (i + 1); j < allObjects.size(); j++)
			{
				auto currentTarget = allObjects.at(j).lock();
				if (currentTarget)
				{ 
					rigidbody->HandleCollision(*currentTarget);
				}
			}

			rigidbody->UpdatePosition();
		}
	}

	void Scene::MainDraw()
	{
		m_deferredLightingMgr.PrepareNewFrame();

		// Render all objects in geometry pass to deferred framebuffer
		m_root->MainDraw();

		// Render deferred lighting
		m_deferredLightingMgr.RenderLighting(m_camera->GetTransform(), m_pointLights);
	}

	GameObject& Scene::CreatePointLight(GameObject& parent, glm::vec3 pos, glm::vec3 colour)
	{
		auto light = parent.AddChild().lock();
		light->GetTransform().SetLocalPosition(pos);

		auto& pointLight = light->AddComponent<PointLight>().lock();
		pointLight->SetColour(colour);

		// @TODO: Replace vector with GetComponentsOfType<PointLight>()
		m_pointLights.push_back(pointLight);

		return *light;
	}

	GameObject& Scene::CreateJiggy(glm::vec3 pos, std::shared_ptr<Camera> camera, std::shared_ptr<Material> material)
	{
		// Create the test jiggy
		auto jiggy = m_root->AddChild().lock();
		jiggy->GetTransform().SetLocalPosition(pos);

		// Make it render
		auto& meshRenderer = jiggy->AddComponent<MeshRenderer>().lock();
		meshRenderer->SetMesh("Jiggy.obj");
		meshRenderer->SetCamera(camera);
		meshRenderer->SetMaterial(material);

		// Add Rigidbody and collider
		//jiggy->AddComponent<Rigidbody>();
		jiggy->AddComponent<AABBCollider>();
		// TestComponent makes an object spin
		jiggy->AddComponent<TestComponent>();

		// Create a light orbiting the jiggy
		CreatePointLight(*jiggy, glm::vec3(3, 3, 0), glm::vec3(1, 0, 0));
		CreatePointLight(*jiggy, glm::vec3(-3, 3, 0), glm::vec3(0, 1, 0));
		CreatePointLight(*jiggy, glm::vec3(0, 3, 3), glm::vec3(0, 0, 1));
		CreatePointLight(*jiggy, glm::vec3(0, 3, -3), glm::vec3(1, 1, 1));

		return *jiggy;
	}

	GameObject& Scene::CreateLink(glm::vec3 pos, std::shared_ptr<Camera> camera)
	{
		// Create the test jiggy
		auto link = m_root->AddChild().lock();
		link->GetTransform().SetLocalPosition(pos);
		link->GetTransform().SetLocalScale(glm::vec3(0.1f, 0.1f, 0.1f));

		// Make it render
		//auto& meshRenderer = link->AddComponent<MeshRenderer>().lock();
		//meshRenderer->SetMesh("charizard.obj");
		//meshRenderer->SetCamera(camera);

		//std::shared_ptr<UnlitTexturedMat> texMat = std::make_shared<UnlitTexturedMat>();
		//texMat->SetTexture("charizard.png");
		//meshRenderer->SetMaterial(texMat);

		auto& lodModel = link->AddComponent<LODModel>().lock();
		lodModel->SetCamera(camera);
		lodModel->Load("Models/charizard");

		// Add Rigidbody and collider
		link->AddComponent<Rigidbody>();
		link->AddComponent<AABBCollider>();
		// Add character controller component
		link->AddComponent<CharController>();

		return *link;
	}

	GameObject& Scene::CreateFloor(std::shared_ptr<Camera> camera)
	{
		auto floor = m_root->AddChild().lock();
		floor->GetTransform().SetLocalPosition(glm::vec3(-100, -10.0f, -100));
		floor->GetTransform().SetLocalScale(glm::vec3(100.0f, 0.5f, 100.0f));

		auto meshRenderer = floor->AddComponent<MeshRenderer>().lock();
		meshRenderer->SetMesh("cube.obj");
		meshRenderer->SetCamera(camera);

		std::shared_ptr<UnlitTexturedMat> floorMat = std::make_shared<UnlitTexturedMat>();
		floorMat->SetTexture("cube.bmp");
		meshRenderer->SetMaterial(floorMat);

		auto rb = floor->AddComponent<Rigidbody>().lock();
		rb->LockPosition();

		auto collider = floor->AddComponent<AABBCollider>().lock();

		return *floor;
	}
}
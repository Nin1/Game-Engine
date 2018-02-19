#include "stdafx.h"
#include "Scene.h"
#include "Input.h"
#include <Components\AABBCollider.h>
#include <Components\Camera.h>
#include <Components\CharController.h>
#include <Components\LODModel.h>
#include <Components\MeshRenderer.h>
#include <Components\Rigidbody.h>
#include <Components\TestComponent.h>

#include <Rendering\Mesh.h>
#include <Rendering\Materials\DiscoMat.h>
#include <Rendering\Materials\LitColourMat.h>
#include <Rendering\Materials\LitTexturedMat.h>
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

		std::shared_ptr<DiscoMat> discoMat = std::make_shared<DiscoMat>();
		std::shared_ptr<UnlitTexturedMat> texturedMat = std::make_shared<UnlitTexturedMat>();
		texturedMat->SetTexture("Models/Jiggy.bmp");

		//CreateJiggy(glm::vec3(15, 0, 0), camera.lock(), texturedMat);
		//CreateJiggy(glm::vec3(-15, 0, 0), camera.lock(), texturedMat);
		//CreateJiggy(glm::vec3(0, 0, 15), camera.lock(), texturedMat);
		auto lodReferenceObj = CreateLink(glm::vec3(0, 0, -15), camera.lock());
		CreateFloor(camera.lock());
		// Create a ton of spheres
		for (int i = -35; i < 35; i++)
		{
			for (int j = -15; j < 15; j++)
			{
				CreateSphere(glm::vec3(i*2, -5, j*2), camera.lock(), lodReferenceObj);
			}
		}
		// Create a bunch of pretty lights on the floor
/*		CreatePointLight(*m_root, glm::vec3(10, 0, 0), glm::vec3(1, 0, 1));
		CreatePointLight(*m_root, glm::vec3(-10, 0, 0), glm::vec3(1, 1, 0));
		CreatePointLight(*m_root, glm::vec3(0, 0, 10), glm::vec3(0, 1, 1));
		CreatePointLight(*m_root, glm::vec3(0, 0, -10), glm::vec3(1, 0, 0));
		CreatePointLight(*m_root, glm::vec3(-10, 0, 10), glm::vec3(0, 1, 0));
		CreatePointLight(*m_root, glm::vec3(10, 0, 10), glm::vec3(0, 0, 1));
		CreatePointLight(*m_root, glm::vec3(10, 0, -10), glm::vec3(1, 1, 1));
		CreatePointLight(*m_root, glm::vec3(-10, 0, -10), glm::vec3(1, 1, 1));*/
		// Create a big light in the center of the scene
		GameObject& bigLight = CreatePointLight(*m_root, glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
		bigLight.GetComponent<PointLight>()->SetLinearAttenuation(0.001f);
		bigLight.GetComponent<PointLight>()->SetQuadraticAttenuation(0.002f);
	}

	void Scene::FixedLogic()
	{
		LODModel::StartNewFrame();

		m_root->FixedLogic();

		LODModel::SortAndSetLODValues();
		
		// Generate list of all GameObjects
		std::vector<std::weak_ptr<GameObject>> allObjects = m_root->GetAllChildren();

		// Handle collision between all GameObjects once
		/*
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
				if (!allObjects.at(j).expired())
				{ 
					rigidbody->HandleCollision(*allObjects.at(j).lock());
				}
			}

			rigidbody->UpdatePosition();
		}
		*/
	}

	void Scene::MainDraw()
	{
		m_deferredLightingMgr.PrepareNewFrame();

		if (Input::GetKeyHeld('f'))
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		// Render all objects in geometry pass to deferred framebuffer
		m_root->MainDraw();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// Render deferred lighting
		m_deferredLightingMgr.RenderLighting(m_camera->GetTransform(), m_pointLights);

		Mesh::ResetRenderCount();
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
		meshRenderer->SetMesh("Models/Jiggy.obj");
		meshRenderer->SetCamera(camera);
		meshRenderer->SetMaterial(material);

		// Add Rigidbody and collider
		jiggy->AddComponent<Rigidbody>();
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

	GameObject& Scene::CreateSphere(glm::vec3 pos, std::shared_ptr<Camera> camera, std::weak_ptr<GameObject> lodReferenceObj)
	{
		// Create the test jiggy
		auto sphere = m_root->AddChild().lock();
		sphere->GetTransform().SetLocalPosition(pos);
		sphere->GetTransform().SetLocalScale(glm::vec3(0.1f, 0.1f, 0.1f));

		auto& lodModel = sphere->AddComponent<LODModel>().lock();
		lodModel->SetCamera(camera);
		lodModel->SetReferenceObject(lodReferenceObj);
		lodModel->Load("Models/sphere");

		// Add Rigidbody and collider
		//sphere->AddComponent<Rigidbody>();
		//sphere->AddComponent<AABBCollider>();

		return *sphere;
	}

	std::weak_ptr<GameObject> Scene::CreateLink(glm::vec3 pos, std::shared_ptr<Camera> camera)
	{
		// Create the test jiggy
		auto link = m_root->AddChild().lock();
		link->GetTransform().SetLocalPosition(pos);
		link->GetTransform().SetLocalScale(glm::vec3(0.1f, 0.1f, 0.1f));

		auto& lodModel = link->AddComponent<LODModel>().lock();
		lodModel->SetCamera(camera);
		lodModel->Load("Models/testSphere");

		// Add Rigidbody and collider
		link->AddComponent<Rigidbody>();
		link->AddComponent<AABBCollider>();

		// Add character controller component
		link->AddComponent<CharController>();

		CreatePointLight(*link, glm::vec3(0.0f), glm::vec3(1.0f));

		return link;
	}

	GameObject& Scene::CreateFloor(std::shared_ptr<Camera> camera)
	{
		auto floor = m_root->AddChild().lock();
		floor->GetTransform().SetLocalPosition(glm::vec3(-100, -10.0f, -100));
		floor->GetTransform().SetLocalScale(glm::vec3(100.0f, 0.5f, 100.0f));

		auto meshRenderer = floor->AddComponent<MeshRenderer>().lock();
		meshRenderer->SetMesh("Models/cube.obj");
		meshRenderer->SetCamera(camera);

		std::shared_ptr<LitTexturedMat> floorMat = std::make_shared<LitTexturedMat>();
		floorMat->SetTexture("Models/cube.bmp");
		meshRenderer->SetMaterial(floorMat);

		auto rb = floor->AddComponent<Rigidbody>().lock();
		rb->LockPosition();

		auto collider = floor->AddComponent<AABBCollider>().lock();

		return *floor;
	}
}
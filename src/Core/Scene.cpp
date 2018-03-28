#include "stdafx.h"
#include "Scene.h"
#include "Input.h"
#include <Components\AABBCollider.h>
#include <Components\Camera.h>
#include <Components\ControllableCamera.h>
#include <Components\CharController.h>
#include <Components\LODModel.h>
#include <Components\MeshRenderer.h>
#include <Components\Rigidbody.h>
#include <Components\TessModel.h>
#include <Components\TestComponent.h>

#include <Rendering\Mesh.h>
#include <Rendering\Materials\DiscoMat.h>
#include <Rendering\Materials\LitColourMat.h>
#include <Rendering\Materials\LitTexturedMat.h>
#include <Rendering\Materials\SolidColourMat.h>
#include <Rendering\Materials\UnlitTexturedMat.h>
#include <Rendering\Materials\TessellatedMat.h>

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
		auto camera = m_camera->AddComponent<ControllableCamera>().lock();
		m_camera->GetTransform().SetLocalPosition(glm::vec3(0, -4.0f, 18.0f));
		m_camera->GetTransform().SetLocalRotation(glm::vec3(-29.4f, 270.0f, 0.0f));

		// Create directional light
		m_directionalLight = m_root->AddChild().lock();
		auto directionalCamera = m_directionalLight->AddComponent<Camera>().lock();
		auto directionalLight = m_directionalLight->AddComponent<DirectionalLight>().lock();
		m_directionalLight->GetTransform().SetLocalRotation(glm::vec3(-45.0f, 45.0f, -45.4f));
		directionalCamera->SetOrthographic(true);
		directionalLight->SetCamera(directionalCamera);
		
		//CreateJiggy(glm::vec3(15, 0, 0), camera.lock(), texturedMat);
		//CreateJiggy(glm::vec3(-15, 0, 0), camera.lock(), texturedMat);
		//CreateJiggy(glm::vec3(0, 0, 15), camera.lock(), texturedMat);
		auto lodReferenceObj = CreateLink(glm::vec3(-15, -6, -15), camera);
		CreateFloor(camera);
		// Create a ton of spheres
		for (int i = -10; i < 10; i++)
		{
			for (int j = -10; j < 10; j++)
			{
				//CreateSphere(glm::vec3(i*2, 0, j*2), camera, lodReferenceObj);
			}
		}
		// Create a bunch of pretty lights on the floor
		CreatePointLight(*m_root, glm::vec3(10, -6, 0), glm::vec3(1, 0, 1));
		CreatePointLight(*m_root, glm::vec3(-10, -6, 0), glm::vec3(1, 1, 0));
		CreatePointLight(*m_root, glm::vec3(0, -6, 10), glm::vec3(0, 1, 1));
		CreatePointLight(*m_root, glm::vec3(0, -6, -10), glm::vec3(1, 0, 0));
		CreatePointLight(*m_root, glm::vec3(-10, -6, 10), glm::vec3(0, 1, 0));
		CreatePointLight(*m_root, glm::vec3(10, -6, 10), glm::vec3(0, 0, 1));
		CreatePointLight(*m_root, glm::vec3(10, -6, -10), glm::vec3(1, 1, 1));
		CreatePointLight(*m_root, glm::vec3(-10, -6, -10), glm::vec3(1, 1, 1));
		/*/ Create a big light in the center of the scene
		GameObject& bigLight = CreatePointLight(*m_root, glm::vec3(10, -6, -10), glm::vec3(0, 1, 1));
		bigLight.GetComponent<PointLight>()->SetLinearAttenuation(0.001f);
		bigLight.GetComponent<PointLight>()->SetQuadraticAttenuation(0.002f);*/
	}

	void Scene::FixedLogic()
	{
		if (Input::GetKeyDown('t'))
		{
			TessellatedMat::ToggleTessellation();
		}

		LODModel::StartNewFrame();

		m_root->FixedLogic();

		LODModel::SortAndSetLODValues();

		// Generate list of all GameObjects
		//std::vector<std::weak_ptr<GameObject>> allObjects = m_root->GetAllChildren();

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

		/** Shadow Pass */

		Material::ResetCurrentShader();	// Tell Material to use a new shader the next time it is asked - this is dumb
		m_deferredLightingMgr.PrepareNewShadowPass();
		m_root->MainDraw(SHADOW_PASS, *m_directionalLight->GetComponent<Camera>());

		/** Geometry Pass */

		Material::ResetCurrentShader();
		m_deferredLightingMgr.PrepareNewGeometryPass();

		if (Input::GetKeyHeld('f'))
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		// Render all objects in geometry pass to deferred framebuffer
		m_root->MainDraw(GEOMETRY_PASS, *m_camera->GetComponent<Camera>());
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		/** Lighting */

		// Render deferred lighting
		m_deferredLightingMgr.RenderLighting(m_camera->GetComponent<Camera>(), m_pointLights, m_directionalLight->GetComponent<DirectionalLight>());

		Mesh::ResetRenderCount();
	}

	GameObject& Scene::CreatePointLight(GameObject& parent, glm::vec3 pos, glm::vec3 colour)
	{
		auto light = parent.AddChild().lock();
		light->GetTransform().SetLocalPosition(pos);

		auto& pointLight = light->AddComponent<PointLight>().lock();
		pointLight->SetColour(colour);
		pointLight->SetLinearAttenuation(0.01f);
		pointLight->SetQuadraticAttenuation(0.02f);

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
		link->GetTransform().SetLocalScale(glm::vec3(1.0f, 1.0f, 1.0f));
		link->GetTransform().SetLocalRotation(glm::vec3(0, 180, 0));

		auto& lodModel = link->AddComponent<TessModel>().lock();
		lodModel->SetCamera(camera);
		lodModel->Load("Models/crash");

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

		auto& lodModel = floor->AddComponent<LODModel>().lock();
		lodModel->SetCamera(camera);
		lodModel->Load("Models/floor");

		auto rb = floor->AddComponent<Rigidbody>().lock();
		rb->LockPosition();

		auto collider = floor->AddComponent<AABBCollider>().lock();

		return *floor;
	}
}
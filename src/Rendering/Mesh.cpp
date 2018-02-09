#include "stdafx.h"
#include "Mesh.h"
#include <Core\GameObject.h>
#include <Components\Transform.h>
#include <fstream>
#include <sstream>
#include <ostream>
#include <SOIL/SOIL.h>

namespace snes
{
	std::map<std::string, std::shared_ptr<Mesh>> Mesh::m_loadedMeshes;
	uint Mesh::m_verticesRendered = 0;

	Mesh::Mesh(const char* modelPath)
	{
		Load(modelPath);
	}

	std::shared_ptr<Mesh> Mesh::GetMesh(const char* modelPath)
	{
		if (!m_loadedMeshes[modelPath])
		{
			m_loadedMeshes[modelPath] = std::make_shared<Mesh>(Mesh(modelPath));

			if (m_loadedMeshes[modelPath]->GetVertices().size() == 0)
			{
				std::cout << "Error loading mesh: " << modelPath << std::endl;
				return nullptr;
			}
		}

		return m_loadedMeshes[modelPath];
	}

	bool Mesh::Load(const char* modelPath)
	{
		std::ifstream modelFile(modelPath, std::ios::in);

		if (!modelFile)
		{
			std::cout << "Error opening file: " << modelPath << std::endl;
			return false;
		}

		std::vector<glm::vec3> tempVerts;
		std::vector<glm::vec2> tempUVs;
		std::vector<glm::vec3> tempNormals;
		std::vector<uint> vertIndices, uvIndices, normalIndices;
		std::string line;
		uint faceAttributeCount = 0;
		while (std::getline(modelFile, line))
		{
			std::istringstream lineStream(line);
			std::string mode;
			std::getline(lineStream, mode, ' ');

			if (mode == "v")
			{
				/** Vertex information */
				glm::vec3 vert;
				lineStream >> vert.x;
				lineStream >> vert.y;
				lineStream >> vert.z;
				tempVerts.push_back(vert);
			}
			if (mode == "vt")
			{
				/** TexCoord information */
				glm::vec2 texCoord;
				lineStream >> texCoord.x;
				lineStream >> texCoord.y;
				tempUVs.push_back(texCoord);
			}
			if (mode == "vn")
			{
				/** Vertex normal information */
				glm::vec3 normal;
				lineStream >> normal.x;
				lineStream >> normal.y;
				lineStream >> normal.z;
				tempNormals.push_back(normal);
			}
			if (mode == "f")
			{
				/** Face information */
				uint vertIndex[3];	// Vertex indices
				uint uvIndex[3];		// Texture indices
				uint normalIndex[3];	// Normal indices

				if (faceAttributeCount == 0)
				{
					faceAttributeCount = GetFaceAttributeCount(lineStream.str());
				}

				// Load attributes for each vertex in a face
				for (int i = 0; i < 3; ++i)
				{
					std::string attribIndex;
					char delimiter = '/';
					if (faceAttributeCount >= 1)
					{
						if (faceAttributeCount == 1)
						{
							delimiter = ' ';
						}
						std::getline(lineStream, attribIndex, delimiter);
						vertIndex[i] = std::stoi(attribIndex);
					}
					if (faceAttributeCount >= 2)
					{
						if (faceAttributeCount == 2)
						{
							delimiter = ' ';
						}
						std::getline(lineStream, attribIndex, delimiter);
						uvIndex[i] = std::stoi(attribIndex);
					}
					if (faceAttributeCount >= 3)
					{
						if (faceAttributeCount == 3)
						{
							delimiter = ' ';
						}
						std::getline(lineStream, attribIndex, delimiter);
						normalIndex[i] = std::stoi(attribIndex);
					}
				}

				// Store all vertex, uv, and normal indices for this face
				if (faceAttributeCount >= 1)
				{
					vertIndices.push_back(vertIndex[0]);
					vertIndices.push_back(vertIndex[1]);
					vertIndices.push_back(vertIndex[2]);
				}
				if (faceAttributeCount >= 2)
				{
					uvIndices.push_back(uvIndex[0]);
					uvIndices.push_back(uvIndex[1]);
					uvIndices.push_back(uvIndex[2]);
				}
				if (faceAttributeCount >= 3)
				{
					normalIndices.push_back(normalIndex[0]);
					normalIndices.push_back(normalIndex[1]);
					normalIndices.push_back(normalIndex[2]);
				}
			}
		}

		// Generate a vertex mesh from the face data
		// (duplicate some vertices to use with multiple faces)
		if (tempVerts.size() > 0)
		{
			m_vertices.reserve(tempVerts.size());

			for (const auto& vertexIndex : vertIndices)
			{
				glm::vec3 vertex = tempVerts[vertexIndex - 1];
				m_vertices.push_back(vertex);
			}
		}


		if (tempUVs.size() > 0)
		{
			m_texCoords.reserve(tempUVs.size());

			for (const auto& uvIndex : uvIndices)
			{
				glm::vec2 uv = tempUVs[uvIndex - 1];
				m_texCoords.push_back(uv);
			}
		}


		if (tempNormals.size() > 0)
		{
			m_normals.reserve(tempNormals.size());

			for (const auto& normalIndex : normalIndices)
			{
				glm::vec3 normal = tempNormals[normalIndex - 1];
				m_normals.push_back(normal);
			}
		}

		// Create vertex array and buffer objects
		InitialiseVAO();
		glBindVertexArray(m_vertexArrayID);
		InitialiseVBO();

		return true;
	}

	uint Mesh::GetFaceAttributeCount(std::string face)
	{
		// Count the number of '/'s in a face line, divide by 3 (to get one vertex), and add one
		return ((uint)std::count(face.begin(), face.end(), '/') / 3) + 1;
	}

	void Mesh::InitialiseVAO()
	{
		glGenVertexArrays(1, &m_vertexArrayID);
	}

	void Mesh::InitialiseVBO()
	{
		// Create VBO
		uint attribID = 0;
		glGenBuffers(1, &m_vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferID);

		// Add vertex data to VBO
		glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3), &m_vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(attribID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(attribID);
		++attribID;

		// If we have textures, add UV information to VBO
		if (this->HasUVs())
		{
			glGenBuffers(1, &m_uvBufferID);
			glBindBuffer(GL_ARRAY_BUFFER, m_uvBufferID);
			glBufferData(GL_ARRAY_BUFFER, m_texCoords.size() * sizeof(glm::vec2), &m_texCoords[0], GL_STATIC_DRAW);
			glVertexAttribPointer(attribID, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glEnableVertexAttribArray(attribID);
			++attribID;
		}

		// If we have normals, add normals to VBO
		if (this->HasNormals())
		{
			glGenBuffers(1, &m_normalBufferID);
			glBindBuffer(GL_ARRAY_BUFFER, m_normalBufferID);
			glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(glm::vec3), &m_normals[0], GL_STATIC_DRAW);
			glVertexAttribPointer(attribID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glEnableVertexAttribArray(attribID);
			//++attribID;
		}
	}

	const void Mesh::PrepareForRendering() const
	{
		glBindVertexArray(m_vertexArrayID);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferID);
		m_verticesRendered += m_vertices.size();
	}

	void Mesh::ResetRenderCount()
	{
		std::cout << "Vertices rendered: " << m_verticesRendered << std::endl;
		m_verticesRendered = 0;
	}
}

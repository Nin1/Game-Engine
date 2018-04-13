#include "stdafx.h"
#include "Mesh.h"
#include <Core\GameObject.h>
#include <Components\Transform.h>
#include <algorithm>
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

	std::shared_ptr<Mesh> Mesh::GetMesh(const char* modelPath, bool withNeighbourData)
	{
		// NOTE: If trying to get a mesh that has already been loaded, but has had neighbour data generated,
		//		 the mesh won't work when neighbour data isn't desired.

		std::string modelID = modelPath;

		if (withNeighbourData)
		{
			modelID.append("n");
		}

		if (!m_loadedMeshes[modelID])
		{
			m_loadedMeshes[modelID] = std::make_shared<Mesh>(Mesh(modelPath));

			if (m_loadedMeshes[modelID]->GetVertices().size() == 0)
			{
				std::cout << "Error loading mesh: " << modelPath << std::endl;
				return nullptr;
			}

			if (withNeighbourData)
			{
				m_loadedMeshes[modelID]->GenNeighbourData();
			}
		}

		return m_loadedMeshes[modelID];
	}

	bool Mesh::Load(const char* modelPath)
	{
		/** Load the model file */

		std::ifstream modelFile(modelPath, std::ios::in);

		if (!modelFile)
		{
			std::cout << "Error opening file: " << modelPath << std::endl;
			return false;
		}

		/** Set up temporary containers */

		std::vector<glm::vec3> tempVerts;
		std::vector<glm::vec2> tempUVs;
		std::vector<glm::vec3> tempNormals;
		std::vector<glm::vec3> tempTangents;
		std::vector<glm::vec3> tempBitangents;
		std::vector<uint> vertIndices, uvIndices, normalIndices;
		std::string line;
		uint faceAttributeCount = 0;

		/** Parse the model file */

		while (std::getline(modelFile, line))
		{
			std::istringstream lineStream(line);
			lineStream.imbue(std::locale(lineStream.getloc(), new ObjLoadDelimiters));

			std::string mode;
			lineStream >> mode;

			if (mode == "v")
			{
				/** Vertex information */
				glm::vec3 vert;
				lineStream >> vert.x;
				lineStream >> vert.y;
				lineStream >> vert.z;

				// Find biggest distance between any two vertices
				for (const auto& vertex : tempVerts)
				{
					float dist = glm::length(vertex - vert);
					if (dist > m_size)
					{
						m_size = dist;
					}
				}

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
					if (tempVerts.size() > 0)
					{
						lineStream >> vertIndex[i];
					}
					if (tempUVs.size() > 0)
					{
						if (faceAttributeCount >= 2)
						{
							lineStream >> uvIndex[i];
						}
						else
						{
							uvIndex[i] = vertIndex[i];
						}
					}
					if (tempNormals.size() > 0)
					{
						if (faceAttributeCount >= 3)
						{
							lineStream >> normalIndex[i];
						}
						else
						{
							normalIndex[i] = vertIndex[i];
						}
					}
				}

				// Store all vertex, uv, and normal indices for this face
				if (tempVerts.size() > 0)
				{
					vertIndices.push_back(vertIndex[0]);
					vertIndices.push_back(vertIndex[1]);
					vertIndices.push_back(vertIndex[2]);
				}
				if (tempUVs.size() > 0)
				{
					uvIndices.push_back(uvIndex[0]);
					uvIndices.push_back(uvIndex[1]);
					uvIndices.push_back(uvIndex[2]);
				}
				if (tempNormals.size() > 0)
				{
					normalIndices.push_back(normalIndex[0]);
					normalIndices.push_back(normalIndex[1]);
					normalIndices.push_back(normalIndex[2]);
				}
			}
		}

		/** Convert to usable mesh data */

		// Generate a vertex mesh from the face data
		// (duplicate some vertices to use with multiple faces)
		if (tempVerts.size() > 0)
		{
			m_vertices.reserve(vertIndices.size());

			for (const auto& vertexIndex : vertIndices)
			{
				glm::vec3 vertex = tempVerts[vertexIndex - 1];
				m_vertices.push_back(vertex);
			}

			m_numFaces = m_vertices.size() / 3;
		}

		/** UVs */

		if (tempUVs.size() > 0)
		{
			m_texCoords.reserve(tempUVs.size());

			for (const auto& uvIndex : uvIndices)
			{
				glm::vec2 uv = tempUVs[uvIndex - 1];
				m_texCoords.push_back(uv);
			}
		}

		/** Vertex normals */

		// Set up container for calculating normals
		std::vector<glm::vec3> calculatedNormals;
		for (const auto& vertex : tempVerts)
		{
			calculatedNormals.push_back(glm::vec3(0));
		}

		// For each face, calculate its contribution to each of its vertex's normals
		for (uint i = 0; i < vertIndices.size(); i += 3)
		{
			glm::vec3 edge1 = tempVerts[vertIndices[i + 1] - 1] - tempVerts[vertIndices[i] - 1];
			glm::vec3 edge2 = tempVerts[vertIndices[i + 2] - 1] - tempVerts[vertIndices[i] - 1];
			glm::vec3 surfaceNormal = glm::cross(edge1, edge2);

			calculatedNormals[vertIndices[i] - 1] = glm::normalize(calculatedNormals[vertIndices[i] - 1] + surfaceNormal);
			calculatedNormals[vertIndices[i + 1] - 1] = glm::normalize(calculatedNormals[vertIndices[i + 1] - 1] + surfaceNormal);
			calculatedNormals[vertIndices[i + 2] - 1] = glm::normalize(calculatedNormals[vertIndices[i + 2] - 1] + surfaceNormal);
		}

		m_normals.reserve(vertIndices.size());
		for (const auto& vertexIndex : vertIndices)
		{
			glm::vec3 normal = calculatedNormals[vertexIndex - 1];
			m_normals.push_back(normal);
		}

		/*
		if (tempNormals.size() > 0)
		{
			m_normals.reserve(tempNormals.size());

			for (const auto& normalIndex : normalIndices)
			{
				glm::vec3 normal = tempNormals[normalIndex - 1];
				m_normals.push_back(normal);
			}
		}*/

		// Create vertex array and buffer objects
		InitialiseVAO();
		glBindVertexArray(m_vertexArrayID);
		InitialiseVBO();

		return true;
	}

	uint Mesh::GetFaceAttributeCount(std::string face)
	{
		// Count the number of '/'s in a face line, divide by 3 (to get one vertex), and add one
		return ((uint)std::count(face.begin() + 1, face.end(), '/') / 3) + 1;
	}

	void Mesh::GenNeighbourData()
	{
		// Generate vertex data with neighbours
		// Faces will be stored as 6 vertices, where:
		// Vertices 0 - 2: Face vertices
		// Vertex 3: Neighbour of edge 0 - 1
		// Vertex 4: Neighbour of edge 1 - 2
		// Vertex 5: Neighbour of edge 2 - 0

		std::vector<glm::vec3> verticesWithNeighbours;
		verticesWithNeighbours.reserve(m_vertices.size() * 2);
		std::vector<glm::vec2> uvsWithNeighbours;
		uvsWithNeighbours.reserve(m_texCoords.size() * 2);
		std::vector<glm::vec3> normalsWithNeighbours;
		normalsWithNeighbours.reserve(m_normals.size() * 2);

		// For each face, check each of its edges against all edges of all other faces.
		// If a match is found, the remaining vertex of the other face is that edge's neighbour

		// faceStart = first vertex in face
		for (uint faceStart = 0; faceStart < m_vertices.size(); faceStart += 3)
		{
			// Add the original face vertices
			verticesWithNeighbours.push_back(m_vertices[faceStart]);
			verticesWithNeighbours.push_back(m_vertices[faceStart + 1]);
			verticesWithNeighbours.push_back(m_vertices[faceStart + 2]);
			if (this->HasUVs())
			{
				uvsWithNeighbours.push_back(m_texCoords[faceStart]);
				uvsWithNeighbours.push_back(m_texCoords[faceStart + 1]);
				uvsWithNeighbours.push_back(m_texCoords[faceStart + 2]);
			}
			if (this->HasNormals())
			{
				normalsWithNeighbours.push_back(m_normals[faceStart]);
				normalsWithNeighbours.push_back(m_normals[faceStart + 1]);
				normalsWithNeighbours.push_back(m_normals[faceStart + 2]);
			}

			// edgeStart = first vertex in edge (0-1, 1-2, 2-0)
			for (uint edgeStart = 0; edgeStart < 3; ++edgeStart)
			{
				int edgeEnd = (edgeStart + 1) % 3;
				bool neighbourFound = false;

				for (uint otherFaceStart = 0; otherFaceStart < m_vertices.size(); otherFaceStart += 3)
				{
					// Don't check against the same face
					if (otherFaceStart == faceStart)
					{
						continue;
					}

					bool sharedVertex[3] = { false };

					// If two vertices from face i are in the same position as two vertices from face j, mark them
					for (uint i = 0; i < 3; ++i)
					{
						if (m_vertices[otherFaceStart + i] == m_vertices[faceStart + edgeStart] ||
							m_vertices[otherFaceStart + i] == m_vertices[faceStart + edgeEnd])
						{
							sharedVertex[i] = true;
						}
					}

					int neighbourIndex = -1;

					if (sharedVertex[0] && sharedVertex[1])
					{
						neighbourIndex = otherFaceStart + 2;
					}
					else if (sharedVertex[1] && sharedVertex[2])
					{
						neighbourIndex = otherFaceStart;
					}
					else if (sharedVertex[2] && sharedVertex[0])
					{
						neighbourIndex = otherFaceStart + 1;
					}

					// If a neighbour was found, add it to the new vectors
					if (neighbourIndex > -1)
					{
						verticesWithNeighbours.push_back(m_vertices[neighbourIndex]);
						if (this->HasUVs())
						{
							uvsWithNeighbours.push_back(m_texCoords[neighbourIndex]);
						}
						if (this->HasNormals())
						{
							normalsWithNeighbours.push_back(m_normals[neighbourIndex]);
						}
						neighbourFound = true;
						break;
					}
				}

				if (!neighbourFound)
				{
					// If no neighbour was found, use the first vertex of the edge as a placeholder
					verticesWithNeighbours.push_back(m_vertices[faceStart + edgeStart]);
					if (this->HasUVs())
					{
						uvsWithNeighbours.push_back(m_texCoords[faceStart + edgeStart]);
					}
					if (this->HasNormals())
					{
						normalsWithNeighbours.push_back(m_normals[faceStart + edgeStart]);
					}
				}
			}
		}

		m_vertices = verticesWithNeighbours;
		m_texCoords = uvsWithNeighbours;
		m_normals = normalsWithNeighbours;

		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3), &m_vertices[0], GL_STATIC_DRAW);
		if (this->HasUVs())
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_uvBufferID);
			glBufferData(GL_ARRAY_BUFFER, m_texCoords.size() * sizeof(glm::vec2), &m_texCoords[0], GL_STATIC_DRAW);
		}
		if (this->HasNormals())
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_normalBufferID);
			glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(glm::vec3), &m_normals[0], GL_STATIC_DRAW);
		}
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
		//std::cout << "Vertices rendered: " << m_verticesRendered << std::endl;
		m_verticesRendered = 0;
	}
}

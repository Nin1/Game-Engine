#pragma once
#include <GL\glew.h>
#include <glm\vec2.hpp>
#include <glm\vec3.hpp>
#include <map>

namespace snes
{
	class Mesh
	{
	public:
		~Mesh() {};

		/** @return true if the mesh has texture coordinates */
		bool HasUVs() const { return m_texCoords.size() > 0; }
		/** @return true if the mesh has vertex normals */
		bool HasNormals() const { return m_normals.size() > 0; }

		/** @return a list of all the vertices in the mesh */
		const std::vector<glm::vec3>& GetVertices() const { return m_vertices; }
		/** @return a list of the texture coordinates for each vertex */
		const std::vector<glm::vec2>& GetUVs() const { return m_texCoords; }
		/** @return a list of the normals for each vertex */
		const std::vector<glm::vec3>& GetNormals() const { return m_normals; }
		/** @return the texture ID of the mesh (only 1 texture supported) */
		GLuint GetTextureID() const { return m_textureID; }

		/** @return the number of vertices in the mesh */
		uint GetVertexCount() const { return (uint)m_vertices.size();	}

		const void PrepareForRendering() const;

	public:
		/** Returns the mesh data from the mesh at the given path */
		static std::shared_ptr<Mesh> GetMesh(const char* modelPath);

	private:
		Mesh(const char* modelPath);

		/** Load the given mesh */
		bool Load(const char* modelPath);

		/** Cache of all loaded meshes */
		static std::map<std::string, std::shared_ptr<Mesh>> m_loadedMeshes;

	private:
		uint GetFaceAttributeCount(std::string face);

		void InitialiseVAO();
		void InitialiseVBO();

		std::vector<glm::vec3> m_vertices;
		std::vector<glm::vec2> m_texCoords;
		std::vector<glm::vec3> m_normals;

		GLuint m_vertexArrayID = -1;
		GLuint m_vertexBufferID = -1;
		GLuint m_uvBufferID = -1;
		GLuint m_normalBufferID = -1;
		GLuint m_textureID = 0;
	};
}

#pragma once

#include "../Mesh.hpp"

class Cube
{
public:
	static Mesh CreateCube(std::string const& shaderName, float size) {
		// Vertex buffer (positions)
		float verticesTemp[] = {
			// Front face
			-0.5f * size, -0.5f * size,  0.5f * size,  // 0 - bottom left
			 0.5f * size, -0.5f * size,  0.5f * size,  // 1 - bottom right
			 0.5f * size,  0.5f * size,  0.5f * size,  // 2 - top right
			-0.5f * size,  0.5f * size,  0.5f * size,  // 3 - top left

			// Back face
			-0.5f * size, -0.5f * size, -0.5f * size,  // 4 - bottom left
			 0.5f * size, -0.5f * size, -0.5f * size,  // 5 - bottom right
			 0.5f * size,  0.5f * size, -0.5f * size,  // 6 - top right
			-0.5f * size,  0.5f * size, -0.5f * size   // 7 - top left
		};

		// Color buffer (RGBA colors per vertex)
		float colorsTemp[] = {
			// Front face
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f
		};

		float normalsTemp[] = {
			// Front face
			-0.577f, -0.577f,  0.577f,  // 0 - bottom left
			 0.577f, -0.577f,  0.577f,  // 1 - bottom right
			 0.577f,  0.577f,  0.577f,  // 2 - top right
			-0.577f,  0.577f,  0.577f,  // 3 - top left

			// Back face
			-0.577f, -0.577f, -0.577f,  // 4 - bottom left
			 0.577f, -0.577f, -0.577f,  // 5 - bottom right
			 0.577f,  0.577f, -0.577f,  // 6 - top right
			-0.577f,  0.577f, -0.577f   // 7 - top left
		};

		// Index buffer (defining triangles)
		unsigned int indicesTemp[] = {
			// Front face
			0, 1, 2,
			2, 3, 0,
			// Right face
			1, 5, 6,
			6, 2, 1,
			// Back face
			5, 4, 7,
			7, 6, 5,
			// Left face
			4, 0, 3,
			3, 7, 4,
			// Top face
			3, 2, 6,
			6, 7, 3,
			// Bottom face
			4, 5, 1,
			1, 0, 4
		};

		std::shared_ptr<float[]> vertices(new float[24], std::default_delete<float[]>());
		std::shared_ptr<float[]> colors(new float[24], std::default_delete<float[]>());
		std::shared_ptr<float[]> normals(new float[24], std::default_delete<float[]>());
		std::shared_ptr<unsigned int[]> indices(new unsigned int[36], std::default_delete<unsigned int[]>());

		memcpy(vertices.get(), verticesTemp, sizeof(verticesTemp));
		memcpy(colors.get(), colorsTemp, sizeof(colorsTemp));
		memcpy(normals.get(), normalsTemp, sizeof(normalsTemp));
		memcpy(indices.get(), indicesTemp, sizeof(indicesTemp));

		Mesh mesh = Mesh(shaderName);

		mesh.registerMesh();
		mesh.updateMesh(vertices, sizeof(vertices), colors, sizeof(colors), normals, sizeof(normals), indices, sizeof(indices));

		return mesh;
	}

	static Mesh CreateCube(std::string const& shaderName, float size, glm::vec3 colour) {
		// Vertex buffer (positions)
		float verticesTemp[] = {
			// Front face
			-0.5f * size, -0.5f * size,  0.5f * size,  // 0 - bottom left
			 0.5f * size, -0.5f * size,  0.5f * size,  // 1 - bottom right
			 0.5f * size,  0.5f * size,  0.5f * size,  // 2 - top right
			-0.5f * size,  0.5f * size,  0.5f * size,  // 3 - top left

			// Back face
			-0.5f * size, -0.5f * size, -0.5f * size,  // 4 - bottom left
			 0.5f * size, -0.5f * size, -0.5f * size,  // 5 - bottom right
			 0.5f * size,  0.5f * size, -0.5f * size,  // 6 - top right
			-0.5f * size,  0.5f * size, -0.5f * size   // 7 - top left
		};

		// Color buffer (RGBA colors per vertex)
		float colorsTemp[] = {
			// Front face
			colour.x, colour.y, colour.z,
			colour.x, colour.y, colour.z,
			colour.x, colour.y, colour.z,
			colour.x, colour.y, colour.z,
			colour.x, colour.y, colour.z,
			colour.x, colour.y, colour.z,
			colour.x, colour.y, colour.z,
			colour.x, colour.y, colour.z
		};

		float normalsTemp[] = {
			// Front face
			-0.577f, -0.577f,  0.577f,  // 0 - bottom left
			 0.577f, -0.577f,  0.577f,  // 1 - bottom right
			 0.577f,  0.577f,  0.577f,  // 2 - top right
			-0.577f,  0.577f,  0.577f,  // 3 - top left

			// Back face
			-0.577f, -0.577f, -0.577f,  // 4 - bottom left
			 0.577f, -0.577f, -0.577f,  // 5 - bottom right
			 0.577f,  0.577f, -0.577f,  // 6 - top right
			-0.577f,  0.577f, -0.577f   // 7 - top left
		};

		// Index buffer (defining triangles)
		unsigned int indicesTemp[] = {
			// Front face
			0, 1, 2,
			2, 3, 0,
			// Right face
			1, 5, 6,
			6, 2, 1,
			// Back face
			5, 4, 7,
			7, 6, 5,
			// Left face
			4, 0, 3,
			3, 7, 4,
			// Top face
			3, 2, 6,
			6, 7, 3,
			// Bottom face
			4, 5, 1,
			1, 0, 4
		};

		std::shared_ptr<float[]> vertices(new float[24], std::default_delete<float[]>());
		std::shared_ptr<float[]> colors(new float[24], std::default_delete<float[]>());
		std::shared_ptr<float[]> normals(new float[24], std::default_delete<float[]>());
		std::shared_ptr<unsigned int[]> indices(new unsigned int[36], std::default_delete<unsigned int[]>());

		memcpy(vertices.get(), verticesTemp, sizeof(verticesTemp));
		memcpy(colors.get(), colorsTemp, sizeof(colorsTemp));
		memcpy(normals.get(), normalsTemp, sizeof(normalsTemp));
		memcpy(indices.get(), indicesTemp, sizeof(indicesTemp));

		Mesh mesh = Mesh(shaderName);

		mesh.registerMesh();
		mesh.updateMesh(vertices, sizeof(vertices), colors, sizeof(colors), normals, sizeof(normals), indices, sizeof(indices));

		return mesh;
	}
};


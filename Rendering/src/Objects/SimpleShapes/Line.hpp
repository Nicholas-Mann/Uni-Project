#pragma once

#include <glm/glm.hpp>
#include "../Mesh.hpp"
#include <memory>

class Line
{
public:
	static Mesh CreateLine(std::string const& shaderName, glm::vec3 start, glm::vec3 end, float thickness) {
		glm::vec3 direction = glm::normalize(end - start);

		glm::vec3 perpendicular = glm::normalize(glm::cross(direction, glm::vec3(0, 0, -1))) * thickness * 0.5f;
		if (direction.z > 0.999f || direction.z < -0.999f) {
			perpendicular = glm::normalize(glm::cross(direction, glm::vec3(0, -1, 0))) * thickness * 0.5f;
		}

		glm::vec3 depthOffset = glm::normalize(glm::cross(direction, perpendicular)) * thickness * 0.5f;

		// Define 8 cuboid vertices
		glm::vec3 v0 = start + perpendicular + depthOffset;
		glm::vec3 v1 = start - perpendicular + depthOffset;
		glm::vec3 v2 = end - perpendicular + depthOffset;
		glm::vec3 v3 = end + perpendicular + depthOffset;

		glm::vec3 v4 = start + perpendicular - depthOffset;
		glm::vec3 v5 = start - perpendicular - depthOffset;
		glm::vec3 v6 = end - perpendicular - depthOffset;
		glm::vec3 v7 = end + perpendicular - depthOffset;

		float verticesTemp[] = {
			v0.x, v0.y, v0.z,  v1.x, v1.y, v1.z,  v2.x, v2.y, v2.z,  v3.x, v3.y, v3.z,
			v4.x, v4.y, v4.z,  v5.x, v5.y, v5.z,  v6.x, v6.y, v6.z,  v7.x, v7.y, v7.z
		};

		float colorsTemp[8 * 3];
		for (int i = 0; i < 8; i++) {
			colorsTemp[i * 3] = 1.0f;
			colorsTemp[i * 3 + 1] = 1.0f;
			colorsTemp[i * 3 + 2] = 1.0f;
		}

		float normalsTemp[] = {
			0, 0, 1,   0, 0, 1,   0, 0, 1,   0, 0, 1,   // Front
			0, 0, -1,  0, 0, -1,  0, 0, -1,  0, 0, -1   // Back
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

	static Mesh CreateLine(std::string const& shaderName, glm::vec3 start, glm::vec3 end, float thickness, glm::vec3 colour) {
		glm::vec3 direction = glm::normalize(end - start);

		glm::vec3 perpendicular = glm::normalize(glm::cross(direction, glm::vec3(0, 0, -1))) * thickness * 0.5f;
		if (direction.z > 0.999f || direction.z < -0.999f) {
			perpendicular = glm::normalize(glm::cross(direction, glm::vec3(0, -1, 0))) * thickness * 0.5f;
		}

		glm::vec3 depthOffset = glm::normalize(glm::cross(direction, perpendicular)) * thickness * 0.5f;

		// Define 8 cuboid vertices
		glm::vec3 v0 = start + perpendicular + depthOffset;
		glm::vec3 v1 = start - perpendicular + depthOffset;
		glm::vec3 v2 = end - perpendicular + depthOffset;
		glm::vec3 v3 = end + perpendicular + depthOffset;

		glm::vec3 v4 = start + perpendicular - depthOffset;
		glm::vec3 v5 = start - perpendicular - depthOffset;
		glm::vec3 v6 = end - perpendicular - depthOffset;
		glm::vec3 v7 = end + perpendicular - depthOffset;

		float verticesTemp[] = {
			v0.x, v0.y, v0.z,  v1.x, v1.y, v1.z,  v2.x, v2.y, v2.z,  v3.x, v3.y, v3.z,
			v4.x, v4.y, v4.z,  v5.x, v5.y, v5.z,  v6.x, v6.y, v6.z,  v7.x, v7.y, v7.z
		};

		float colorsTemp[8 * 3];
		for (int i = 0; i < 8; i++) {
			colorsTemp[i * 3] = 1.0f;
			colorsTemp[i * 3 + 1] = 1.0f;
			colorsTemp[i * 3 + 2] = 1.0f;
		}

		float normalsTemp[] = {
			0, 0, 1,   0, 0, 1,   0, 0, 1,   0, 0, 1,   // Front
			0, 0, -1,  0, 0, -1,  0, 0, -1,  0, 0, -1   // Back
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

	static void CreateLine(float returnVertices[], float returnColors[], float returnNormals[], unsigned int returnIndices[],
		glm::vec3 start, glm::vec3 end, glm::vec3 offset, int indexOffset, float thickness, glm::vec3 colour) {

		glm::vec3 direction = glm::normalize(end - start);
		
		glm::vec3 perpendicular = glm::normalize(glm::cross(direction, glm::vec3(0, 0, -1))) * thickness * 0.5f;
		if (direction.z > 0.999f || direction.z < -0.999f) {
			perpendicular = glm::normalize(glm::cross(direction, glm::vec3(0, -1, 0))) * thickness * 0.5f;
		}

		glm::vec3 depthOffset = glm::normalize(glm::cross(direction, perpendicular)) * thickness * 0.5f;

		// Define 8 cuboid vertices
		glm::vec3 v0 = start + perpendicular + depthOffset - offset;
		glm::vec3 v1 = start - perpendicular + depthOffset - offset;
		glm::vec3 v2 = end - perpendicular + depthOffset - offset;
		glm::vec3 v3 = end + perpendicular + depthOffset - offset;

		glm::vec3 v4 = start + perpendicular - depthOffset - offset;
		glm::vec3 v5 = start - perpendicular - depthOffset - offset;
		glm::vec3 v6 = end - perpendicular - depthOffset - offset;
		glm::vec3 v7 = end + perpendicular - depthOffset - offset;

		float vertices[] = {
			v0.x, v0.y, v0.z,  v1.x, v1.y, v1.z,  v2.x, v2.y, v2.z,  v3.x, v3.y, v3.z,
			v4.x, v4.y, v4.z,  v5.x, v5.y, v5.z,  v6.x, v6.y, v6.z,  v7.x, v7.y, v7.z
		};

		float colors[8 * 3];
		for (int i = 0; i < 8; i++) {
			colors[i * 3] = colour.r;
			colors[i * 3 + 1] = colour.g;
			colors[i * 3 + 2] = colour.b;
		}

		float normals[] = {
			0, 0, 1,   0, 0, 1,   0, 0, 1,   0, 0, 1,   // Front
			0, 0, -1,  0, 0, -1,  0, 0, -1,  0, 0, -1   // Back
		};


		// Index buffer (defining triangles)
		unsigned int indices[] = {
			// Front face
			0 + indexOffset, 1 + indexOffset, 2 + indexOffset,
			2 + indexOffset, 3 + indexOffset, 0 + indexOffset,
			// Right face
			1 + indexOffset, 5 + indexOffset, 6 + indexOffset,
			6 + indexOffset, 2 + indexOffset, 1 + indexOffset,
			// Back face
			5 + indexOffset, 4 + indexOffset, 7 + indexOffset,
			7 + indexOffset, 6 + indexOffset, 5 + indexOffset,
			// Left face
			4 + indexOffset, 0 + indexOffset, 3 + indexOffset,
			3 + indexOffset, 7 + indexOffset, 4 + indexOffset,
			// Top face
			3 + indexOffset, 2 + indexOffset, 6 + indexOffset,
			6 + indexOffset, 7 + indexOffset, 3 + indexOffset,
			// Bottom face
			4 + indexOffset, 5 + indexOffset, 1 + indexOffset,
			1 + indexOffset, 0 + indexOffset, 4 + indexOffset
		};

		std::memcpy(returnVertices, vertices, sizeof(vertices));
		std::memcpy(returnColors, colors, sizeof(colors));
		std::memcpy(returnNormals, normals, sizeof(normals));
		std::memcpy(returnIndices, indices, sizeof(indices));
	}


	static void CreateLine(float returnVertices[], float returnColors[], float returnNormals[], unsigned int returnIndices[], glm::vec3 start, glm::vec3 end, glm::vec3 offset, int indexOffset, float thickness) {

		glm::vec3 direction = glm::normalize(end - start);

		glm::vec3 perpendicular = glm::normalize(glm::cross(direction, glm::vec3(0, 0, -1))) * thickness * 0.5f;
		if (direction.z > 0.999f || direction.z < -0.999f) {
			perpendicular = glm::normalize(glm::cross(direction, glm::vec3(0, -1, 0))) * thickness * 0.5f;
		}

		glm::vec3 depthOffset = glm::normalize(glm::cross(direction, perpendicular)) * thickness * 0.5f;

		// Define 8 cuboid vertices
		glm::vec3 v0 = start + perpendicular + depthOffset + offset;
		glm::vec3 v1 = start - perpendicular + depthOffset + offset;
		glm::vec3 v2 = end - perpendicular + depthOffset + offset;
		glm::vec3 v3 = end + perpendicular + depthOffset + offset;

		glm::vec3 v4 = start + perpendicular - depthOffset + offset;
		glm::vec3 v5 = start - perpendicular - depthOffset + offset;
		glm::vec3 v6 = end - perpendicular - depthOffset + offset;
		glm::vec3 v7 = end + perpendicular - depthOffset + offset;

		float vertices[] = {
			v0.x, v0.y, v0.z,  v1.x, v1.y, v1.z,  v2.x, v2.y, v2.z,  v3.x, v3.y, v3.z,
			v4.x, v4.y, v4.z,  v5.x, v5.y, v5.z,  v6.x, v6.y, v6.z,  v7.x, v7.y, v7.z
		};

		float colors[8 * 3];
		for (int i = 0; i < 8; i++) {
			colors[i * 3] = 1.0f;
			colors[i * 3 + 1] = 1.0f;
			colors[i * 3 + 2] = 1.0f;
		}

		float normals[] = {
			0, 0, 1,   0, 0, 1,   0, 0, 1,   0, 0, 1,   // Front
			0, 0, -1,  0, 0, -1,  0, 0, -1,  0, 0, -1   // Back
		};


		// Index buffer (defining triangles)
		unsigned int indices[] = {
			// Front face
			0 + indexOffset, 1 + indexOffset, 2 + indexOffset,
			2 + indexOffset, 3 + indexOffset, 0 + indexOffset,
			// Right face
			1 + indexOffset, 5 + indexOffset, 6 + indexOffset,
			6 + indexOffset, 2 + indexOffset, 1 + indexOffset,
			// Back face
			5 + indexOffset, 4 + indexOffset, 7 + indexOffset,
			7 + indexOffset, 6 + indexOffset, 5 + indexOffset,
			// Left face
			4 + indexOffset, 0 + indexOffset, 3 + indexOffset,
			3 + indexOffset, 7 + indexOffset, 4 + indexOffset,
			// Top face
			3 + indexOffset, 2 + indexOffset, 6 + indexOffset,
			6 + indexOffset, 7 + indexOffset, 3 + indexOffset,
			// Bottom face
			4 + indexOffset, 5 + indexOffset, 1 + indexOffset,
			1 + indexOffset, 0 + indexOffset, 4 + indexOffset
		};

		std::memcpy(returnVertices, vertices, sizeof(vertices));
		std::memcpy(returnColors, colors, sizeof(colors));
		std::memcpy(returnNormals, normals, sizeof(normals));
		std::memcpy(returnIndices, indices, sizeof(indices));
	}
	


private:

};
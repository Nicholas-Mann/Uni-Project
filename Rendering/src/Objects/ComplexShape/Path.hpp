#pragma once

#include "../Object.hpp"
#include "../SimpleShapes/Line.hpp"

class Path
{
public:

	static inline Mesh GeneratePath(std::string const& shaderName, std::vector<glm::vec3> const& points, float thickness) {
		glm::vec3 start = points[0];

		std::shared_ptr<float[]> vertices(new float[3 * 8 * points.size()], std::default_delete<float[]>());
		std::shared_ptr<float[]> colors(new float[3 * 8 * points.size()], std::default_delete<float[]>());
		std::shared_ptr<float[]> normals(new float[3 * 8 * points.size()], std::default_delete<float[]>());
		std::shared_ptr<unsigned int[]> indices(new unsigned int[6 * 6 * points.size()], std::default_delete<unsigned int[]>());

		float* vertexPointer = vertices.get();
		float* colorPointer = colors.get();
		float* normalPointer = normals.get();
		unsigned int* indexPointer = indices.get();

		for (int i = 0; i < points.size() - 1; i++) {
			float* returnVertices = vertexPointer;
			float* returnColors = colorPointer;
			float* reurnNormals = normalPointer;
			unsigned int* reutnIndices = indexPointer;
			//Line::CreateLine(returnVertices, returnColors, reurnNormals, reutnIndices, points[i], points[i + 1], startPos, i * 8, thickness);
			vertexPointer += 3 * 8;
			colorPointer += 3 * 8;
			normalPointer += 3 * 8;
			indexPointer += 6 * 6;
		}

		Mesh mesh = Mesh(shaderName);
		mesh.registerMesh();
		mesh.updateMesh(vertices, sizeof(float) * 3 * 8 * points.size(), colors, sizeof(float) * 3 * 8 * points.size(), normals, sizeof(float) * 3 * 8 * points.size(), indices, sizeof(unsigned int) * 6 * 6 * points.size());

		return mesh;
	}

	static inline Mesh GeneratePath(std::string const& shaderName, std::vector<glm::vec3> const& points, float thickness, glm::vec3 colour) {
		glm::vec3 start = points[0];

		std::shared_ptr<float[]> vertices(new float[3 * 8 * points.size()], std::default_delete<float[]>());
		std::shared_ptr<float[]> colors(new float[3 * 8 * points.size()], std::default_delete<float[]>());
		std::shared_ptr<float[]> normals(new float[3 * 8 * points.size()], std::default_delete<float[]>());
		std::shared_ptr<unsigned int[]> indices(new unsigned int[6 * 6 * points.size()], std::default_delete<unsigned int[]>());

		float* vertexPointer = vertices.get();
		float* colorPointer = colors.get();
		float* normalPointer = normals.get();
		unsigned int* indexPointer = indices.get();

		glm::vec3 startPos = points[0];

		for (int i = 0; i < points.size() - 1; i++) {
			float* returnVertices = vertexPointer;
			float* returnColors = colorPointer;
			float* reurnNormals = normalPointer;
			unsigned int* reutnIndices = indexPointer;
			Line::CreateLine(returnVertices, returnColors, reurnNormals, reutnIndices, points[i], points[i + 1], startPos, i * 8, thickness, colour);
			vertexPointer += 3 * 8;
			colorPointer += 3 * 8;
			normalPointer += 3 * 8;
			indexPointer += 6 * 6;
		}

		Mesh mesh = Mesh(shaderName);
		mesh.registerMesh();
		mesh.updateMesh(vertices, sizeof(float) * 3 * 8 * (points.size() - 1), colors, sizeof(float) * 3 * 8 * (points.size() - 1), normals, sizeof(float) * 3 * 8 * (points.size() - 1), indices, sizeof(unsigned int) * 6 * 6 * (points.size() - 1));

		return mesh;
	}
};
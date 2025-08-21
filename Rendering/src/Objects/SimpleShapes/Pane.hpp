#pragma once

#include "../Mesh.hpp"

class Pane
{
public:
	static Mesh CreateTerrain(std::string const& shaderName, float* heightMap, float triagleSize, int chunkSize) {

		int pointSize = chunkSize * chunkSize * 3;

		std::shared_ptr<float[]> vertices(new float[pointSize]);
		std::shared_ptr<float[]> colors(new float[pointSize]);
		std::shared_ptr<float[]> normals(new float[pointSize]);
		std::shared_ptr<unsigned int[]> indices(new unsigned int[(chunkSize - 1) * (chunkSize - 1) * 6]);

		int index = 0;

		for (int x = 1; x < (chunkSize + 1); x++) {
			for (int z = 1; z < (chunkSize + 1); z++) {
				vertices[index] = (x - 1) * triagleSize;
				vertices[index + 1] = heightMap[x * (chunkSize + 2) + z];
				vertices[index + 2] = (z - 1) * triagleSize;

				//colors[index] = 1.0f;
				//colors[index + 1] = 1.0f;
				//colors[index + 2] = 1.0f;


				// Calculate the normals
				if (x > 0 && z > 0 && x < (chunkSize + 2) - 1 && z < (chunkSize + 2) - 1) {
					float hL = heightMap[(x - 1) * (chunkSize + 2) + z];
					float hR = heightMap[(x + 1) * (chunkSize + 2) + z];
					float hD = heightMap[x * (chunkSize + 2) + z - 1];
					float hU = heightMap[x * (chunkSize + 2) + z + 1];

					normals[index] = hL - hR;
					normals[index + 1] = 2.0f;
					normals[index + 2] = hD - hU;
				}
				else
				{
					normals[index] = 0.0f;
					normals[index + 1] = 1.0f;
					normals[index + 2] = 0.0f;
				}

				//normals[index] = 0.0f;
				//normals[index + 1] = 1.0f;
				//normals[index + 2] = 0.0f;
				index += 3;
			}
		}

		index = 0;

		for (int x = 0; x < chunkSize - 1; x++) {
			for (int z = 0; z < chunkSize - 1; z++) {
				indices[index] = x * chunkSize + z;
				indices[index + 1] = x * chunkSize + z + 1;
				indices[index + 2] = (x + 1) * chunkSize + z;
				indices[index + 3] = x * chunkSize + z + 1;
				indices[index + 4] = (x + 1) * chunkSize + z + 1;
				indices[index + 5] = (x + 1) * chunkSize + z;
				index += 6;
			}
		}

		Mesh mesh = Mesh(shaderName);

		mesh.registerMesh();
		mesh.updateMesh(vertices, pointSize * sizeof(float), colors, pointSize * sizeof(float), normals, pointSize * sizeof(float), indices, (chunkSize - 1) * (chunkSize - 1) * 6 * sizeof(float));
		return mesh;
	}

	static Mesh* UpdateTerrain(Mesh* mesh, std::string const& shaderName, float* heightMap, float triagleSize, int chunkSize) {

		int pointSize = chunkSize * chunkSize * 3;

		std::shared_ptr<float[]> vertices(new float[pointSize]);
		std::shared_ptr<float[]> colors(new float[pointSize]);
		std::shared_ptr<float[]> normals(new float[pointSize]);
		std::shared_ptr<unsigned int[]> indices(new unsigned int[(chunkSize - 1) * (chunkSize - 1) * 6]);

		int index = 0;

		for (int x = 1; x < (chunkSize + 1); x++) {
			for (int z = 1; z < (chunkSize + 1); z++) {
				vertices[index] = (x - 1) * triagleSize;
				vertices[index + 1] = heightMap[x * (chunkSize + 2) + z];
				vertices[index + 2] = (z - 1) * triagleSize;

				//colors[index] = heightMap[x * (chunkSize + 2) + z];
				//colors[index + 1] = heightMap[x * (chunkSize + 2) + z];
				//colors[index + 2] = heightMap[x * (chunkSize + 2) + z];

				// Calculate the normals
				if (x > 0 && z > 0 && x < (chunkSize + 2) - 1 && z < (chunkSize + 2) - 1) {
					float hL = heightMap[(x - 1) * (chunkSize + 2) +z];
					float hR = heightMap[(x + 1) * (chunkSize + 2) + z];
					float hD = heightMap[x * (chunkSize + 2) + z - 1];
					float hU = heightMap[x * (chunkSize + 2) + z + 1];

					normals[index] = hL - hR;
					normals[index + 1] = 2.0f * triagleSize;
					normals[index + 2] = hD - hU;
				}
				else
				{
					normals[index] = 0.0f;
					normals[index + 1] = 1.0f;
					normals[index + 2] = 0.0f;
				}

				// Normalise 
				float length = sqrt(normals[index] * normals[index] + normals[index + 1] * normals[index + 1] + normals[index + 2] * normals[index + 2]);
				if (length != 0.0f) {
					normals[index] /= length;
					normals[index + 1] /= length;
					normals[index + 2] /= length;
				}

				//normals[index] = 0.0f;
				//normals[index + 1] = 1.0f;
				//normals[index + 2] = 0.0f;
				index += 3;
			}
		}

		index = 0;

		for (int x = 0; x < chunkSize - 1; x++) {
			for (int z = 0; z < chunkSize - 1; z++) {
				indices[index] = x * chunkSize + z;
				indices[index + 1] = x * chunkSize + z + 1;
				indices[index + 2] = (x + 1) * chunkSize + z;
				indices[index + 3] = x * chunkSize + z + 1;
				indices[index + 4] = (x + 1) * chunkSize + z + 1;
				indices[index + 5] = (x + 1) * chunkSize + z;
				index += 6;
			}
		}

		mesh->updateMesh(vertices, pointSize * sizeof(float), colors, pointSize * sizeof(float), normals, pointSize * sizeof(float), indices, (chunkSize - 1) * (chunkSize - 1) * 6 * sizeof(float));

		return mesh;
	}
};


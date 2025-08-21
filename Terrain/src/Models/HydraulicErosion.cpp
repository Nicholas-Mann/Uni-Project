#include "HydraulicErosion.hpp"

#include "../../../Support/src/MathsFunction.hpp"
#include "../../../Rendering/src/Objects/ComplexShape/Path.hpp"
#include "../../../Rendering/src/UI/InguiWidgits/ImguiCore.hpp"
#include "../../../Core/src/Core/Scene.hpp"
#include "../../../Terrain/src/TerrainManager.hpp"
#include "Perlin.hpp"
#include <random>
#include <math.h>
#include <chrono>

// Perlin Settings
int HydraulicErosion::addNoiseSize = 100;
float HydraulicErosion::addWeight = 60.0f;
int HydraulicErosion::addOctaves = 9;

// Hydraulic Erosion Settings
int HydraulicErosion::addRainFactor = 1;
float HydraulicErosion::addErosionFactor = 0.2;
int HydraulicErosion::addHydraulicIterations = 100;
float HydraulicErosion::addSedimentCarryFactor = 0.05f;
float HydraulicErosion::addMaxSlope = 1.0f;
int HydraulicErosion::addRainSmoothing = 3;

// Edge Smoothing Settings
bool HydraulicErosion::addEdgeSmoothingEnabled = true;
bool HydraulicErosion::addEdgeBlending = true;
int HydraulicErosion::addEdgeSmoothingSize = 10;
int HydraulicErosion::addEdgeSmoothingIterations = 1;



HydraulicErosion::HydraulicErosion(std::string name, int chunkSize, int chunkResolution, int renderDistance, int seed) : Terrain(name, chunkSize, chunkResolution, renderDistance, seed)
{
	terrainType = "HydraulicErosion";
}

HydraulicErosion::HydraulicErosion(std::string name, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, int chunkSize, int chunkResolution, int renderDistance, int seed) : Terrain(name, pos, rot, scale, chunkSize, chunkResolution, renderDistance, seed)
{
	terrainType = "HydraulicErosion";
}

void HydraulicErosion::generateHighMap(Chunk* chunk, Terrain* terrain)
{
	debugInit(chunk);

	// Start Time
	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

	// Lock the chunk
	chunk->setProcessing(true);

	float* heightMap = chunk->getHeightMap();

	for (int x = 0; x < chunk->getHeightMapSize(); x++) {
		for (int z = 0; z < chunk->getHeightMapSize(); z++) {
			heightMap[x * chunk->getHeightMapSize() + z] = 0.0f;
		}
	}

	for (int i = 0; i < octaves; i++) {
		float factor = pow(2, i);

		float* localMap = Perlin::createPerlin(chunk, noiseSize * factor, weight / (factor * 2), seed);

		for (int x = 0; x < chunk->getHeightMapSize(); x++) {
			for (int z = 0; z < chunk->getHeightMapSize(); z++) {
				heightMap[x * chunk->getHeightMapSize() + z] += localMap[x * chunk->getHeightMapSize() + z];
			}
		}

		delete[] localMap;
	}

	// Rng for the rain
	glm::vec2 chunkPos = chunk->getChunkPos();
	std::mt19937 rng(seed + chunkPos.x * 1000 + chunkPos.y * 1000);

	for (int i = 0; i < addRainFactor; i++) {
		for (int x = 1; x < chunk->getHeightMapSize() - 1; x++) {
			for (int z = 1; z < chunk->getHeightMapSize() - 1; z++) {
				// Follow the rain drop down the terrain
				glm::ivec2 rainPos = glm::ivec2(rng() % chunk->getHeightMapSize(), rng() % chunk->getHeightMapSize());
				//glm::ivec2 rainPos = glm::ivec2(x, z);

				float sediment = 0.0f;
				float water = 1.0f;

				std::vector<glm::ivec2> path;

				glm::ivec2 nextPos = glm::ivec2(-1, -1);

				for (int j = 0; j < hydraulicIterations; j++) {
					//if (lastPos == rainPos) {
					//	break;
					//}
					if (TerrainManager::getDebug())
						path.push_back(rainPos);

					// Get Height
					float heightCurrent = heightMap[rainPos.x * chunk->getHeightMapSize() + rainPos.y];

					float relativeHeight = heightCurrent;

					nextPos = getLowestNeighbour(heightMap, rainPos.x, rainPos.y, chunk->getHeightMapSize(), relativeHeight);

					if (nextPos.x < 2 || nextPos.x > chunk->getHeightMapSize() - 2 || nextPos.y < 2 || nextPos.y > chunk->getHeightMapSize() - 2)
						break;

					float nextHeight = heightMap[nextPos.x * chunk->getHeightMapSize() + nextPos.y];

					float heightDifference = heightCurrent - nextHeight;

					// Update the water
					if (water > 0.0f)
						water -= 0.01f;
					else
						water = 0.0f;

					//if (water <= 0.0f) {
					//	break;
					//}

					// Erode the terrain
					float erodeAmount = std::max(heightDifference * erosionFactor, 0.0f);

					glm::ivec2 highestNeighbour = getHighestNeighbour(heightMap, rainPos.x, rainPos.y, chunk->getHeightMapSize(), relativeHeight);

					float highest = heightMap[highestNeighbour.x * chunk->getHeightMapSize() + highestNeighbour.y];

					// Calculate the lowest hight allowed
					float minHight = highest - maxSlope;

					float maxDifference = heightMap[rainPos.x * chunk->getHeightMapSize() + rainPos.y] - minHight;

					if (heightMap[rainPos.x * chunk->getHeightMapSize() + rainPos.y] > minHight) {
						if (heightMap[rainPos.x * chunk->getHeightMapSize() + rainPos.y] - erodeAmount < minHight) {
							erodeAmount = heightMap[rainPos.x * chunk->getHeightMapSize() + rainPos.y] - minHight;
						}

						heightMap[rainPos.x * chunk->getHeightMapSize() + rainPos.y] -= erodeAmount;
						sediment += erodeAmount;
					}

					// Calculate the sediment capacity based on the velocity and water
					float sedimentCapacity = std::max(water * sedimentCarryFactor, 0.0f);
					// Check if the sediment is greater than the capacity
					if (sediment > sedimentCapacity) {
						// Deposit the sediment

						float maxSediment = (highest - heightMap[rainPos.x * chunk->getHeightMapSize() + rainPos.y]) * erodeAmount;

						float sedimentAmount = (sediment - sedimentCapacity);

						if (sedimentAmount > maxSediment) {
							sedimentAmount = maxSediment;
						}

						sediment -= sedimentAmount;
						// Update the height map
						heightMap[rainPos.x * chunk->getHeightMapSize() + rainPos.y] += sedimentAmount;
					}

					// Drop some of the sediment on the next point
					float sedimentAmount = sediment * 0.05;
					heightMap[rainPos.x * chunk->getHeightMapSize() + rainPos.y] += sedimentAmount;
					sediment -= sedimentAmount;

					rainPos = nextPos;
				}

				if (TerrainManager::getDebug() && !chunk->getShouldEnd() && targetStep - 1 <= chunk->getStepNumber()) {

					float triangleSize = (float)chunk->getSize() / ((float)chunk->getHeightMapSize() - 1);

					// Draw the path
					std::vector<glm::vec3> pathPoints;
					for (int i = 0; i < path.size(); i++) {
						pathPoints.push_back(glm::vec3(path[i].x * triangleSize + chunkPos.x * chunk->getSize(), heightMap[path[i].x * chunk->getHeightMapSize() + path[i].y], path[i].y * triangleSize + chunkPos.y * chunk->getSize()));
					}

					Mesh pathMesh = Path::GeneratePath("simple3D", pathPoints, 0.1f, glm::vec3(1.0f, 0.0f, 0.0f));
					Object* pathObject = new Object("Path", pathMesh, pathPoints[0], glm::vec3(0.0f), glm::vec3(1.0f));
					Scene::get_current_scene()->add_object(pathObject);

					debugStep(chunk, heightMap);

					Scene::get_current_scene()->remove_object(pathObject);
				}

				if (TerrainManager::getDebug())
					path.clear();

			}
		}
	}

	// Smooth the height map
	float* secondHeightMap = new float[chunk->getHeightMapSize() * chunk->getHeightMapSize()];

	for (int i = 0; i < rainSmoothing; i++) {
		for (int x = 0; x < chunk->getHeightMapSize(); x++) {
			for (int z = 0; z < chunk->getHeightMapSize(); z++) {

				// Average the height map
				float average = 0.0f;
				int count = 0;

				for (int k = -1; k < 2; k++) {
					for (int j = -1; j < 2; j++) {
						if (x + k < 0 || x + k > chunk->getHeightMapSize() - 1)
							continue;
						if (z + j < 0 || z + j > chunk->getHeightMapSize() - 1)
							continue;
						average += heightMap[(x + k) * chunk->getHeightMapSize() + (z + j)];
						count++;
					}
				}

				secondHeightMap[x * chunk->getHeightMapSize() + z] = average / count;

				//secondHeightMap[x * chunk->getHeightMapSize() + z] = heightMap[x * chunk->getHeightMapSize() + z];
			}
		}


		// Copy the second height map to the first
		for (int x = 0; x < chunk->getHeightMapSize(); x++) {
			for (int z = 0; z < chunk->getHeightMapSize(); z++) {
				heightMap[x * chunk->getHeightMapSize() + z] = secondHeightMap[x * chunk->getHeightMapSize() + z];
			}
		}
	}

	chunk->setGenerationState(1);

	if (edgeSmoothingEnabled) {
		Chunk* chunkNorth = terrain->getChunk(chunk->getChunkPos() + glm::ivec2(0, 1));
		Chunk* chunkEast = terrain->getChunk(chunk->getChunkPos() + glm::ivec2(1, 0));
		// Smooth the tettain between the chunks
		while (!(chunk->smoothedNorth && chunk->smoothedEast && chunk->smoothedSouth && chunk->smoothedWest) && !chunk->getClosing()) {
			chunkNorth = terrain->getChunk(chunk->getChunkPos() + glm::ivec2(0, 1));
			chunkEast = terrain->getChunk(chunk->getChunkPos() + glm::ivec2(1, 0));
			// Check to the north
			if (chunkNorth != nullptr) {
				if (chunkNorth->getGenerationState() >= 1 && chunk->smoothedNorth == false) {
					// Smooth the terrain between the chunks
					float* northHeightMap = chunkNorth->getHeightMap();

					float* temp = new float[((edgeSmoothingSize * 2) - 1) * chunk->getHeightMapSize()];
					float* temp2 = new float[((edgeSmoothingSize * 2) - 1) * chunk->getHeightMapSize()];

					std::fill(temp, temp + ((edgeSmoothingSize * 2) - 1) * chunk->getHeightMapSize(), 0.0f);
					std::fill(temp2, temp2 + ((edgeSmoothingSize * 2) - 1) * chunk->getHeightMapSize(), 0.0f);

					// Copy the height map to the temp array
					for (int x = 0; x < chunk->getHeightMapSize(); x++) {
						for (int i = 0; i < edgeSmoothingSize; i++) {
							temp[i * chunk->getHeightMapSize() + x] = heightMap[x * chunk->getHeightMapSize() + chunk->getHeightMapSize() - edgeSmoothingSize + i - padding];
							temp[(i + edgeSmoothingSize - 1) * chunk->getHeightMapSize() + x] = northHeightMap[x * chunk->getHeightMapSize() + i + padding];

							temp2[i * chunk->getHeightMapSize() + x] = heightMap[x * chunk->getHeightMapSize() + chunk->getHeightMapSize() - edgeSmoothingSize + i - padding];
							temp2[(i + edgeSmoothingSize - 1) * chunk->getHeightMapSize() + x] = northHeightMap[x * chunk->getHeightMapSize() + i + padding];
						}
					}

					// Smooth the terrain
					for (int o = 0; o < edgeSmoothingIterations; o++) {
						for (int x = edgeSmoothingSize; x < chunk->getHeightMapSize() - edgeSmoothingSize; x++) {
							for (int i = 0; i < (edgeSmoothingSize * 2 - 1); i++) {
								// Prevent Soothing the edges
								if (i <= edgeSmoothingIterations || i >= (edgeSmoothingSize * 2 - 2 - edgeSmoothingIterations))
									temp2[i * chunk->getHeightMapSize() + x] = temp[i * chunk->getHeightMapSize() + x];

								float average = 0.0f;
								int count = 0;
								int smoothingRange = (edgeSmoothingSize - ((i - edgeSmoothingSize) * (i - edgeSmoothingSize)) / edgeSmoothingSize) / 2.0f;
								for (int k = -smoothingRange; k <= smoothingRange; k++) {
									for (int j = -smoothingRange; j <= smoothingRange; j++) {
										if (x + k < 0 || x + k > chunk->getHeightMapSize() - 1)
											continue;
										if (i + j < 0 || i + j >(edgeSmoothingSize * 2) - 2)
											continue;
										average += temp[(i + j) * chunk->getHeightMapSize() + (x + k)];

										count++;
									}
								}
								temp2[i * chunk->getHeightMapSize() + x] = average / count;
							}
						}

						// Copy the temp2 to the temp
						for (int x = 0; x < chunk->getHeightMapSize(); x++) {
							for (int i = 0; i < (edgeSmoothingSize * 2 - 1); i++) {
								temp[i * chunk->getHeightMapSize() + x] = temp2[i * chunk->getHeightMapSize() + x];
							}
						}
					}


					if (edgeBlending) {
						// Blend the old terrain to the new smoothed terrain
						for (int x = edgeSmoothingSize; x < chunk->getHeightMapSize() - edgeSmoothingSize; x++) {
							for (int i = 0; i < edgeSmoothingSize; i++) {

								float blendFactor = (float)(edgeSmoothingSize - i) / ((float)edgeSmoothingSize);
								blendFactor = 1.0f - 1.0f / (1.0f + std::powf(2.71828f, blendFactor * 10.0f - 5.0f));
								float difference = temp[i * chunk->getHeightMapSize() + x] - heightMap[x * chunk->getHeightMapSize() + chunk->getHeightMapSize() - edgeSmoothingSize + i - padding];

								temp[i * chunk->getHeightMapSize() + x] -= difference * blendFactor;
								
								float blendFactor2 = ((float)i) / ((float)edgeSmoothingSize);
								blendFactor = 1.0f - 1.0f / (1.0f + std::powf(2.71828f, blendFactor * 10.0f - 5.0f));
								float difference2 = temp[(i + edgeSmoothingSize - 1 ) * chunk->getHeightMapSize() + x] - northHeightMap[x * chunk->getHeightMapSize() + i + padding];
								
								temp[(i + edgeSmoothingSize - 1) * chunk->getHeightMapSize() + x] -= difference2 * blendFactor2;
							}
						}
					}

					// Copy the temp to the height map making sure to also update the padding
					for (int x = 0; x < chunk->getHeightMapSize(); x++) {
						for (int i = 0; i < edgeSmoothingSize + padding; i++) {
							heightMap[x * chunk->getHeightMapSize() + chunk->getHeightMapSize() - edgeSmoothingSize + i - padding] = temp[i * chunk->getHeightMapSize() + x];
							northHeightMap[x * chunk->getHeightMapSize() + i] = temp[(i + edgeSmoothingSize - 1 - padding) * chunk->getHeightMapSize() + x];
						}
					}

					chunk->smoothedNorth = true;
					chunkNorth->smoothedSouth = true;
				}
			}
			// Check to the east
			if (chunkEast != nullptr) {
				if (chunkEast->getGenerationState() >= 1 && chunk->smoothedEast == false) {
					// Smooth the terrain between the chunks
					float* eastHeightMap = chunkEast->getHeightMap();

					float* temp = new float[((edgeSmoothingSize * 2) - 1) * chunk->getHeightMapSize()];
					float* temp2 = new float[((edgeSmoothingSize * 2) - 1) * chunk->getHeightMapSize()];

					std::fill(temp, temp + ((edgeSmoothingSize * 2) - 1) * chunk->getHeightMapSize(), 0.0f);
					std::fill(temp2, temp2 + ((edgeSmoothingSize * 2) - 1) * chunk->getHeightMapSize(), 0.0f);

					// Copy the height map to the temp array
					for (int z = 0; z < chunk->getHeightMapSize(); z++) {
						for (int i = 0; i < edgeSmoothingSize; i++) {
							temp[i * chunk->getHeightMapSize() + z] = heightMap[(chunk->getHeightMapSize() - edgeSmoothingSize + i - padding) * chunk->getHeightMapSize() + z];
							temp[(i + edgeSmoothingSize - 1) * chunk->getHeightMapSize() + z] = eastHeightMap[(i + padding) * chunk->getHeightMapSize() + z];

							temp2[i * chunk->getHeightMapSize() + z] = heightMap[(chunk->getHeightMapSize() - edgeSmoothingSize + i - padding) * chunk->getHeightMapSize() + z];
							temp2[(i + edgeSmoothingSize - 1) * chunk->getHeightMapSize() + z] = eastHeightMap[(i + padding) * chunk->getHeightMapSize() + z];
						}
					}

					// Smooth the terrain
					for (int o = 0; o < edgeSmoothingIterations; o++) {
						for (int x = edgeSmoothingSize; x < chunk->getHeightMapSize() - edgeSmoothingSize; x++) {
							for (int i = 0; i < (edgeSmoothingSize * 2 - 1); i++) {
								// Prevent Soothing the edges
								if (i <= edgeSmoothingIterations || i >= (edgeSmoothingSize * 2 - 2 - edgeSmoothingIterations))
									temp2[i * chunk->getHeightMapSize() + x] = temp[i * chunk->getHeightMapSize() + x];

								float average = 0.0f;
								int count = 0;
								int smoothingRange = (edgeSmoothingSize - ((i - edgeSmoothingSize) * (i - edgeSmoothingSize)) / edgeSmoothingSize) / 2.0f;
								for (int k = -smoothingRange; k <= smoothingRange; k++) {
									for (int j = -smoothingRange; j <= smoothingRange; j++) {
										if (x + k < 0 || x + k > chunk->getHeightMapSize() - 1)
											continue;
										if (i + j < 0 || i + j >(edgeSmoothingSize * 2) - 2)
											continue;
										average += temp[(i + j) * chunk->getHeightMapSize() + (x + k)];

										count++;
									}
								}
								temp2[i * chunk->getHeightMapSize() + x] = average / count;
							}
						}

						// Copy the temp2 to the temp
						for (int x = edgeSmoothingSize; x < chunk->getHeightMapSize() - edgeSmoothingSize; x++) {
							for (int i = 0; i < (edgeSmoothingSize * 2 - 1); i++) {
								temp[i * chunk->getHeightMapSize() + x] = temp2[i * chunk->getHeightMapSize() + x];
							}
						}
					}


					if (edgeBlending) {
						// Blend the old terrain to the new smoothed terrain
						for (int z = edgeSmoothingSize; z < chunk->getHeightMapSize() - edgeSmoothingSize; z++) {
							for (int i = 0; i < edgeSmoothingSize; i++) {

								float blendFactor = (float)(edgeSmoothingSize - i) / ((float)edgeSmoothingSize);
								blendFactor = 1.0f - 1.0f / (1.0f + std::powf(2.71828f, blendFactor * 10.0f - 5.0f));
								float difference = temp[i * chunk->getHeightMapSize() + z] - heightMap[(chunk->getHeightMapSize() - edgeSmoothingSize + i - padding) * chunk->getHeightMapSize() + z];

								temp[i * chunk->getHeightMapSize() + z] -= difference * blendFactor;

								float blendFactor2 = ((float)i) / ((float)edgeSmoothingSize);
								blendFactor = 1.0f - 1.0f / (1.0f + std::powf(2.71828f, blendFactor * 10.0f - 5.0f));
								float difference2 = temp[(i + edgeSmoothingSize - 1) * chunk->getHeightMapSize() + z] - eastHeightMap[(i + padding) * chunk->getHeightMapSize() + z];

								temp[(i + edgeSmoothingSize - 1) * chunk->getHeightMapSize() + z] -= difference2 * blendFactor2;
							}
						}
					}

					// Copy the temp to the height map making sure to also update the padding
					for (int z = 0; z < chunk->getHeightMapSize(); z++) {
						for (int i = 0; i < edgeSmoothingSize + padding; i++) {
							heightMap[(chunk->getHeightMapSize() - edgeSmoothingSize + i - padding) * chunk->getHeightMapSize() + z] = temp[i * chunk->getHeightMapSize() + z];
							eastHeightMap[(i) * chunk->getHeightMapSize() + z] = temp[(i + edgeSmoothingSize - 1 - padding) * chunk->getHeightMapSize() + z];
						}
					}

					chunk->smoothedEast = true;
					chunkEast->smoothedWest = true;
				}
			}
		}

		chunk->setGenerationState(2);

		// Smooth the NE Corner
		chunkNorth = terrain->getChunk(chunk->getChunkPos() + glm::ivec2(0, 1));
		chunkEast = terrain->getChunk(chunk->getChunkPos() + glm::ivec2(1, 0));
		Chunk* chunkNorthEast = terrain->getChunk(chunk->getChunkPos() + glm::ivec2(1, 1));
	}

	chunk->setGenerationState(3);

	chunk->setHeightMap(heightMap);

	// Set the next state of the chunk
	chunk->setChunkState(-2);

	// Unlock the chunk
	chunk->setProcessing(false);

	//delete[] secondHeightMap;
	
	// Print the time taken
	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> duration = std::chrono::duration_cast<std::chrono::duration<float>>(end - start);
	std::cout << "Time taken to generate height map: " << duration.count() << " seconds" << std::endl;

}

glm::vec2 HydraulicErosion::getHighestNeighbour(float* heightMap, int x, int z, int resolution, float height) {
	float highest = height;
	glm::vec2 highestPos = glm::vec2(x, z);
	if (x > 0 && heightMap[(x - 1) * resolution + z] > highest) {
		highest = heightMap[(x - 1) * resolution + z];
		highestPos = glm::vec2(x - 1, z);
	}
	if (x < resolution - 1 && heightMap[(x + 1) * resolution + z] > highest) {
		highest = heightMap[(x + 1) * resolution + z];
		highestPos = glm::vec2(x + 1, z);
	}
	if (z > 0 && heightMap[x * resolution + z - 1] > highest) {
		highest = heightMap[x * resolution + z - 1];
		highestPos = glm::vec2(x, z - 1);
	}
	if (z < resolution - 1 && heightMap[x * resolution + z + 1] > highest) {
		highest = heightMap[x * resolution + z + 1];
		highestPos = glm::vec2(x, z + 1);
	}
	return highestPos;
}

glm::vec2 HydraulicErosion::getLowestNeighbour(float* heightMap, int x, int z, int resolution, float height)
{
	float lowest = height;
	glm::vec2 lowestPos = glm::vec2(x, z);
	if (x > 0 && heightMap[(x - 1) * resolution + z] < lowest) {
		lowest = heightMap[(x - 1) * resolution + z];
		lowestPos = glm::vec2(x - 1, z);
	}
	if (x < resolution - 1 && heightMap[(x + 1) * resolution + z] < lowest) {
		lowest = heightMap[(x + 1) * resolution + z];
		lowestPos = glm::vec2(x + 1, z);
	}
	if (z > 0 && heightMap[x * resolution + z - 1] < lowest) {
		lowest = heightMap[x * resolution + z - 1];
		lowestPos = glm::vec2(x, z - 1);
	}
	if (z < resolution - 1 && heightMap[x * resolution + z + 1] < lowest) {
		lowest = heightMap[x * resolution + z + 1];
		lowestPos = glm::vec2(x, z + 1);
	}

	////Check Diaognals
	//if (x > 0 && z > 0 && heightMap[(x - 1) * resolution + z - 1] < lowest) {
	//	lowest = heightMap[(x - 1) * resolution + z - 1];
	//	lowestPos = glm::vec2(x - 1, z - 1);
	//}
	//if (x < resolution - 1 && z > 0 && heightMap[(x + 1) * resolution + z - 1] < lowest) {
	//	lowest = heightMap[(x + 1) * resolution + z - 1];
	//	lowestPos = glm::vec2(x + 1, z - 1);
	//}
	//if (x > 0 && z < resolution - 1 && heightMap[(x - 1) * resolution + z + 1] < lowest) {
	//	lowest = heightMap[(x - 1) * resolution + z + 1];
	//	lowestPos = glm::vec2(x - 1, z + 1);
	//}
	//if (x < resolution - 1 && z < resolution - 1 && heightMap[(x + 1) * resolution + z + 1] < lowest) {
	//	lowest = heightMap[(x + 1) * resolution + z + 1];
	//	lowestPos = glm::vec2(x + 1, z + 1);
	//}


	return lowestPos;
}

void HydraulicErosion::ImGuiWindow()
{
	ImGui::Begin("Object");

	ImGuiTerrainDefault();

	ImGui::Separator();

	// Terrain Settings
	ImGui::Text("Terrain Settings");

	// Noise Size
	ImGui::InputInt("Noise Size", &this->noiseSize);

	// Weight
	ImGui::InputFloat("Weight", &this->weight);

	// Octaves
	ImGui::InputInt("Octaves", &this->octaves);
	if (this->octaves < 1) {
		this->octaves = 1;
	}
	float factor = pow(2, this->octaves);
	ImGui::Text("Larges Map Size: %f", factor);

	ImGui::End();
}

float HydraulicErosion::dot(glm::vec2 gradient, glm::vec2 gridPos) {
	// Compute the dot-product
	return (gridPos.x * gradient.x + gridPos.y * gradient.y);
}

void HydraulicErosion::ImGuiWindowAdd() {
	ImGui::Separator();

	// Inital Perlin Settings
	ImGui::Text("Hydraulic Erosion Settings");

	ImGui::InputInt("Noise Size", &addNoiseSize);
	ImGui::InputFloat("Weight", &addWeight);
	ImGui::InputInt("Octaves", &addOctaves);
	float factor = pow(2, addOctaves);
	ImGui::Text("Larges Map Size: %f", factor);

	ImGui::Separator();

	ImGui::Text("Hydraulic Erosion Settings");
	ImGui::InputInt("Rain Factor", &addRainFactor);
	ImGui::InputFloat("Erosion Factor", &addErosionFactor);
	ImGui::InputInt("Hydraulic Iterations", &addHydraulicIterations);
	ImGui::InputFloat("Sediment Carry Factor", &addSedimentCarryFactor);
	ImGui::InputFloat("Max Slope", &addMaxSlope);
	ImGui::InputInt("Rain Smoothing", &addRainSmoothing);

	ImGui::Separator();

	ImGui::Text("Edge Smoothing Settings");
	ImGui::Checkbox("Edge Smoothing", &addEdgeSmoothingEnabled);
	ImGui::Checkbox("Edge Blending", &addEdgeBlending);
	ImGui::InputInt("Edge Smoothing Size", &addEdgeSmoothingSize);
	ImGui::InputInt("Edge Smoothing Iterations", &addEdgeSmoothingIterations);
}

#include "mesh.hpp"

#include "../../../Core/src/Core/Camera.hpp"
//#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "../Lighting/LightingManager.hpp"
#include "../../../Rendering/src/Render/Program.hpp"
#include "../../../Rendering/src/Render/RenderingHelp.hpp"
#include <mutex>

std::mutex meshMutex;

class ImguiCore {
public:
	static int triangleCount;
};

Mesh::Mesh() {

}

Mesh::Mesh(std::string const& shaderName)
{
	this->shader = ShaderProgram::getShader(shaderName);

}

Mesh::~Mesh()
{
}

void Mesh::cleanUp()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vboPos);
	glDeleteBuffers(1, &vboColour);
	glDeleteBuffers(1, &vboNormal);
}

void printBufferContents(GLuint buffer, GLenum bufferType, unsigned int size) {
	glBindBuffer(bufferType, buffer);
	std::vector<float> data(size / sizeof(float));
	glGetBufferSubData(bufferType, 0, size, data.data());
	glBindBuffer(bufferType, 0);

	std::cout << "Buffer contents: ";
	for (const auto& value : data) {
		std::cout << value << " ";
	}
	std::cout << std::endl;
}

void printVAOContents(GLuint vao) {
	glBindVertexArray(vao);

	GLint numAttribs;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &numAttribs);

	std::cout << "VAO " << vao << " contents:" << std::endl;

	for (GLint i = 0; i < numAttribs; ++i) {
		GLint enabled;
		glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
		if (enabled) {
			GLint size, type, normalized, stride, buffer;
			void* pointer;

			glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_SIZE, &size);
			glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_TYPE, &type);
			glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, &normalized);
			glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &stride);
			glGetVertexAttribPointerv(i, GL_VERTEX_ATTRIB_ARRAY_POINTER, &pointer);
			glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &buffer);

			std::cout << "Attribute " << i << ":" << std::endl;
			std::cout << "  Size: " << size << std::endl;
			std::cout << "  Type: " << type << std::endl;
			std::cout << "  Normalized: " << normalized << std::endl;
			std::cout << "  Stride: " << stride << std::endl;
			std::cout << "  Pointer: " << pointer << std::endl;
			std::cout << "  Buffer: " << buffer << std::endl;
		}
	}

	glBindVertexArray(0);
}

void checkGLError(const std::string& message) {
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cerr << "OpenGL Error (" << message << "): " << error << std::endl;
	}
}

void Mesh::updateMesh(std::shared_ptr<float[]> vertices, unsigned int verticesSize, std::shared_ptr<float[]> colour, unsigned int colourSize, std::shared_ptr<float[]> normals, unsigned int normalsSize, std::shared_ptr<unsigned int[]> indices, unsigned int indicesSize) {
	std::lock_guard<std::mutex> lock(meshMutex);

	this->vertices = vertices;
	this->colour = colour;
	this->normals = normals;
	this->indices = indices;
	this->vertexCount = verticesSize;
	this->colourCount = colourSize;
	this->normalCount = normalsSize;
	this->indexCount = indicesSize;

	if (RenderingHelp::mainThreadID != std::this_thread::get_id()) {
		// Give time for the hightmap to copy
		//std::this_thread::sleep_for(std::chrono::milliseconds(10));
		needsRenderData = true;
		return;
	}

	size = indicesSize / sizeof(unsigned int);

	glBindVertexArray(vao);

	// Element Buffer Object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices.get(), GL_STATIC_DRAW);
	checkGLError("Element Buffer Object");

	// Vertex Buffer Object
	glBindBuffer(GL_ARRAY_BUFFER, vboPos);
	glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices.get(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	checkGLError("Vertex Buffer Object");

	// Colour Buffer Object
	glBindBuffer(GL_ARRAY_BUFFER, vboColour);
	glBufferData(GL_ARRAY_BUFFER, colourSize, colour.get(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	checkGLError("Colour Buffer Object");

	// Normal Buffer Object
	glBindBuffer(GL_ARRAY_BUFFER, vboNormal);
	glBufferData(GL_ARRAY_BUFFER, normalsSize, normals.get(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	checkGLError("Normal Buffer Object");

	// Unbind buffers
	glBindVertexArray(0);
}

void Mesh::refreshMesh() {
	size = indexCount / sizeof(unsigned int);
	glBindVertexArray(vao);

	// Element Buffer Object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount, indices.get(), GL_STATIC_DRAW);
	checkGLError("Element Buffer Object");

	// Vertex Buffer Object
	glBindBuffer(GL_ARRAY_BUFFER, vboPos);
	glBufferData(GL_ARRAY_BUFFER, vertexCount, vertices.get(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	checkGLError("Vertex Buffer Object");

	// Colour Buffer Object
	glBindBuffer(GL_ARRAY_BUFFER, vboColour);
	glBufferData(GL_ARRAY_BUFFER, colourCount, colour.get(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	checkGLError("Colour Buffer Object");

	// Normal Buffer Object
	glBindBuffer(GL_ARRAY_BUFFER, vboNormal);
	glBufferData(GL_ARRAY_BUFFER, normalCount, normals.get(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	checkGLError("Normal Buffer Object");
	// Unbind buffers
	glBindVertexArray(0);
}

void Mesh::registerMesh() {
	if (RenderingHelp::mainThreadID != std::this_thread::get_id()) {
		needsRenderData = true;
		return;
	}

	if (!redistered) {
		redistered = true;
	}
	else {
		return;
	}

	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);

	// Element Buffer Object
	glGenBuffers(1, &ebo);
	checkGLError("Element Buffer Object");

	// Vertex Buffer Object
	glGenBuffers(1, &vboPos);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	checkGLError("Vertex Buffer Object");

	// Colour Buffer Object
	glGenBuffers(1, &vboColour);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	checkGLError("Colour Buffer Object");

	// Normal Buffer Object
	glGenBuffers(1, &vboNormal);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	checkGLError("Normal Buffer Object");
	
	// Unbind buffers
	glBindVertexArray(0); 
}

void Mesh::drawMesh(glm::mat4 modelTranslation, Material *material) {
	// Lock 
	std::lock_guard<std::mutex> lock(meshMutex);
	if (needsRenderData) {
		registerMesh();
		refreshMesh();
		needsRenderData = false;
	}

	glBindVertexArray(vao);

	glUseProgram(shader);

	glm::mat4 cameraProjection = Camera::get_active()->get_projection();
	glm::mat4 cameraTranslation = Camera::get_active()->get_view();
	glm::vec3 viewPos = Camera::get_active()->get_position();
	glm::mat3 normalMatrix = transpose(inverse(modelTranslation));

	material->useMaterial(shader);

	ShaderProgram::loadUniformMat4(shader, "uProjection", cameraProjection);
	ShaderProgram::loadUniformMat4(shader, "uCameraView", cameraTranslation);
	ShaderProgram::loadUniformMat4(shader, "uModel", modelTranslation);
	ShaderProgram::loadUniformMat3(shader, "uNormalMatrix", normalMatrix);
	ShaderProgram::loadUniformVec3(shader, "uViewPos", viewPos);

	LightingManager::RenderLighting(shader);

	glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);
	checkGLError("glDrawElements");

	ImguiCore::triangleCount += size / 3;

	glUseProgram(0);

	glBindVertexArray(0);
}
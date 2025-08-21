#include "TextHandler.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

TextHandler::TextHandler(std::string atlasFile, std::string fontFile)
{
	this->atlasWidth = 0;
	this->atlasHeight = 0;
	this->lineHeight = 0;

	// Open the atlas file
	//textureID = load_texture_2d(atlasFile.c_str());
    textureID = NULL;

    std::ifstream file(fontFile);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << fontFile << "\n";
    }

    std::unordered_map<char, Letter> letterMap;
    std::string line;

    while (std::getline(file, line)) {
        if (line.starts_with("common")) {
            std::vector<std::string> parts = split(line, " ");
            try {
                parts[1].erase(0, 11); // Remove "lineHeight="
                lineHeight = std::stoi(parts[1]);

                parts[3].erase(0, 7); // Remove "scaleW="
                atlasWidth = std::stoi(parts[3]);

                parts[4].erase(0, 7); // Remove "scaleH="
                atlasHeight = std::stoi(parts[4]);
            }
            catch (const std::exception& e) {
                std::cerr << "Error parsing line: " << line << " - " << e.what() << "\n";
            }
        }

        if (line.starts_with("char ")) {
            std::vector<std::string> parts = split(line, " ");

            if (parts.size() < 9) {
                std::cerr << "Malformed line: " << line << "\n";
                continue;
            }

            try {
                Letter letter;

                parts[1].erase(0, 3); // Remove "id="
                letter.letter = static_cast<char>(std::stoi(parts[1]));

                parts[2].erase(0, 2); // Remove "x="
                letter.xPos = std::stoi(parts[2]);

                parts[3].erase(0, 2); // Remove "y="
                letter.yPos = std::stoi(parts[3]);

                parts[4].erase(0, 6); // Remove "width="
                letter.width = std::stoi(parts[4]);

                parts[5].erase(0, 7); // Remove "height="
                letter.height = std::stoi(parts[5]);

                parts[6].erase(0, 8); // Remove "xoffset="
                letter.xOffset = std::stoi(parts[6]);

                parts[7].erase(0, 8); // Remove "yoffset="
                letter.yOffset = std::stoi(parts[7]);

                parts[8].erase(0, 9); // Remove "xadvance="
                letter.xAdvance = std::stoi(parts[8]);

                // Add the letter to the map, using `letter.letter` as the key
                letterMap[letter.letter] = letter;
            }
            catch (const std::exception& e) {
                std::cerr << "Error parsing line: " << line << " - " << e.what() << "\n";
            }
        }
    }

	letters = letterMap;
}

TextHandler::~TextHandler()
{
	glDeleteTextures(1, &textureID);
}

Word TextHandler::generateText(std::string text, float scale, glm::vec4 colour)
{
	std::vector<glm::vec3> positions;
	std::vector<glm::vec4> colours;
	std::vector<glm::vec2> texCoords;

	int wordWidth = 0;
	int wordHeightLow = 0;
	int wordHeightHigh = 0;

	float x = 0;
	float y = 0;
    for (int i = 0; i < (int)text.size(); i++) {
		if (text[i] == '\n') {
			x = 0;
			y += lineHeight;
			continue;
		}

		Letter letter = letters[text[i]];
		float x0 = x + letter.xOffset * scale;
		float y0 = y + letter.yOffset * scale;
		float x1 = x0 + letter.width * scale;
		float y1 = y0 + letter.height * scale;
		float s0 = letter.xPos / (float)atlasWidth;
		float t0 = ((float)atlasHeight - letter.yPos) / (float)atlasHeight;
		float s1 = (letter.xPos + letter.width) / (float)atlasWidth;
        float t1 = ((float)atlasHeight - (letter.yPos + letter.height)) / (float)atlasHeight;

		if (y1 > wordHeightHigh) {
			wordHeightHigh = (int)y1;
		}
		if (y0 < wordHeightLow) {
			wordHeightLow = (int)y0;
		}

		positions.push_back(glm::vec3{ x0, y0, 0.f });
		positions.push_back(glm::vec3{ x1, y0, 0.f });
		positions.push_back(glm::vec3{ x1, y1, 0.f });
		positions.push_back(glm::vec3{ x0, y0, 0.f });
		positions.push_back(glm::vec3{ x1, y1, 0.f });
		positions.push_back(glm::vec3{ x0, y1, 0.f });
        colours.push_back(colour);
        colours.push_back(colour);
        colours.push_back(colour);
        colours.push_back(colour);
        colours.push_back(colour);
        colours.push_back(colour);
		texCoords.push_back(glm::vec2{ s0, t0 });
		texCoords.push_back(glm::vec2{ s1, t0 });
		texCoords.push_back(glm::vec2{ s1, t1 });
		texCoords.push_back(glm::vec2{ s0, t0 });
		texCoords.push_back(glm::vec2{ s1, t1 });
		texCoords.push_back(glm::vec2{ s0, t1 });
		x += (float)letter.xAdvance * scale;
		if (x > wordWidth) {
			wordWidth = (float)x;
		}
    }


	// This creates a VAO and VBO for the text
	GLuint VAO, posVBO, texVBO, colourVBO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

    glGenBuffers(1, &posVBO);
    glBindBuffer(GL_ARRAY_BUFFER, posVBO);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &texVBO);
    glBindBuffer(GL_ARRAY_BUFFER, texVBO);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &colourVBO);
    glBindBuffer(GL_ARRAY_BUFFER, colourVBO);
    glBufferData(GL_ARRAY_BUFFER, colours.size() * sizeof(glm::vec4), colours.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    // unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

	Word word;
	word.word = text;
	word.size = (int)positions.size();
	word.text = VAO;
	word.width = wordWidth;
	word.height = wordHeightHigh - wordHeightLow;

    return word;
}

void TextHandler::renderText(Word word, float x, float y, float windowWidth, float windowHeight, GLuint prog)
{
    glm::mat4 projection = {
        2.f / windowWidth, 0.f, 0.f, -1.f,
        0.f, -2.0f / windowHeight, 0.f, 1.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f };

	glm::mat4 translation = {
		1.f, 0.f, 0.f, x,
		0.f, 1.f, 0.f, y,
		0.f, 0.f, 1.f, 0.f, 
        0.f, 0.f, 0.f, 1.f };

	glm::mat4 mvp = projection * translation;

	glUseProgram(prog);

	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glUniformMatrix4fv(glGetUniformLocation(prog, "uProjCameraWorld"), 1, GL_TRUE, glm::value_ptr(mvp));

	//this->m_material.useMaterial(prog);

	glBindVertexArray(word.text);
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)word.size);

	glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glBindVertexArray(0);
    glUseProgram(0);
}
#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>


struct Word {
    std::string word = "";
    int size = 0;
    GLuint text = 0;
	int width = 0;
	int height = 0;
};

class TextHandler
{
public:
	TextHandler(std::string atlisFile, std::string fontFile);
    Word generateText(std::string text, float scale, glm::vec4 colour);
	void renderText(Word word, float x, float y, float windowWidth, float windowHeight, GLuint prog);
	~TextHandler();

private:
    struct Letter
    {
        char letter = -1;
        int xPos = 0;
        int yPos = 0;
        int width = 0;
        int height = 0;
        int xOffset = 0;
        int yOffset = 0;
        int xAdvance = 0;
    };

	GLuint textureID;
    std::unordered_map<char, Letter> letters;
	int atlasWidth;
	int atlasHeight;
	int lineHeight;

    std::vector<std::string> split(std::string& s, const std::string& delimiter) {
        std::vector<std::string> tokens;
        size_t pos = 0;
        std::string token;
        while ((pos = s.find(delimiter)) != std::string::npos) {
            token = s.substr(0, pos);
			if (token != "" && token != "\n")
                tokens.push_back(token);
            s = s.substr(pos + delimiter.length(), s.length());
        }
        if (token != "" && token != "\n")
        tokens.push_back(s);

        return tokens;
    }

    bool isBlank(const std::string& str) {
        return str.empty() || str == "";
    }


};


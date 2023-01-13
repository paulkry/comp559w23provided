#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLSL.h"
#include "Program.h"

#include <ft2build.h>
#include FT_FREETYPE_H

using namespace std;

shared_ptr<Program> prog;

struct Character {
	unsigned int TextureID;  // ID handle of the glyph texture
	glm::ivec2   Size;       // Size of glyph
	glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
	unsigned int Advance;    // Offset to advance to next glyph
};

map<char, Character> Characters;

void initTextRender( string RES_DIR ) {
	
	prog = make_shared<Program>();
	prog->setVerbose(true);
	prog->setShaderNames(RES_DIR + "text_vert.glsl", RES_DIR + "text_frag.glsl");
	prog->init();
	prog->addUniform("text"); // texture unit
	prog->addUniform("P");
	prog->addUniform("MV");	
	prog->setVerbose(false);

	GLSL::checkError(GET_FILE_LINE);

	// Init font stuff here 
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return;// -1;
	}
	FT_Face face;
	//if (FT_New_Face(ft, "C:/Windows/Fonts/Arial.ttf", 0, &face)) {
	// HOW are you supposed to know the name of these font files !?
	if (FT_New_Face(ft, "C:/Windows/Fonts/consola.ttf", 0, &face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return;// -1;
	}
	FT_Set_Pixel_Sizes(face, 0, 48);
	if (FT_Load_Char(face, 'X', FT_LOAD_RENDER)) {
		std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
		return; // -1;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

	for (unsigned char c = 0; c < 128; c++) {
		// load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// generate texture
		unsigned int texture;
		glActiveTexture(GL_TEXTURE0); // just in case we need to do this for a particular texture unit
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);

		// set texture options (are these for the texture or for the texture unit??
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<char, Character>(c, character));
	}
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void RenderString( glm::mat4& P, glm::mat4& MV, float x, float y, float scale, string& text ) {
	// activate corresponding render state	

	prog->bind();
	glColor4f(1, 1, 1, 1);
	glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
	glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, &MV[0][0]);
	glUniform1i(prog->getUniform("text"), 0);

	glActiveTexture(GL_TEXTURE0);

	float xStart = x;

	for (char c : text) {
		if (c == '\n') {
			x = xStart;
			y += 48 * scale;
		} else {
			Character ch = Characters[c];
			float xpos = x + ch.Bearing.x * scale;
			float ypos =  y + (ch.Size.y - ch.Bearing.y) * scale;

			// You can get a(massive) performance boost if you buffer the vertices only once, and use matrices to scaleand translate a glyph's quad.
			// Probably also a massive performance boost by not switching textures on the texture unit constantly.  They should all be dumped arbitrarily into a signel texture
			float w = ch.Size.x * scale;
			float h = -ch.Size.y * scale;
			
			// render glyph texture over quad
			glBindTexture(GL_TEXTURE_2D, ch.TextureID);

			// xy coord in xy and text coords in zw
			glBegin(GL_TRIANGLES);
			glVertex4f(xpos, ypos + h, 0.0f, 0.0f);
			glVertex4f(xpos, ypos, 0.0f, 1.0f);
			glVertex4f(xpos + w, ypos, 1.0f, 1.0f);
			glVertex4f(xpos, ypos + h, 0.0f, 0.0f);
			glVertex4f(xpos + w, ypos, 1.0f, 1.0f);
			glVertex4f(xpos + w, ypos + h, 1.0f, 0.0f);
			glEnd();
			
			// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
		}
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	prog->unbind();
	GLSL::checkError(GET_FILE_LINE);
}
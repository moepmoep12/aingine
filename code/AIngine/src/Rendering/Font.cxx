#include "Rendering/Font.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "Debug/log.h"

#include <string>

namespace AIngine::Rendering {
	Font::Font(const char * path, unsigned int size)
		: Size(size)
	{
		ExtractName(path);
		FT_Library ft;
		bool initSucess = !FT_Init_FreeType(&ft);
		assert(initSucess);
		FT_Face face;
		bool faceSucess = !FT_New_Face(ft, path, 0, &face);
		assert(faceSucess);
		// Set size to load glyphs as
		FT_Set_Pixel_Sizes(face, 0, size);
		// Disable byte-alignment restriction
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// Load first 128 characters of ASCII set
		for (GLubyte c = 0; c < 128; c++)
		{
			// Load character glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				CORE_ERROR("ERROR::FREETYTPE: Failed to load Glyph {0}", c);
				continue;
			}
			// Generate texture
			GLuint texture;
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
			// Set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// Now store character for later use
			RenderableCharacter character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			Characters.insert(std::pair<GLchar, RenderableCharacter>(c, character));
		}
		glBindTexture(GL_TEXTURE_2D, 0);
		// Destroy FreeType once we're finished
		FT_Done_Face(face);
		FT_Done_FreeType(ft);
	}

	Font::~Font()
	{
		Characters.clear();
	}

	void Font::ExtractName(const char * path)
	{
		std::string Path(path);
		unsigned int first = Path.find_last_of('/');
		unsigned int last = Path.find_last_of('.');
		Name = Path.substr(first, last - first).c_str();
	}
}
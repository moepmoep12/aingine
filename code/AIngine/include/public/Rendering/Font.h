#pragma once

#include <glad/glad.h>
#include <map>
#include <glm/glm.hpp>

namespace AIngine::Rendering {

	// inspired/copied from https://learnopengl.com/code_viewer.php?code=in-practice/text_rendering
	/// Holds all state information relevant to a character as loaded using FreeType
	struct RenderableCharacter {
		unsigned int TextureID;
		glm::ivec2 Size;
		glm::ivec2 Bearing;
		unsigned int Advance;
	};

	class Font final {
	public:
		explicit Font(const char* path, unsigned int size);
		~Font();

	public:
		std::map<GLchar, RenderableCharacter> Characters;
		const char* Name;
		unsigned int Size;


	private:
		void ExtractName(const char* path);
	};
}
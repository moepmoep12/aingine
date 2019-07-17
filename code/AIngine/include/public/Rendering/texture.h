#pragma once

#include "Rendering/bitmap.h"

#include <glad/glad.h>
#include <string>

namespace AIngine::Rendering {

	class Texture2D final {
	public:
		// Holds the ID of the texture object, used for all texture operations to reference to this particlar texture
		GLuint ID = -1;

		// Texture image dimensions
		GLuint Width, Height; // Width and height of loaded image in pixels

		// Texture Format
		GLuint Internal_Format; // Format of texture object
		GLuint Image_Format; // Format of loaded image

		// Texture configuration
		GLuint Wrap_S; // Wrapping mode on S axis
		GLuint Wrap_T; // Wrapping mode on T axis
		GLuint Filter_Min; // Filtering mode if texture pixels < screen pixels
		GLuint Filter_Max; // Filtering mode if texture pixels > screen pixels

		// Constructor (sets default texture modes)
		Texture2D();
		// Creates the texture on the graphics card
		Texture2D(Bitmap &texture);

		// copy constructor
		Texture2D(const Texture2D& other);

		// copy assignment
		Texture2D& operator=(const Texture2D& other);

		// move constructor
		Texture2D(Texture2D&& other);

		// move assignment
		Texture2D& operator=(Texture2D&& other);

		virtual ~Texture2D();

		// Generates texture from a bitmap
		void Generate(Bitmap &texture);

		// Binds the texture as the current active GL_TEXTURE_2D texture object
		void Bind() const;

		std::string FileName;

		bool operator==(const Texture2D& other) const;
	};
}

namespace std {

	template <>
	struct hash<AIngine::Rendering::Texture2D>
	{
		std::size_t operator()(const AIngine::Rendering::Texture2D& k) const
		{
			using std::size_t;
			using std::hash;
			using std::string;

			// Compute individual hash values for first, second and third
			// http://stackoverflow.com/a/1646913/126995
			size_t res = 17;
			res = res * 31 + hash<int>()(k.Width);
			res = res * 31 + hash<int>()(k.Height);
			res = res * 31 + hash<int>()(k.Image_Format);
			res = res * 31 + hash<string>()(k.FileName);
			return res;
		}
	};

}


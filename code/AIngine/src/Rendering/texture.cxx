#include "Rendering/texture.h"
#include "AIngine/GameObject.h"
#include "Assets/Assets.h"

namespace AIngine::Rendering {

	Texture2D::Texture2D()
		: Width(0), Height(0), Internal_Format(GL_RGB), Image_Format(GL_RGB), Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT), Filter_Min(GL_LINEAR), Filter_Max(GL_LINEAR)
	{
		glGenTextures(1, &this->ID);
	}

	Texture2D::Texture2D(Bitmap & texture)
		: Width(0), Height(0), Internal_Format(GL_RGB), Image_Format(GL_RGB), Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT), Filter_Min(GL_LINEAR), Filter_Max(GL_LINEAR)
	{
		glGenTextures(1, &this->ID);
		Generate(texture);
	}

	// move constructor
	Texture2D::Texture2D(Texture2D && other)
		: Width(other.Width), Height(other.Height), Internal_Format(other.Internal_Format), Image_Format(other.Image_Format), Wrap_S(other.Wrap_S), Wrap_T(other.Wrap_T), Filter_Min(other.Filter_Min), Filter_Max(other.Filter_Max), FileName(other.FileName)
	{
		// take ownership of GPU resources
		ID = other.ID;
		other.ID = -1;
	}

	// move assignment
	Texture2D & Texture2D::operator=(Texture2D && other)
	{
		if (&other != this) {

			// copy attributes
			Width = other.Width;
			Height = other.Height;
			Internal_Format = other.Internal_Format;
			Image_Format = other.Image_Format;
			Wrap_S = other.Wrap_S;
			Wrap_T = other.Wrap_T;
			Filter_Min = other.Filter_Min;
			Filter_Max = other.Filter_Max;
			FileName = other.FileName;

			// take ownership of GPU resources
			glDeleteTextures(1, &this->ID);
			ID = other.ID;
			other.ID = -1;
		}
		return *this;
	}

	Texture2D::~Texture2D()
	{
		glDeleteTextures(1, &this->ID);
	}

	// copy constructor
	Texture2D::Texture2D(const Texture2D & other)
		: Width(other.Width), Height(other.Height), Internal_Format(other.Internal_Format), Image_Format(other.Image_Format), Wrap_S(other.Wrap_S), Wrap_T(other.Wrap_T), Filter_Min(other.Filter_Min), Filter_Max(other.Filter_Max), FileName(other.FileName)
	{
		// first we copy all attributes (but ID)
		// if the other texture was initialized with a bitmap we initialize as well
		glGenTextures(1, &this->ID);

		// copy the bitmap
		if (!other.FileName.empty()) {
			AIngine::Assets::BitmapAsset* bitmapasset = AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>(other.FileName);
			Generate(bitmapasset->GetBitmap());
		}
	}

	// copy assignment
	Texture2D & Texture2D::operator=(const Texture2D & other)
	{
		if (&other != this) {

			// copy attributes
			Width = other.Width;
			Height = other.Height;
			Internal_Format = other.Internal_Format;
			Image_Format = other.Image_Format;
			Wrap_S = other.Wrap_S;
			Wrap_T = other.Wrap_T;
			Filter_Min = other.Filter_Min;
			Filter_Max = other.Filter_Max;
			FileName = other.FileName;

			// copy the bitmap
			if (!other.FileName.empty()) {
				AIngine::Assets::BitmapAsset* bitmapasset = AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>(other.FileName);
				Generate(bitmapasset->GetBitmap());
			}

			// the other doesn't have a bitmap, so we delete our too
			else {
				// free data
				glDeleteTextures(1, &this->ID);
				ID = -1;
			}
		}
		return *this;
	}

	void Texture2D::Generate(Bitmap &texture)
	{
		this->FileName = texture.GetName();
		this->Width = texture.GetWidth();
		this->Height = texture.GetHeight();
		this->Image_Format = (texture.GetChannels() == 3) ? GL_RGB : GL_RGBA;
		this->Internal_Format = (texture.GetChannels() == 3) ? GL_RGB : GL_RGBA;
		// Create Texture
		glBindTexture(GL_TEXTURE_2D, this->ID);
		glTexImage2D(GL_TEXTURE_2D,
			0,
			this->Internal_Format,
			Width,
			Height,
			0,
			this->Image_Format,
			GL_UNSIGNED_BYTE,
			texture.GetData());

		// Set Texture wrap and filter modes
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);
		// Unbind texture
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture2D::Bind() const
	{
		glBindTexture(GL_TEXTURE_2D, this->ID);
	}
}
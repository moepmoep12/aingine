#include "Rendering/bitmap.h"

// See stb_image.h
#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

Bitmap::Bitmap(std::string const &filename) {
    m_data = stbi_load(filename.c_str(), &m_width, &m_height, &m_channels, 0);
    if (m_data == nullptr)
        throw std::runtime_error(std::string("Could not load file: ") + filename);
}

int Bitmap::GetWidth() const {
    return m_width;
}

int Bitmap::GetHeight() const {
    return m_height;
}

int Bitmap::GetChannels() const {
    return m_channels;
}

unsigned char *Bitmap::GetData() const {
    return m_data;
}

Bitmap::~Bitmap() {
    stbi_image_free(m_data);
   // printf("Delete Bitmap");
}

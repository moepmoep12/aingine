#include "bitmap.h"

// See stb_image.h
#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

Bitmap::Bitmap(std::string const &filename) {
    data = stbi_load(filename.c_str(), &width, &height, &channels, 0);
    if (data == nullptr)
        throw std::runtime_error(std::string("Could not load file: ") + filename);
}

int Bitmap::GetWidth() const {
    return width;
}

int Bitmap::GetHeight() const {
    return height;
}

int Bitmap::GetChannels() const {
    return channels;
}

unsigned char *Bitmap::GetData() const {
    return data;
}

Bitmap::~Bitmap() {
    stbi_image_free(data);
    printf("Delete Bitmap");
}

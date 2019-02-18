#pragma once

#include <string>

class Bitmap {
public:
    Bitmap(std::string const &filename);

    ~Bitmap();

    int GetWidth() const;

    int GetHeight() const;

    int GetChannels() const;

    unsigned char *GetData() const;

private:
    int width, height, channels;
    unsigned char *data;
};


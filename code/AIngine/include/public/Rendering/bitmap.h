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

	inline const std::string& GetName() const { return m_filename; }

private:
    int m_width, m_height, m_channels;
    unsigned char* m_data;
	std::string m_filename;
};


#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <stdexcept>

static void ReadFile(std::string const &filepath, std::string &content) {
    try {
        std::ifstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        file.open(filepath);
        std::stringstream fStream;
        fStream << file.rdbuf();
        content = fStream.str();
        file.close();
    }
    catch (const std::exception &e) {
        throw std::runtime_error("Failed to load read file: " + filepath + " (" + e.what() + ")");
    }
}

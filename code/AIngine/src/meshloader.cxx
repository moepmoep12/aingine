#include "meshloader.h"

#include <iostream>
#include <tiny_obj_loader.h>

void LoadOBJ(std::string const &assetPath, AIngine::Renderer::Mesh * mesh) {
    // Load Mesh Data
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;
	std::string warning;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warning, &err, assetPath.c_str());

    // Fatal Error
    if (!ret)
        throw std::runtime_error(std::string("Failed to load file: ") + assetPath + err);

    // Minor Error
    if (!err.empty()) {
        std::cout << err << std::endl;
    }

    // Convert Mesh Data
    std::vector<float> positions, texcoords;
    std::vector<unsigned int> indices;

    {
        tinyobj::mesh_t &mesh = shapes[0].mesh;
        for (size_t v = 0; v < mesh.indices.size(); v++) {
            // Note: This throws away any vertex reuse.
            // See http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-9-vbo-indexing/ for more information.
            indices.push_back((unsigned int) v);

            int vIndex = mesh.indices[v].vertex_index;
            positions.push_back(attrib.vertices[3 * vIndex + 0]);
            positions.push_back(attrib.vertices[3 * vIndex + 1]);
            positions.push_back(attrib.vertices[3 * vIndex + 2]);

            int uvIndex = mesh.indices[v].texcoord_index;
            texcoords.push_back(attrib.texcoords[2 * uvIndex + 0]);
            texcoords.push_back(1.0f - attrib.texcoords[2 * uvIndex + 1]);
        }
    }

    mesh->SetPositions(positions.data(), positions.size());
    mesh->SetTexCoordinates(texcoords.data(), texcoords.size());
    mesh->SetIndices(indices.data(), indices.size());
}


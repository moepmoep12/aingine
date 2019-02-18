#include "mesh.h"

namespace AIngine::Renderer {

	Mesh::Mesh() {
	}

	Mesh::~Mesh() {
		if (positionBuffer != GL_NONE)
			glDeleteBuffers(1, &positionBuffer);

		if (texCoordBuffer != GL_NONE)
			glDeleteBuffers(1, &texCoordBuffer);

		if (indexBuffer != GL_NONE)
			glDeleteBuffers(1, &indexBuffer);
	}

	void Mesh::Bind(GLShaderProgram *sp) {
		positionAttribute = glGetAttribLocation(sp->GetID(), "position");
		texCoordAttribute = glGetAttribLocation(sp->GetID(), "texCoord");

		// in position
		if (positionBuffer != GL_NONE && positionAttribute != -1) {
			glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
			glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(positionAttribute);
		}

		// in position
		if (texCoordBuffer != GL_NONE && texCoordAttribute != -1) {
			glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
			glVertexAttribPointer(texCoordAttribute, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(texCoordAttribute);
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	}

	void Mesh::Draw() {
		glDrawElements(GL_TRIANGLES, NumIndices, GL_UNSIGNED_INT, 0);
	}

	void Mesh::SetPositions(float *positions, size_t count) {
		if (positionBuffer == GL_NONE)
			glGenBuffers(1, &positionBuffer);

		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * count, positions, GL_STATIC_DRAW);
	}

	void Mesh::SetTexCoordinates(float *uvs, size_t count) {
		if (texCoordBuffer == GL_NONE)
			glGenBuffers(1, &texCoordBuffer);

		glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * count, uvs, GL_STATIC_DRAW);
	}

	void Mesh::SetIndices(unsigned int *indices, size_t count) {
		if (indexBuffer == GL_NONE)
			glGenBuffers(1, &indexBuffer);

		NumIndices = (GLsizei) count;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * count, indices, GL_STATIC_DRAW);
	};

}
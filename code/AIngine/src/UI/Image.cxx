#include "UI/Image.h"
#include "Rendering/shader.h"

namespace AIngine::UI {

	void Image::Render(const glm::mat4 & modelMatrix, AIngine::Rendering::GLShaderProgram & shader) const
	{
		Texture.Bind();
		glm::vec4 color = TintColor;
		if (IsDisabled())
			color = DisabledColor;

		shader.SetVector4f(5 /*spriteColor*/, color);
	}
}
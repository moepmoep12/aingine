#pragma once

#include "AIngine/Component.h"
#include "Rendering/shader.h"
#include "Rendering/texture.h"
#include "Events/Event.h"

#include <vector>

namespace AIngine {

	namespace Editor {
		class ParticleEmitterWidget;
	}

	struct Particle {
		// Rotation in radians
		float Lifetime, Rotation, AngularVelocity;
		// Position local to Owner
		glm::vec2 Position, Velocity;
		// Size in WorldSpace
		glm::vec2 Size;
		glm::vec4 Color;

		Particle()
			: Lifetime(0), Position(0), Color(0), Velocity(0), Rotation(0), AngularVelocity(0), Size(0) {}
	};

	class ParticleEmitter : public Component {
	public:
		ParticleEmitter();
		virtual ~ParticleEmitter();

		inline  AIngine::Rendering::Texture2D&  GetTexture() { return m_texture; }
		inline void SetTexture(const AIngine::Rendering::Texture2D& texture) { m_texture = texture; SetName(texture.FileName); }

		inline const std::vector<Particle>& GetParticles() const { return m_particles; }

		inline int GetAmount() const { return m_amount; }
		void SetAmount(int value);

		inline glm::vec2 GetLocalSpawnPosition() const { return m_localSpawnPosition; }
		inline void SetLocalSpawnPosition(const glm::vec2& pos) { m_localSpawnPosition = pos; }

		virtual void OnUpdate(float deltatime) override;

		void Update(float delta, int particlesToSpawn);

		AIngine::Events::Event<void, Particle*, int, const glm::vec2&> SpawnParticlesEvent;
		AIngine::Events::Event<void, Particle&, float> UpdateParticleEvent;

		typedef AIngine::Events::EventHandler<void, Particle*, int, const glm::vec2&> SpawnParticlesHandler;
		typedef AIngine::Events::EventHandler<void, Particle&, float>  UpdateParticleHandler;

		GLuint BlendFunc = GL_ONE;

		GLuint m_vao;
		GLuint m_offsetVBO;
		GLuint m_sizeVBO;
		GLuint m_rotationVBO;
		GLuint m_ColorVBO;

		enum { maxParticles = 10000 };

	protected:
		int GetAvailableParticle() const;

		virtual Component* Copy(GameObject* const owner) const override;

	private:
		friend class AIngine::Editor::ParticleEmitterWidget;

		bool m_startCalled = false;
		AIngine::Rendering::Texture2D m_texture;
		std::vector<Particle> m_particles;
		glm::vec2 m_localSpawnPosition = glm::vec2(0);
		int m_amount = 0;
	};
}
#pragma once

#include "Particles/ParticleSystem.h"
#include "Particles/ParticleHelpers.h"

class FluidTile;

namespace particles
{
	class FluidUpdater : public ParticleUpdater {
	public:
		FluidUpdater() {}
		~FluidUpdater() {}

		void update(ParticleData* data, float dt);

	public:
		FluidTile* fluidTile = nullptr;
	};

	class FadingColorUpdater final : public ColorUpdater {
	public:
		FadingColorUpdater() {}

		void update(ParticleData* data, float dt) override;

		// start the fading timer (in seconds)
		void setFading(float timeToFade);
		void resetColor();

	private:
		float m_timeToFade;
		float m_initialTimeToFade;
		bool m_isFading = false;
	};


	class AttractingEulerUpdater : public EulerUpdater {
	public:
		AttractingEulerUpdater(const sf::Vector2f* refPos, float fraction = 1.f);
		~AttractingEulerUpdater() {}

		void update(ParticleData *data, float dt) override;

	public:
		const sf::Vector2f* m_refPos;
		sf::Vector2f m_oldPos;
		float m_fraction;
	};

}

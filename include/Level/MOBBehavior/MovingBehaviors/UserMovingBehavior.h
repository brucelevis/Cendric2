#pragma once

#include "global.h"
#include "InputController.h"
#include "Level/MOBBehavior/MovingBehavior.h"

// A moving behavior for the level main character
class UserMovingBehavior : public MovingBehavior {
public:
	UserMovingBehavior(LevelMainCharacter* mainChar);
	~UserMovingBehavior() {};

	void update(const sf::Time& frameTime) override;

	void checkCollisions(const sf::Vector2f& nextPosition) override;
	void handleMovementInput() override;
	void updateAnimation() override;

private:
	// makes it easier to jump
	static const sf::Time JUMP_GRACE_TIME;
	sf::Time m_jumpGraceTime = sf::Time::Zero;
	sf::Time m_jumpPressTime = sf::Time::Zero;
	sf::Time m_frameTime = sf::Time::Zero;

	float m_startHeight;
	float m_jumpHeightTest;
	sf::Time m_jumpTimeTest;
};
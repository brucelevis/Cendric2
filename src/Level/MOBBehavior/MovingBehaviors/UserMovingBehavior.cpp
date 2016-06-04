#include "Level/MOBBehavior/MovingBehaviors/UserMovingBehavior.h"
#include "Level/Level.h"
#include "Level/LevelMainCharacter.h"

const sf::Time UserMovingBehavior::JUMP_GRACE_TIME = sf::milliseconds(100);

UserMovingBehavior::UserMovingBehavior(LevelMainCharacter* mainChar) : MovingBehavior(mainChar) {
	// use this assignment because the "normal" assigner in moving behavior can't get this yet.
	m_mainChar = mainChar;
}

void UserMovingBehavior::update(const sf::Time& frameTime) {
	updateTime(m_jumpGraceTime, frameTime);
	bool wasGrounded = m_isGrounded;
	MovingBehavior::update(frameTime);
	if (wasGrounded && !m_isGrounded) {
		m_jumpGraceTime = JUMP_GRACE_TIME;
	}

	// TEST STUFF - Remove later
	if (m_isGrounded && !wasGrounded) {
		// std::cout << "h: " <<  m_jumpHeightTest - m_startHeight << ", t: " << m_jumpTimeTest.asSeconds() << std::endl;
	}
	else {
		m_jumpTimeTest += frameTime;
		if (m_mainChar->getPosition().y < m_jumpHeightTest) m_jumpHeightTest = m_mainChar->getPosition().y;
	}

	m_frameTime = frameTime;
}

void UserMovingBehavior::checkCollisions(const sf::Vector2f& nextPosition) {
	bool collidesX;
	bool collidesY;
	MovingBehavior::checkXYDirection(nextPosition, collidesX, collidesY);
}

void UserMovingBehavior::handleMovementInput() {
	if (m_mainChar->isDead()) return;
	float newAccelerationX = m_mainChar->getAcceleration().x;
	if (!m_mainChar->isStunned() &&
		!m_mainChar->isFeared()) {

		if (g_inputController->isKeyActive(Key::Left)) {
			m_nextIsFacingRight = false;
			newAccelerationX -= m_walkAcceleration;
		}
		if (g_inputController->isKeyActive(Key::Right)) {
			m_nextIsFacingRight = true;
			newAccelerationX += m_walkAcceleration;
		}

		float velY = m_isFlippedGravity ? m_configuredMaxVelocityYUp : -m_configuredMaxVelocityYUp;
		if (g_inputController->isKeyJustPressed(Key::Jump) && (m_isGrounded || m_jumpGraceTime > sf::Time::Zero)) {
			m_jumpGraceTime = sf::Time::Zero;
			m_jumpPressTime = sf::Time::Zero;
			m_mainChar->setVelocityY(velY); // first jump vel will always be max y vel. 
			
			// TEST STUFF - Remove later
			m_startHeight = m_mainChar->getPosition().y;
			m_jumpHeightTest = m_startHeight;
			m_jumpTimeTest = sf::Time::Zero;
		}
		else if (g_inputController->isKeyActive(Key::Jump)) {
			m_jumpPressTime += m_frameTime;
			if (m_jumpPressTime < sf::seconds(0.3f)) {
				m_mainChar->setVelocityY(velY);
			}
		}
		else {
			m_jumpPressTime = sf::seconds(100.f);
		}
	}

	m_mainChar->setAcceleration(sf::Vector2f(newAccelerationX, (m_isFlippedGravity ? -m_gravity : m_gravity)));
}

void UserMovingBehavior::updateAnimation() {
	// calculate new game state and set animation.

	GameObjectState newState = GameObjectState::Idle;
	if (m_mainChar->isDead()) {
		newState = GameObjectState::Dead;
	}
	else if (m_fightAnimationTime > sf::Time::Zero) {
		newState = m_fightAnimationState;
	}
	else if (!m_isGrounded) {
		newState = GameObjectState::Jumping;
	}
	else if (std::abs(m_mainChar->getVelocity().x) > 20.0f) {
		newState = GameObjectState::Walking;
	}

	// only update animation if we need to
	if (m_mainChar->getState() != newState || (m_nextIsFacingRight != m_isFacingRight)) {
		m_isFacingRight = m_nextIsFacingRight;
		m_mainChar->setState(newState, false);
		if (newState != GameObjectState::Dead)
			m_mainChar->setCurrentAnimation(m_mainChar->getAnimation(m_mainChar->getState()), !m_isFacingRight);
	}
}

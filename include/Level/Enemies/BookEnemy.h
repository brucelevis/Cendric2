#pragma once

#include "global.h"
#include "Level/Level.h"
#include "Spells/SpellManager.h"
#include "Screens/Screen.h"
#include "Particles/ParticleSystem.h"

// A flying book
class BookEnemy : public virtual Enemy {
public:
	BookEnemy(const Level* level, Screen* screen);
	virtual ~BookEnemy() { delete m_ps; }

	void update(const sf::Time& frameTime) override;
	void render(sf::RenderTarget& target) override;

	void onHit(Spell* spell) override;

	void setDead() override;

	MovingBehavior* createMovingBehavior(bool asAlly) override;
	AttackingBehavior* createAttackingBehavior(bool asAlly) override;

	sf::Time getConfiguredWaitingTime() const override;
	sf::Time getConfiguredChasingTime() const override;

	void insertDefaultLoot(std::map<std::string, int>& loot, int& gold) const override;
	void insertRespawnLoot(std::map<std::string, int>& loot, int& gold) const override;

	EnemyID getEnemyID() const override { return EnemyID::Book; }

protected:
	std::string getSpritePath() const override;
	void handleAttackInput();
	// loads attributes and adds immune spells + enemies. all attributes are set to zero before that call. default does nothing.
	void loadAttributes() override;
	// loads spells and adds them to the spell manager. default does nothing.
	void loadSpells() override;
	void loadAnimation(int skinNr) override;

	void loadParticleSystem();

private:
	particles::TextureParticleSystem* m_ps;
	particles::ParticleSpawner* m_particleSpawner;
};
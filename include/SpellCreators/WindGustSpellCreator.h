#pragma once

#include "global.h"
#include "Spells/SpellCreator.h"

#include "Spells/WindGustSpell.h"

// a class that creates windgust spells
class WindGustSpellCreator final : public SpellCreator {
public:
	WindGustSpellCreator(const SpellData& spellData, LevelMovableGameObject *owner);
	std::string getStrengthModifierName() const override;
	int getStrengthModifierValue() const override;

	void execExecuteSpell(const sf::Vector2f& target) override;

private:
	void addRangeModifier(int level) override;
	void addStrengthModifier(int level) override;
	void addDurationModifier(int level) override;
};
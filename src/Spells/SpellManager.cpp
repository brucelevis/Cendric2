#include "Spells/SpellManager.h"
#include "GUI/SpellSelection.h"

using namespace std;

SpellManager::SpellManager(LevelMovableGameObject* owner) {
	m_currentSpell = -1;
	m_owner = owner;
}

SpellManager::~SpellManager() {
	clearSpells();
}

void SpellManager::clearSpells() {
	CLEAR_VECTOR(m_spellMap);
	m_coolDownMap.clear();
}

int SpellManager::getSelectedSpell() {
	return m_currentSpell;
}

void SpellManager::setSpellSelection(SpellSelection* selection) {
	m_spellSelection = selection;
}

void SpellManager::setSpellsAllied(bool value) {
	for (auto& spellcreator : m_spellMap) {
		spellcreator->setSpellAllied(value);
	}
}

void SpellManager::setGlobalCooldown(const sf::Time& cooldown) {
	m_globalCooldown = cooldown;
}

void SpellManager::addSpell(const SpellData& spell) {
	addSpell(spell, std::vector<SpellModifier>());
}

void SpellManager::addSpell(const SpellData& spell, const std::vector<SpellModifier>& modifiers) {
	m_spellMap.push_back(SpellData::getSpellCreator(spell, modifiers, m_owner));
	m_coolDownMap.push_back(sf::Time::Zero);
}

template <typename T>
bool SpellManager::executeCurrentSpell(T target, bool force) {
	if (!force) {
		// check if execution is ready.
		if (m_remainingGlobalCooldown.asMilliseconds() != 0) return false;
		if (m_currentSpell == -1 || m_coolDownMap[m_currentSpell].asMilliseconds() != 0) return false;
		if (!m_owner->isReady()) return false;
		for (auto& spellcreator : m_spellMap) {
			if (!spellcreator->isReady())
				return false;
		}
	}

	// spell has been cast. set cooldown.
  	sf::Time cooldown = m_spellMap[m_currentSpell]->getSpellData().cooldown * m_owner->getAttributes()->cooldownMultiplier;
	m_coolDownMap[m_currentSpell] = cooldown;
	m_remainingGlobalCooldown = m_globalCooldown;
	m_spellMap[m_currentSpell]->executeSpell(target);
	if (m_spellSelection != nullptr) {
		m_spellSelection->activateSlot(m_currentSpell, cooldown);
	}
	return true;
}

bool SpellManager::executeCurrentSpell(const LevelMovableGameObject* target, bool force) {
	return executeCurrentSpell<const LevelMovableGameObject*>(target, force);
}

bool SpellManager::executeCurrentSpell(const sf::Vector2f& target, bool force) {
	return executeCurrentSpell<const sf::Vector2f&>(target, force);
}

void SpellManager::update(sf::Time frameTime) {
	// update global cooldown
	updateTime(m_remainingGlobalCooldown, frameTime);
	// update cooldown map
	for (auto& it : m_coolDownMap) {
		updateTime(it, frameTime);
	}
	// update spellcreators
	for (auto& it : m_spellMap) {
		(*it).update(frameTime);
	}
}

void SpellManager::setAndExecuteSpell(int spellNr) {
	setCurrentSpell(spellNr);
	if (m_currentSpell == -1) return;
	if (!(m_spellMap.at(m_currentSpell)->getSpellData().needsTarget) && m_currentSpell == spellNr) {
		executeCurrentSpell(sf::Vector2f());
	}
}

void SpellManager::setCurrentSpell(int spellNr) {
	if (spellNr < -1 || spellNr + 1 > static_cast<int>(m_spellMap.size())) {
		g_logger->logWarning("SpellManager::setCurrentSpell", "A invalid spell is set as current spell. Spell nr: " + to_string(spellNr));
		m_currentSpell = -1;
		return;
	}
	m_currentSpell = spellNr;
	if (m_spellSelection != nullptr) {
		m_spellSelection->selectSlot(spellNr);
	}
}

std::vector<SpellCreator*>& SpellManager::getSpellMap() {
	return m_spellMap;
}
#include "Level/LevelInterface.h"
#include "Level/LevelMainCharacter.h"
#include "Screens/WorldScreen.h"
#include "GUI/SlotClone.h"
#include "World/Item.h"

LevelInterface::LevelInterface(WorldScreen* screen, LevelMainCharacter* character) : WorldInterface(screen),
m_character(character) {
	loadGuiSidebar();
	loadMapSidebar();
	m_inventory = new Inventory(this);
	m_characterInfo = new CharacterInfo(screen, character->getAttributes());
	m_quickSlotBar = new QuickSlotBar(this);
	m_spellbook = new Spellbook(m_core, false);
	m_questLog = new QuestLog(m_core);
	m_mapOverlay = new MapOverlay(m_screen, m_mapSidebar);
	m_buffBar = new BuffBar(this);
	m_mainCharHealthBar = new HealthBar(character->getAttributes(), HealthBarStyle::MainCharacter);

	const Level* level = dynamic_cast<const Level*>(screen->getWorld());

	m_enemyHealthBar = new HealthBar(nullptr, level->getWorldData()->isBossLevel ? 
		HealthBarStyle::Boss : 
		HealthBarStyle::Enemy);

	m_enemyHealthBar->setVisible(false);
}

LevelInterface::~LevelInterface() {
	delete m_guiSidebar;
	delete m_mapSidebar;
	delete m_spellSelection;
	delete m_inventory;
	delete m_characterInfo;
	delete m_mainCharHealthBar;
	delete m_enemyHealthBar;
	delete m_quickSlotBar;
	delete m_spellbook;
	delete m_questLog;
	delete m_buffBar;
}

void LevelInterface::render(sf::RenderTarget& target) {
	target.setView(target.getDefaultView());
	
	m_buffBar->render(target);

	m_mainCharHealthBar->render(target);
	m_enemyHealthBar->render(target);
	
	m_spellSelection->render(target);
	
	WorldInterface::render(target);
}

void LevelInterface::renderAfterForeground(sf::RenderTarget& target) {
	WorldInterface::renderAfterForeground(target);
	
	m_quickSlotBar->renderAfterForeground(target);
	m_spellSelection->renderAfterForeground(target);
}

void LevelInterface::update(const sf::Time& frameTime) {
	if (m_character->isDead()) {
		m_mainCharHealthBar->update(frameTime);
		return;
	}
		
	WorldInterface::update(frameTime);

	m_mainCharHealthBar->update(frameTime);
	m_enemyHealthBar->update(frameTime);

	m_buffBar->update(frameTime);
	m_spellSelection->update(frameTime);
}

void LevelInterface::setEnemyForHealthBar(const Enemy* enemy) {
	if (enemy == nullptr) {
		m_enemyHealthBar->setVisible(false);
		m_enemyHealthBar->setAttributes(nullptr);
		return;
	}
	m_enemyHealthBar->setName(g_textProvider->getText(enemy->getEnemyName(), "enemy"));
	m_enemyHealthBar->setAttributes(enemy->getAttributes());
	m_enemyHealthBar->setVisible(true);
}

BuffBar& LevelInterface::getBuffBar() {
	return *m_buffBar;
}

void LevelInterface::clearConsumedFood() {
	m_consumedFood.clear();
}

void LevelInterface::restoreConsumedFood() {
	for (auto& it : m_consumedFood) {
		getScreen()->getCharacterCore()->notifyItemChange(it.first, it.second);
	}
	m_consumedFood.clear();
}

void LevelInterface::consumeItem(const std::string& itemID) {
	if (m_character->isEating()) return;
	Item* item = g_resourceManager->getItem(itemID);
	if (item == nullptr || !item->getCheck().isConsumable) return;
	auto const food = item->getBean<ItemFoodBean>();
	
	m_character->consumeFood(
		food->food_duration,
		item->getAttributes());
	getBuffBar().addFoodBuff(
		sf::IntRect(item->getIconTextureLocation().x, item->getIconTextureLocation().y, 50, 50),
		food->food_duration,
		item->getID(),
		item->getAttributes());

	if (!contains(m_consumedFood, item->getID())) {
		m_consumedFood.insert({ item->getID(), 0 });
	}
	m_consumedFood[item->getID()] += 1;

	m_screen->notifyItemChange(item->getID(), -1);
	m_quickSlotBar->reload();
}

void LevelInterface::reloadInventory(const std::string& changedItemID) {
	WorldInterface::reloadInventory(changedItemID);
	m_quickSlotBar->reload();
}

void LevelInterface::notifyCharacterInfo() {
	m_characterInfo->notifyChange();
}

bool LevelInterface::isEnemyHealthBarDisplayed() {
	return m_enemyHealthBar->getAttributes() != nullptr;
}
 
void LevelInterface::setSpellManager(SpellManager* spellManager) {
	// use this spell manager for the interface bar.
	delete m_spellSelection;
	m_spellSelection = new SpellSelection(spellManager);
}

void LevelInterface::setPermanentCore(CharacterCore* permanentCore) {
	m_permanentCore = permanentCore;
}

LevelMainCharacter* LevelInterface::getMainCharacter() const {
	return m_character;
}

CharacterCore* LevelInterface::getPermanentCore() const {
	return m_permanentCore;
}
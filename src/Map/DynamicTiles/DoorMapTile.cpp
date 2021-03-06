#include "Map/DynamicTiles/DoorMapTile.h"
#include "Map/Map.h"
#include "Screens/MapScreen.h"
#include "GameObjectComponents/InteractComponent.h"
#include "FileIO/ParserTools.h"
#include "Registrar.h"

REGISTER_MAP_DYNAMIC_TILE(MapDynamicTileID::Door, DoorMapTile)

const float DoorMapTile::OPEN_RANGE = 50.f;

DoorMapTile::DoorMapTile(MapScreen* mapScreen) : MapDynamicTile(mapScreen) {

	m_interactComponent = new InteractComponent(g_textProvider->getText("Door"), this, m_mainChar);
	m_interactComponent->setInteractRange(OPEN_RANGE);
	m_interactComponent->setInteractText("ToOpen");
	m_interactComponent->setOnInteract(std::bind(&DoorMapTile::onRightClick, this));
	addComponent(m_interactComponent);
}

void DoorMapTile::open() {
	m_isOpen = true;
	m_isCollidable = false;
	setState(GameObjectState::Open);
	m_interactComponent->setInteractable(false);
}

void DoorMapTile::close() {
	m_isOpen = false;
	m_isCollidable = true;
	setState(GameObjectState::Closed);
	m_interactComponent->setInteractable(true);
}

void DoorMapTile::update(const sf::Time& frameTime) {
	if (m_isReloadNeeded) {
		reloadConditions();
	}

	MapDynamicTile::update(frameTime);
}

bool DoorMapTile::init(const MapTileProperties& properties) {
	setBoundingBox(sf::FloatRect(0.f, 0.f, TILE_SIZE_F, TILE_SIZE_F));
	setPositionOffset(sf::Vector2f(0.f, 0.f));
	setSpriteOffset(sf::Vector2f(0.f, 0.f));

	if (contains(properties, std::string("key"))) {
		m_keyItemId = properties.at("key");
	}

	if (contains(properties, std::string("not conditions"))) {
		auto notCond = ParserTools::parseConditions(properties.at("not conditions"), true);
		for (auto& cond : notCond) {
			m_conditions.push_back(cond);
		}
	}

	if (contains(properties, std::string("conditions"))) {
		auto notCond = ParserTools::parseConditions(properties.at("conditions"), false);
		for (auto& cond : notCond) {
			m_conditions.push_back(cond);
		}
	}

	return true;
}

void DoorMapTile::loadAnimation(int skinNr) {
	const sf::Texture* tex = g_resourceManager->getTexture(getSpritePath());
	Animation* openAnimation = new Animation();
	openAnimation->setSpriteSheet(tex);
	openAnimation->addFrame(sf::IntRect(0, skinNr * TILE_SIZE, TILE_SIZE, TILE_SIZE));

	addAnimation(GameObjectState::Open, openAnimation);

	Animation* closedAnimation = new Animation();
	closedAnimation->setSpriteSheet(tex);
	closedAnimation->addFrame(sf::IntRect(TILE_SIZE, skinNr * TILE_SIZE, TILE_SIZE, TILE_SIZE));

	addAnimation(GameObjectState::Closed, closedAnimation);

	playCurrentAnimation(false);
	reloadConditions();
}

std::string DoorMapTile::getSpritePath() const {
	return "res/assets/map_dynamic_tiles/spritesheet_tiles_door.png";
}

void DoorMapTile::onRightClick() {
	if (m_isOpen) return;

	// check if the door is in range
	bool inRange = dist(m_mainChar->getCenter(), getCenter()) <= OPEN_RANGE;

	if (!inRange) {
		m_screen->setNegativeTooltip("OutOfRange");
	}
	else if (m_isConditionsFulfilled && !m_keyItemId.empty() && m_screen->getCharacterCore()->hasItem(m_keyItemId, 1)) {
		open();
		g_resourceManager->playSound(GlobalResource::SOUND_MISC_UNLOCK);

		std::string tooltipText = g_textProvider->getText("Used");
		tooltipText.append(g_textProvider->getText(m_keyItemId, "item"));
		m_screen->setTooltipTextRaw(tooltipText, COLOR_GOOD, true);

		g_inputController->lockAction();
	}
	else {
		m_screen->setNegativeTooltip("IsLockedKey");
	}
}

void DoorMapTile::reloadConditions() {
	CharacterCore* core = m_screen->getCharacterCore();

	bool conditionsFulfilled = true;
	for (auto& condition : m_conditions) {
		if ((condition.negative && core->isConditionFulfilled(condition.type, condition.name))
			|| (!condition.negative && !core->isConditionFulfilled(condition.type, condition.name))) {
			conditionsFulfilled = false;
			break;
		}
	}

	m_isConditionsFulfilled = conditionsFulfilled;

	if ((m_isConditionsFulfilled && m_keyItemId.empty())
		|| m_mainChar->getBoundingBox()->intersects(*getBoundingBox())) {
		open();
	}
	else {
		close();
	}

	m_isReloadNeeded = false;
}

void DoorMapTile::notifyReloadNeeded() {
	m_isReloadNeeded = true;
}
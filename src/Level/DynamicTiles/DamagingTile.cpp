#include "Level/DynamicTiles/DamagingTile.h"
#include "GameObjectComponents/LightComponent.h"
#include "Spells/Spell.h"
#include "Registrar.h"

REGISTER_LEVEL_DYNAMIC_TILE(LevelDynamicTileID::Damaging, DamagingTile)

bool DamagingTile::init(const LevelTileProperties& properties) {
	setPositionOffset(sf::Vector2f(5.f, 5.f));
	setSpriteOffset(sf::Vector2f(-5.f, -5.f));
	setBoundingBox(sf::FloatRect(0.f, 0.f, TILE_SIZE_F - 10.f, TILE_SIZE_F - 10.f));
	addComponent(new LightComponent(LightData(sf::Vector2f(TILE_SIZE_F * 0.5f, TILE_SIZE_F * 0.5f), TILE_SIZE_F, 0.5f), this));

	m_isDivine = contains(properties, std::string("divine"));
	return true;
}

void DamagingTile::loadAnimation(int skinNr) {
	const sf::Texture* tex = g_resourceManager->getTexture(getSpritePath());

	Animation* idleAnimation = new Animation(sf::seconds(0.05f));
	idleAnimation->setSpriteSheet(tex);
	for (int i = 0; i < 4; ++i) {
		idleAnimation->addFrame(sf::IntRect(i * TILE_SIZE, skinNr * TILE_SIZE, TILE_SIZE, TILE_SIZE));
	}

	addAnimation(GameObjectState::Idle, idleAnimation);

	// initial values
	setState(GameObjectState::Idle);
	playCurrentAnimation(true);
}

void DamagingTile::onHit(LevelMovableGameObject* mob) {
	mob->setDead();
}

void DamagingTile::onHit(Spell* spell) {
	if (!m_isDivine || spell->getSpellID() != SpellID::DivineShield) {
		return;
	}
	setDisposed();
}

std::string DamagingTile::getSpritePath() const {
	return "res/assets/level_dynamic_tiles/spritesheet_tiles_damaging.png";
}
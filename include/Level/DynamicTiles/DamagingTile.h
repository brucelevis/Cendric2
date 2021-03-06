#pragma once

#include "global.h"
#include "Level/LevelDynamicTile.h"

class DamagingTile final : public LevelDynamicTile {
public:
	DamagingTile(LevelScreen* levelScreen) : LevelDynamicTile(levelScreen) {}
	bool init(const LevelTileProperties& properties) override;
	void loadAnimation(int skinNr) override;
	void onHit(LevelMovableGameObject* mob) override;
	void onHit(Spell* spell) override;
	LevelDynamicTileID getDynamicTileID() const override { return LevelDynamicTileID::Damaging; }

private:
	std::string getSpritePath() const override;
	bool m_isDivine = false;
};
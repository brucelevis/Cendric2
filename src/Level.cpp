#include "stdafx.h"

Level::Level()
{
}

Level::~Level()
{
}

bool Level::loadFromFile(char* fileName) 
{
	LevelReader reader;
	LevelData data;
	if (!reader.readLevel(fileName, data))
	{
		return false;
	}

	// load level
	m_startPos = data.startPos;
	m_name = data.name;
	m_tileMap.load(data.tileSetPath, data.tileSize, data.layers, data.mapSize.x, data.mapSize.y);
	m_collidableTiles = data.collidableTiles;

	return true;
}

void Level::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	m_tileMap.draw(target, states);
}

TileMap Level::getTilemap() 
{
	return m_tileMap;
}
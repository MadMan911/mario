#include "Game.hpp"
//myfiles
#include "context.hpp"
#include "MenuState.hpp"
#include <math.h>

extern context_t ctx;

//GameState
/**
 * @brief GameState
 */

GameState::GameState(std::string filename) {
	std::ifstream file(filename);
	if (!file.is_open())
		throw "File not found!";

	std::string level_name;
	while (!file.eof()) {
		file >> level_name;
		_Levels.emplace_back(level_name, nullptr);
	}

	file.close();
}

void GameState::update(float dt) {
	//Рассчёты для правильной быстрой отрисовки
    /**
 * @brief Calculations for correct fast rendering
 */
	auto& map = _Current->second->_Content;
	auto& playerBounds = _Player->box;

	_Player->update(dt);

	std::list <std::list <IEntity>::iterator> onRemove;
	for (auto it = _Current->second->_Entities.begin(); it != _Current->second->_Entities.end(); it++) {
		auto& enemyBounds = (*it)->box;

		(*it)->update(dt);
		if (enemyBounds.top + enemyBounds.height > ctx.window.getSize().y)
			onRemove.push_back(it);

		if (playerBounds.intersects(enemyBounds) && (*it)->isAlive) {
			if (playerBounds.top + 16 < enemyBounds.top) {
				(*it)->isAlive = false;
				(*it)->speed.x = -3;
				(*it)->speed.y = -18;

				_Player->speed.y = -17;

				_ScorePoints += 100;
				_Score.setString(std::to_string(_ScorePoints));
			} else {
				ctx.states.remove("game");
				ctx.states.insert("over", IState(new GameoverState("lose")));
			}
		}
	}

	for (const auto& it : onRemove) {
		_Current->second->_Entities.erase(it);
	}

	//width
    /**
* @brief width
*/
	for (size_t i = 0; i != map.size(); i++) {
		//height
        /**
* @brief height
*/
		for (size_t j = 0; j != map[i].size(); j++) {
			auto& top_lhs = _LevelVerticies[(i + j * map.size()) * 4];
			auto& top_rhs = _LevelVerticies[(i + j * map.size()) * 4 + 1];
			auto& bot_rhs = _LevelVerticies[(i + j * map.size()) * 4 + 2];
			auto& bot_lhs = _LevelVerticies[(i + j * map.size()) * 4 + 3];

			size_t id = map[i][j];

			size_t idx = id % 22;
			size_t idy = id / 22;

			top_lhs.position = sf::Vector2f(i * TILE_SIZE, j * TILE_SIZE);
			top_rhs.position = sf::Vector2f(i * TILE_SIZE + TILE_SIZE, j * TILE_SIZE);
			bot_rhs.position = sf::Vector2f(i * TILE_SIZE + TILE_SIZE, j * TILE_SIZE + TILE_SIZE);
			bot_lhs.position = sf::Vector2f(i * TILE_SIZE, j * TILE_SIZE + TILE_SIZE);

			top_lhs.texCoords = sf::Vector2f(idx * (TILE_SIZE + 2), idy * (TILE_SIZE + 2));
			top_rhs.texCoords = sf::Vector2f(idx * (TILE_SIZE + 2) + TILE_SIZE, idy * (TILE_SIZE + 2));
			bot_rhs.texCoords = sf::Vector2f(idx * (TILE_SIZE + 2) + TILE_SIZE, idy * (TILE_SIZE + 2) + TILE_SIZE);
			bot_lhs.texCoords = sf::Vector2f(idx * (TILE_SIZE + 2), idy * (TILE_SIZE + 2) + TILE_SIZE);
		}
	}

	//Смена уровней
    /**
* @brief changing levels
*/
	float endLine = map.size() * 32 - 48;
	if (playerBounds.left > endLine) {
		if (_Current != --_Levels.end()) {
			_Current++;
			_OnLevelChanged = true;
		} else {
			ctx.states.remove("game");
			ctx.states.insert("over", IState(new GameoverState("win")));
		}
	}

	if (_OnLevelChanged) {
		_OnLevelChanged = false;
		_ChangeLevel();
	}
}

void GameState::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	//Фон
    /**
* @brief background
*/
	states.texture = _BackgroundTexture;
	ctx.window.draw(_BackgroundVerticies, states);

	//Карта мира
    /**
* @brief map of world
*/
	states.texture = _Current->second->_Tileset;
	ctx.window.draw(_LevelVerticies, states);

	//Сущности
    /**
* @brief entities
*/
	ctx.window.draw(*_Player);
	ctx.window.draw(_Score, states);

	for (auto& it : _Current->second->_Entities) {
		target.draw(*it, states);
	}
}

void GameState::_ChangeLevel() {
	auto& map = _Current->second->_Content;

	_OnLevelChanged = false;

	size_t width = map.size(), height = map[0].size();

	ctx.window.setSize(sf::Vector2u(width * TILE_SIZE, height * TILE_SIZE));
	_LevelVerticies.resize(width * height * 4);

	//прикрепление сущностей
    /**
* @brief attaching entities
*/
	_Player->linked = _Current->second;
	_Player->box.left = _Current->second->_StartX;
	_Player->box.top = _Current->second->_StartY;

	//это изменение размера фона
    /**
* @brief background resizing
*/
	auto window_size = ctx.window.getSize();

	_BackgroundVerticies[0].position = sf::Vector2f(0, 0);
	_BackgroundVerticies[1].position = sf::Vector2f(window_size.x, 0);
	_BackgroundVerticies[2].position = sf::Vector2f(window_size.x, window_size.y);
	_BackgroundVerticies[3].position = sf::Vector2f(0, window_size.y);

	_BackgroundVerticies[0].texCoords = sf::Vector2f(0, 0);
	_BackgroundVerticies[1].texCoords = sf::Vector2f(window_size.x, 0);
	_BackgroundVerticies[2].texCoords = sf::Vector2f(window_size.x, window_size.y);
	_BackgroundVerticies[3].texCoords = sf::Vector2f(0, window_size.y);
}

void GameState::onCreate() {
	if (_Levels.empty())
		return;

	//Фон
    /**
* @brief background
*/
	_BackgroundTexture = &ctx.textures["background.png"];

	//Подготовка карты
    /**
* @brief map preparation
*/
	sf::Texture& tileset = ctx.textures["tiles.png"];
	for (auto& [level_name, level] : _Levels) {
		level.reset(new Map(tileset));
		level->load("assets/levels/" + level_name);

		for (auto& it : level->_Entities) {
			it->setMap(level);
		}
	}

	_Current = _Levels.begin();

	_Player = IEntity(new Player());

	_Score.setString(std::to_string(_ScorePoints));
	_Score.setCharacterSize(42);
	_Score.setFont(ctx.font);
	_Score.setPosition(10, 10);
	_Score.setFillColor(sf::Color::Black);
	_Score.setOutlineColor(sf::Color::Black);

	_ChangeLevel();
}

//Entity
/**
* @brief entity
*/
void Entity::update(float dt) {
	auto& map = linked;

	float left = box.left, right = box.left + box.width, top = box.top, down = box.top + box.height;

	if (speed.y < maxSpeed * 1.5) {
		speed.y += accel * 2;
	}

	isLeftFree = map->at(std::floor((left + speed.x) / 32), std::floor((top) / 32)) == 0;
	isRightFree = map->at(std::floor((right + speed.x) / 32), std::floor(top / 32)) == 0;

	if ((size_t)box.left % 32 == 0) {
		size_t idx = std::floor(left / 32);
		size_t idy = std::floor((down + speed.y) / 32);
		isDownFree = map->at(std::floor(left / 32), std::floor((down + speed.y) / 32)) == 0;
		isUpFree = map->at(std::floor(left / 32), std::floor((top + speed.y) / 32)) == 0;
	} else {
		isDownFree = map->at(std::floor(left / 32), std::floor((down + speed.y) / 32)) == 0 &&
			map->at(std::floor(right / 32), std::floor((down + speed.y) / 32)) == 0;

		isUpFree = map->at(std::floor(left / 32), std::floor((top + speed.y) / 32)) == 0 &&
			map->at(std::floor(right / 32), std::floor((top + speed.y) / 32)) == 0;
	}

	if (speed.x > 0) {
		if (!isRightFree) {
			box.left = std::floor((box.left + box.width + speed.x) / 32) * 32 - box.width;
			speed.x = 0;
		}
	} else if (speed.x < 0) {
		if (!isLeftFree) {
			box.left = std::floor((box.left + speed.x) / 32 + 1) * 32;
			speed.x = 0;
		}
	}

	if (speed.y > 0) {
		if (!isDownFree) {
			box.top = std::floor((box.top + box.height + speed.y) / 32) * 32 - box.height;
			speed.y = 0;
		}
	} else if (speed.y < 0) {
		if (!isUpFree) {
			std::vector <std::pair <size_t, size_t>> dots;
			if ((size_t)box.left % 32 == 0)
				dots.push_back({ std::floor(left / 32), std::floor((top + speed.y) / 32) });
			else {
				dots.push_back({ std::floor(left / 32), std::floor((top + speed.y) / 32) });
				dots.push_back({ std::floor(right / 32), std::floor((top + speed.y) / 32) });
			}

			for (const auto& it : dots) {
				auto state = std::dynamic_pointer_cast <GameState>(ctx.states.get("game"));
				state->BoundPointsForDot(it.first, it.second);
			}

			box.top = std::floor((box.top + speed.y) / 32 + 1) * 32;
			speed.y = 0;
		}
	}

	box.left += speed.x;
	box.top += speed.y;

	sprite.setPosition(box.left, box.top);
}

void Entity::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(sprite, states);
}

//Player
/**
* @brief player
*/

Player::Player() {
	_UpKey = ctx.input[sf::Keyboard::W];
	_LeftKey = ctx.input[sf::Keyboard::A];
	_DownKey = ctx.input[sf::Keyboard::S];
	_RightKey = ctx.input[sf::Keyboard::D];

	setTexture(ctx.textures["mario.png"]);
	sprite.setPosition(0, 72);
	box.width = 32;
	box.height = 32;
}

void Player::update(float dt) {
	bool flag = false;

	if (_LeftKey->isEnable()) {
		flag = true;
		if (speed.x > -maxSpeed)
			speed.x -= accel;
	}
	if (_RightKey->isEnable()) {
		flag = true;
		if (speed.x < maxSpeed)
			speed.x += accel;
	}
	if (_UpKey->isPressed() && !isDownFree) {
		speed.y = -18;
	}

	if (!flag && speed.x != 0) {
		speed.x += accel * (speed.x > 0 ? -1.f : 1.f);
	}

	_DoAnimation(dt);
	sprite.setTextureRect(sf::IntRect((_FrameId % 6) * 32, (_FrameId / 6) * 32, 32, 32));
	//std::cout << timer << ' ';

	Entity::update(dt);
}

void Player::_DoAnimation(float dt) {
	if (speed.x == 0 && speed.y == 0) {
		_State = Stable;
		_FrameId = 0 + _Dir * 6;
		_Timer = 0;
	} else if (speed.y == 0) {
		_Timer += dt;
		if (_Timer > 0.05f) {
			_Timer = 0;
			if (speed.x > 0) {
				_Dir = false;

				if (_State == Stable || _FrameId >= 4)
					_FrameId = 0;
				else
					_FrameId++;

				_State = Running;
			} else if (speed.x < 0) {
				_Dir = true;

				if (_State == Stable || _FrameId >= 10)
					_FrameId = 6;
				else
					_FrameId++;

				_State = Running;
			}
		}
	} else if (speed.y < 0) {
		_Timer = 0;
		_State = Jumping;
		_FrameId = 5 + _Dir * 6;
	}

	//std::cout << _FrameId << ' ';
}

//Проверка правильности работы
/**
* @brief checking the correct operation
*/
//Пример теста для файла 1.txt:

//30 20
//32 96
//0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
//0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
//0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
//0 0 0 0 7 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
//0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
//0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 7 0 0
//0 0 0 0 0 0 6 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
//0 1 0 0 0 0 6 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
//0 0 0 0 0 0 6 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
//0 0 0 0 6 6 6 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 6 7 6 7 6
//0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
//0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
//0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
//1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
//1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
//1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
//1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
//1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
//1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
//1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1

void test_MapAt() {
	Map map(ctx.textures["tiles.png"]);
	map.load("assets/levels/0.txt");
	bool flag = false;

	flag |= map.at(-1, 10) != 0;
	flag |= map.at(0, 0) != 0;
	flag |= map.at(5, 4) != 7;

	if (!flag)
		throw("Wrong values for Map 1.txt");
}

//Entity creator
/**
* @brief entity creator
*/

IEntity createEntity(std::ifstream& line) {
	IEntity result;
	
	std::string name;
	line >> name;

	if (name == "mushroom") {
		float sx, sy;
		bool dir;

		line >> sx >> sy >> dir;

		result.reset(new Mushroom(sx, sy, dir));
	}

	return result;
}

//Mushroom
/**
* @brief mushroom
*/
Mushroom::Mushroom(float sx, float sy, bool direction) {
	box.left = sx;
	box.top = sy;
	box.width = 32;
	box.height = 32;

	_Direction = direction;

	setTexture(ctx.textures["mushroom.png"]);
	sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));
}

void Mushroom::update(float dt) {
	if (isAlive) {
		//left
		if (_Direction) {
			speed.x = -4;
		}
		//right
		else {
			speed.x = 4;
		}

		Entity::update(dt);

		if (!isLeftFree)
			_Direction = false;
		else if (!isRightFree)
			_Direction = true;

		_FrameId++;
		if (_FrameId > 1)
			_FrameId = 0;

		_Timer += dt;
		if (_Timer > 0.05f) {
			sprite.setTextureRect(sf::IntRect(_FrameId * 32, 0, 32, 32));
			_Timer = 0;
		}
	}

	else {
		_FrameId = 2;
		sprite.setTextureRect(sf::IntRect(_FrameId * 32, 0, 32, 32));

		speed.y += 1.f;

		box.left += speed.x;
		box.top += speed.y;

		sprite.setPosition(box.left, box.top);
	}
}

//Map
/**
* @brief map
*/
void Map::load(std::string filename) {
	std::ifstream file(filename);
	if (!file.is_open())
		throw "File not found!";

	size_t width, height, enenmyCount;
	file >> width >> height;
	file >> _StartX >> _StartY;
	file >> enenmyCount;
	resize(width, height);


	for (size_t i = 0; i < width * height; i++) {
		file >> _Content[i % width][i / width];
	}

	for (size_t i = 0; i != enenmyCount; i++) {
		auto temp = createEntity(file);
		_Entities.push_back(temp);
	}

	file.close();
}
#pragma once

#include <vector>
#include <list>
#include <fstream>

#include <SFML/Graphics.hpp>

#include "StateMachine.hpp"
#include "InputController.hpp"

template<class _T>
using Matrix = std::vector<std::vector<_T>>;

using tile_t = size_t;
using IEntity = std::shared_ptr<class Entity>;

// Class map of the level
class Map {
    friend class Entity;

    friend class GameState;

public:
    // Constructor with textures
    // @param tileset - all textures
    Map(const sf::Texture &tileset) : _Tileset(&tileset) {}

    // Resize the _content Matrix
    // @param width - new width of _Content
    // @param height - new height of _Content
    void resize(size_t width, size_t height) {
        _Content.resize(width);
        for (size_t i = 0; i != width; i++)
            _Content[i].resize(height, tile_t{0});
    }

    // Load map from file
    // @param filename - filename
    void load(std::string filename);

    // Get tileset id from _Content[x][y]
    // @param x
    // @param y
    size_t at(size_t x, size_t y) {
        if (x < 0 || x >= _Content.size() || y < 0 || y >= _Content[0].size())
            return 1;
        return _Content[x][y];
    }

private:
    std::list<IEntity> _Entities; // All entities on map
    Matrix<tile_t> _Content; // All content
    float _StartX, _StartY; // Start points
    const sf::Texture *_Tileset; // Al sfml textures
};

// Class for all entities
class Entity : public sf::Drawable {
    friend class GameState;

public:
    Entity() {}

    // Update on each frame
    // @param dt - elapsed time from last frame
    virtual void update(float dt);

    // Point map for that entity
    void setMap(std::shared_ptr<Map> map) {
        linked = map;
    }

protected:
    std::shared_ptr<Map> linked; // linked map
    sf::Vector2f speed; // speed of entity
    sf::Sprite sprite; // sprite of entity
    sf::FloatRect box; // Collision box
    float accel = 0.5f, maxSpeed = 8.f; // Acceleration and max speed
    bool isUpFree, isDownFree = true, isLeftFree, isRightFree, isAlive = true; // Flag for accept directions

    // Set entity texture
    void setTexture(const sf::Texture &texture) {
        sprite.setTexture(texture);
        sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));
    }

private:
    // Draw entity
    void draw(sf::RenderTarget &target, sf::RenderStates states = sf::RenderStates::Default) const final override;
};

// Creat entity from config
// @param line - config line for entity
IEntity createEntity(std::ifstream &line);

// Player class
class Player : public Entity {
    friend class GameState;

public:
    Player();

    void update(float dt) final override;

    enum State {
        Stable,
        Running,
        Jumping,
        Falling
    };

private:
    IKeyStatus _UpKey, _LeftKey, _DownKey, _RightKey;
    State _State = Stable;

    bool _Dir = true;
    short _FrameId;
    float _Timer = 0;

    void _DoAnimation(float dt);
};


// Mushroom class
class Mushroom : public Entity {
public:
    Mushroom(float sx, float sy, bool direction);

    void update(float dt) final override;

private:

    float _Timer;
    short _FrameId = 0;
    bool _Direction;
};

// State of the game class
class GameState : public State {
public:
    // Constructor which gets config from file
    // @param filename
    GameState(std::string filename);

    void BoundPointsForDot(size_t x, size_t y) {
        if (_Current->second->at(x, y) == 7) {
            _Current->second->_Content[x][y] = 6;
            _ScorePoints += 100;
            _Score.setString(std::to_string(_ScorePoints));
        }
    }

private:
    std::list<std::pair<std::string, std::shared_ptr<Map>>> _Levels; // List of levels
    std::list<std::pair<std::string, std::shared_ptr<Map>>>::iterator _Current; // Iterator to current level
    sf::VertexArray _LevelVerticies = sf::VertexArray(sf::PrimitiveType::Quads), _BackgroundVerticies = sf::VertexArray(
            sf::PrimitiveType::Quads, 4);
    sf::Text _Score; // Score drawable object
    sf::Texture *_BackgroundTexture = nullptr;
    bool _OnLevelChanged = false;
    size_t _ScorePoints = 0; // Current score

    IEntity _Player;

    void update(float dt) final override;

    void onCreate() final override;

    void draw(sf::RenderTarget &target, sf::RenderStates states = sf::RenderStates::Default) const final override;

    //This is for the window to resize to fit the level and also the vertex pool resized to the desired value
    void _ChangeLevel();
};
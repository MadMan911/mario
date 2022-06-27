#include "MenuState.hpp"
//myfiles
/**
* @brief our files
*/
#include "context.hpp"
#include "Game.hpp"

extern context_t ctx;

namespace mouse_input {
	bool leftPre = false, leftNow = false;
	void update(bool nextLeft) {
		leftPre = leftNow;
		leftNow = nextLeft;
	}
}

void GameoverState::update(float dt) {
	mouse_input::update(sf::Mouse::isButtonPressed(sf::Mouse::Left));

	auto mouse = sf::Mouse::getPosition(ctx.window);

	if (_Exit.getGlobalBounds().contains(mouse.x, mouse.y) && mouse_input::leftPre && !mouse_input::leftNow) {
		ctx.states.remove("over");
		ctx.window.close();
	}
}

void GameoverState::onCreate() {
	_Label.setCharacterSize(42);
	_Label.setFont(ctx.font);
	_Label.setPosition(30, 50);
	_Label.setFillColor(sf::Color::White);
	_Label.setOutlineColor(sf::Color::White);

	_Exit.setString("Exit");
	_Exit.setCharacterSize(42);
	_Exit.setFont(ctx.font);
	_Exit.setPosition(30, 120);
	_Exit.setFillColor(sf::Color::White);
	_Exit.setOutlineColor(sf::Color::White);

	ctx.window.setSize(sf::Vector2u(600, 300));
}

void GameoverState::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(_Label, states);
	target.draw(_Exit, states);
}

void MenuState::update(float dt) {
	mouse_input::update(sf::Mouse::isButtonPressed(sf::Mouse::Left));

	auto mouse = sf::Mouse::getPosition(ctx.window);

	if (_Exit.getGlobalBounds().contains(mouse.x, mouse.y) && mouse_input::leftPre && !mouse_input::leftNow) {
		ctx.states.remove("menu");
		ctx.window.close();
	} 
	if (_Start.getGlobalBounds().contains(mouse.x, mouse.y) && mouse_input::leftPre && !mouse_input::leftNow) {
		ctx.states.remove("menu");
		ctx.states.insert("game", IState(new GameState("assets/levels/order.cfg")));
	}
}

void MenuState::onCreate() {
	_Start.setString("Start");
	_Start.setCharacterSize(42);
	_Start.setFont(ctx.font);
	_Start.setPosition(30, 50);
	_Start.setFillColor(sf::Color::White);
	_Start.setOutlineColor(sf::Color::White);

	_Exit.setString("Exit");
	_Exit.setCharacterSize(42);
	_Exit.setFont(ctx.font);
	_Exit.setPosition(30, 120);
	_Exit.setFillColor(sf::Color::White);
	_Exit.setOutlineColor(sf::Color::White);

	ctx.window.setSize(sf::Vector2u(400, 300));
}

void MenuState::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(_Start, states);
	target.draw(_Exit, states);
}

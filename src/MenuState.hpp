#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "StateMachine.hpp"

class GameoverState : public State {
public:
	GameoverState(std::string text) {
		_Label.setString("Game is over! You " + text + "!");
	}

	void update(float dt) final override;
	
	void onCreate() final override;

private:
	sf::Text _Label, _Exit;

	void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const final override;
};

class MenuState : public State {
public:
	void update(float dt) final override;

	void onCreate() final override;

private:
	sf::Text _Start, _Exit;

	void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const final override;
};
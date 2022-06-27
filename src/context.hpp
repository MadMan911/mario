#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>

#include "StateMachine.hpp"
#include "TextureManager.hpp"
#include "InputController.hpp"

#define TILE_SIZE 32

// Controller of all the most important processes
struct context_t {

	sf::RenderWindow window;
	sf::Font font;
	sf::Clock clock;
	float delta = clock.getElapsedTime().asSeconds();

	StateMachine states;
	TextureManager textures;
	InputController input = InputController({ sf::Keyboard::W, sf::Keyboard::A, sf::Keyboard::S, sf::Keyboard::D });

	const float gravity = 4.f;
	const float delay = 1000.f / 60;

	void init() {
		window.create(sf::VideoMode(800, 600), "Simple platformer");
		window.setFramerateLimit(60);

		font.loadFromFile("assets/font.ttf");
		textures.load("assets/textures/include.cfg");
	}
};
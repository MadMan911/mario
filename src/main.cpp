#include <iostream>

//myfiles
/**
* @brief our files
*/
#include "Game.hpp"
#include "MenuState.hpp"
#include "context.hpp"
context_t ctx;

sf::RenderWindow& window = ctx.window;
StateMachine& states = ctx.states;

int main() {
	ctx.init();

	states.insert("menu", IState(new MenuState()));

	while (window.isOpen()) {
		ctx.delta = ctx.clock.getElapsedTime().asSeconds();
		ctx.clock.restart();

		//обработка стандартных ивентов окна из SFML
        /**
* @brief handling standard window events from SFML
*/
		sf::Event e;
		while (window.pollEvent(e)) {
			switch (e.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::Resized:
				window.setView(sf::View(sf::FloatRect(0, 0, e.size.width, e.size.height)));
				break;
			}
		}

		//Обработка главного цикла и отрисовка
        /**
* @brief Processing the main loop and rendering
*/
		window.clear(sf::Color::Black);

		ctx.input.update();
		states.update(ctx.delta);

		window.display();
	}
}
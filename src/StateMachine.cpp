#include "StateMachine.hpp"
//std
/**
* @brief std
*/
#include <list>
#include <string>

//myfiles
/**
* @brief our files
*/
#include "context.hpp"
extern context_t ctx;

//Здесь происходит весь движ
/**
* @brief This is where all the action happens.
*/
void StateMachine::update(float dt) {
	std::list <std::string> onRemove;
	for (auto& [id, state] : _Container) {
		switch (state->_Status) {
		case State::OnCreate:
			state->onCreate();
			state->_Status = State::Active;
			break;
		/*case State::OnResume:
			state->onResume();
			break;*/
		case State::Active:
			state->update(dt);
			ctx.window.draw(*state);
			break;
		/*case State::OnPause:
			state->onPause();
			break;*/
		case State::OnDelete:
			state->onDelete();
			onRemove.push_back(id);
			break;
		}
	}

	//Очередь на удаление
    /**
* @brief Delete Queue
*/
	for (const auto& it : onRemove)
		_Container.erase(it);
}
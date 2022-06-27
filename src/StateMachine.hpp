#pragma once

#include <memory>
#include <string>
#include <list>
#include <unordered_map>

#include <SFML/Graphics/Drawable.hpp>


class State : public sf::Drawable {
	friend class StateMachine;
public:
	enum Status {
		OnCreate,

		Active,

		OnDelete
	};

protected:
	virtual void update(float dt) = 0;

	virtual void onCreate() = 0;
	virtual void onDelete() {}

private:
	Status _Status = OnCreate;
};
using IState = std::shared_ptr <State>;

// Controller of all states of the game
class StateMachine {
public:
	void insert(std::string id, IState state) {
		state->_Status = State::OnCreate;
		_Container.emplace(id, state);
	}
	void remove(std::string id) {
		_Container[id]->_Status = State::OnDelete;
	}
	IState get(std::string id) {
		return _Container[id];
	}

	void update(float dt);

private:
	std::unordered_map <std::string, IState> _Container;
};
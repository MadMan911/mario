#pragma once

#include <initializer_list>
#include <unordered_map>
#include <memory>

#include <SFML/Window/Keyboard.hpp>

// Getting keys status
class KeyStatus {
	friend class InputController;
public:
	bool isPressed() {
		return !pre && now;
	}
	bool isUpped() {
		return pre && !now;
	}
	bool isEnable() {
		return now;
	}
	bool isDisabled() {
		return !now;
	}

private:
	bool pre = false, now = false;

	void update(bool next) {
		pre = now;
		now = next;
	}
};
using IKeyStatus = std::shared_ptr <KeyStatus>;

// Input controlling class
class InputController {
public:
	InputController(std::initializer_list <sf::Keyboard::Key> keys) {
		for (const auto& it : keys) {
			_Container.emplace(it, IKeyStatus(new KeyStatus()));
		}
	}

	void update() {
		for (const auto& it : _Container)
			it.second->update(sf::Keyboard::isKeyPressed(it.first));
	}

	IKeyStatus operator[](sf::Keyboard::Key key) {
		return _Container[key];
	}

private:
	std::unordered_map <sf::Keyboard::Key, IKeyStatus> _Container;
};
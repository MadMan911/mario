#pragma once

#include <string>
#include <fstream>
#include <unordered_map>

#include <SFML/Graphics/Texture.hpp>

//Texture controller that loads all the necessary files from the config
class TextureManager {
public:
	void load(std::string config_name) {
		std::ifstream file(config_name);

		while (!file.eof()) {
			std::string temp;
			file >> temp;

			sf::Texture _Texture;
			_Texture.loadFromFile("assets/textures/" + temp);

			_Content.emplace(temp, std::move(_Texture));
		}
	}

	sf::Texture& operator[](std::string key) {
		auto result = _Content.find(key);
		if (result == _Content.end())
			throw ("Not found texture \'" + key + "\'").c_str();

		return _Content.at(key);
	}

private:
	std::unordered_map <std::string, sf::Texture> _Content;
};
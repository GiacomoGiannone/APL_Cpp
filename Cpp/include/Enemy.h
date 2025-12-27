#pragma once

#include <SFML/Graphics.hpp>

#include "GameObject.h"

class Enemy : public GameObject
{
    private:
        sf::Sprite sprite;
    public:
        Enemy(std::string Folder);
        void update(const Scene& scene) override;
        void draw(sf::RenderWindow& window) override;
};
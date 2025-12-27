#include "Enemy.h"

Enemy::Enemy(std::string Folder)
{
    sf::Texture texture;
    std::string path_to_folder = "assets/pp1/" + Folder;
    //load idle texture
    if(!texture.loadFromFile(path_to_folder + "metarig.013-0_0001.png"))
    {
        // Handle error
    }
    sprite.setTexture(texture);
}
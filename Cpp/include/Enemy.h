#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Hittable.h"

class Block;
class Scene;

class Enemy : public Hittable
{
    private:
        sf::Sprite sprite;
        sf::Vector2f velocity;

        sf::Texture idle_texture;
        std::vector<sf::Texture> walk_textures;
        std::vector<sf::Texture> attack_textures;

        // Collider
        float colliderOffsetX;
        float colliderOffsetY;

        // Animation
        int current_animation_frame;
        float animation_timer, animation_speed;

        // Physics
        bool isGrounded;
        float speed, gravity;
        bool facingRight;

        // Attack state
        bool isAttacking;
        size_t attackFrame;
        float attackTimer;
        static constexpr float attackFrameDuration = 0.15f;
        sf::FloatRect attackHitbox;
        float attackCooldownTimer;
        static constexpr float attackCooldown = 1.5f;

        // AI
        float patrolTimer;
        float patrolDirection;
        static constexpr float patrolChangeTime = 2.0f;
        
        // Player detection
        bool seesPlayer;
        float attackDelayTimer;
        static constexpr float attackDelay = 0.5f;

        void apply_gravity(float dt);
        void moveX(float dt, const std::vector<Block*>& blocks);
        void moveY(float dt, const std::vector<Block*>& blocks);
        void updateAnimation(float dt);
        void updateCollider();
        void updateAI(float dt, const Scene& scene);
        void attack(const Scene& scene);
        void setAttackAnimation();

    public:
        Enemy(std::string Folder);
        void update(const Scene& scene) override;
        void draw(sf::RenderWindow& window) override;
        sf::FloatRect getBounds() const { return collider; }
        sf::FloatRect collider;

        enum class EnemyState
        {
            idle,
            walking,
            attacking
        };

    private:
        EnemyState state = EnemyState::idle;
        EnemyState getState();
};
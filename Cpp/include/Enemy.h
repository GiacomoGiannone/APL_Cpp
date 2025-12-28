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

        // Network
        uint32_t enemyId;
        bool isLocallyControlled; // Solo un client controlla l'AI

        void apply_gravity(float dt);
        void moveX(float dt, const std::vector<Block*>& blocks);
        void moveY(float dt, const std::vector<Block*>& blocks);
        void updateAnimation(float dt);
        void updateCollider();
        void updateAI(float dt, const Scene& scene);
        void attack(const Scene& scene);
        void setAttackAnimation();

    public:
        Enemy(std::string Folder, uint32_t id = 0, bool localControl = true);
        void update(const Scene& scene) override;
        void draw(sf::RenderWindow& window) override;
        sf::FloatRect getBounds() const { return collider; }
        sf::FloatRect collider;

        // Network methods
        uint32_t getId() const { return enemyId; }
        void setId(uint32_t id) { enemyId = id; }
        bool isLocalControl() const { return isLocallyControlled; }
        void setLocalControl(bool local) { isLocallyControlled = local; }
        
        // Sync from network
        void syncFromNetwork(float x, float y, float velX, float velY, 
                             bool faceRight, bool grounded, bool attacking, float health);
        
        // Getters for network sync
        sf::Vector2f getPosition() const { return sprite.getPosition(); }
        sf::Vector2f getVelocity() const { return velocity; }
        bool isFacingRight() const { return facingRight; }
        bool getIsGrounded() const { return isGrounded; }
        bool getIsAttacking() const { return isAttacking; }

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
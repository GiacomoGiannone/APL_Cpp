#include "Enemy.h"
#include "Block.h"
#include "Scene.h"
#include "Player.h"
#include <iostream>
#include <cmath>

Enemy::Enemy(std::string Folder)
    : Hittable(50.f), velocity(0.0f, 0.0f), isGrounded(false), speed(80.0f), gravity(200.0f),
      current_animation_frame(0), animation_timer(0.1f), animation_speed(0.1f),
      facingRight(true), isAttacking(false), attackFrame(0), attackTimer(0.f),
      attackCooldownTimer(0.f), patrolTimer(0.f), patrolDirection(1.f),
      seesPlayer(false), attackDelayTimer(0.f)
{
    sf::Texture texture;
    std::string path_to_folder = "assets/pp1/" + Folder + "/";  
    //load idle texture
    if(!idle_texture.loadFromFile(path_to_folder + "Idle.png"))
    {
        std::cerr << "Could not load enemy texture from path " << path_to_folder + "Idle.png" << std::endl;
    }
    //load walk textures
    for(int i = 1; i <= 4; i++)
    {
        if(!texture.loadFromFile(path_to_folder + "Walk_" + std::to_string(i) + ".png"))
        {
            std::cerr << "Could not load enemy walk texture from path " << path_to_folder + "Walk_" + std::to_string(i) + ".png" << std::endl;
        }
        walk_textures.push_back(texture);
    }
    //load attack textures
    if(!texture.loadFromFile(path_to_folder + "A1.png"))
    {
        std::cerr << "Could not load enemy attack texture from path " << path_to_folder + "A1.png" << std::endl;
    }
    attack_textures.push_back(texture);
    if(!texture.loadFromFile(path_to_folder + "A2.png"))
    {
        std::cerr << "Could not load enemy attack texture from path " << path_to_folder + "A2.png" << std::endl;
    }
    attack_textures.push_back(texture);

    // Setup sprite
    sprite.setTexture(idle_texture);
    
    // Dimensioni del personaggio nella texture
    float characterStartX = 41.f;
    float characterStartY = 24.0f;
    float characterWidth = 15.f;
    float characterHeight = 30.f;
    
    // Imposta il textureRect
    sprite.setTextureRect(sf::IntRect(
        characterStartX,
        characterStartY,
        characterWidth,
        characterHeight
    ));
    
    // Imposta l'origine al centro
    sprite.setOrigin(characterWidth / 2.f, characterHeight / 2.f);
    
    // Posizione iniziale
    sprite.setPosition(300.f, 100.f);
    
    // Configura il collider
    collider.width = characterWidth * 0.75f;
    collider.height = characterHeight;
    
    colliderOffsetX = (characterWidth - collider.width) / 2.f;
    colliderOffsetY = characterHeight - collider.height;
    
    updateCollider();
}

void Enemy::apply_gravity(float dt)
{
    velocity.y += gravity * dt;
}

void Enemy::moveX(float dt, const std::vector<Block*>& blocks)
{
    sprite.move(velocity.x * dt, 0.0f);
    updateCollider();
    
    for(const auto& block : blocks)
    {
        if(collider.intersects(block->getBounds()))
        {
            float overlap = 0.f;
            
            if(velocity.x > 0) // Destra
            {
                overlap = (collider.left + collider.width) - block->getBounds().left;
                sprite.move(-overlap, 0.f);
                patrolDirection = -1.f; // Cambia direzione quando colpisce un muro
            }
            else if(velocity.x < 0) // Sinistra
            {
                overlap = block->getBounds().left + block->getBounds().width - collider.left;
                sprite.move(overlap, 0.f);
                patrolDirection = 1.f; // Cambia direzione quando colpisce un muro
            }
            
            updateCollider();
            velocity.x = 0;
            break;
        }
    }
}

void Enemy::moveY(float dt, const std::vector<Block*>& blocks)
{
    sprite.move(0.0f, velocity.y * dt);
    updateCollider();
    
    isGrounded = false;
    
    for(const auto& block : blocks)
    {
        if(collider.intersects(block->getBounds()))
        {
            float overlap = 0.f;
            
            if(velocity.y > 0) // Cadendo
            {
                overlap = (collider.top + collider.height) - block->getBounds().top;
                sprite.move(0.f, -overlap);
                isGrounded = true;
            }
            else if(velocity.y < 0) // Saltando
            {
                overlap = block->getBounds().top + block->getBounds().height - collider.top;
                sprite.move(0.f, overlap);
            }
            
            updateCollider();
            velocity.y = 0;
            break;
        }
    }
}

void Enemy::updateAI(float dt, const Scene& scene)
{
    // Update attack cooldown
    if(attackCooldownTimer > 0.f)
    {
        attackCooldownTimer -= dt;
    }
    
    // Controlla se ci sono player vicini
    seesPlayer = false;
    for(const auto& player : scene.getPlayers())
    {
        if(player->isLocal())
        {
            float distance = std::abs(sprite.getPosition().x - player->getBounds().left);
            float yDistance = std::abs(sprite.getPosition().y - player->getBounds().top);
            
            if(distance < 60.f && yDistance < 30.f)
            {
                seesPlayer = true;
                // Gira verso il player
                facingRight = (player->getBounds().left > sprite.getPosition().x);
                break;
            }
        }
    }
    
    if(seesPlayer)
    {
        // Fermati quando vedi il player
        velocity.x = 0.f;
        
        // Incrementa il timer di attesa
        attackDelayTimer += dt;
        
        // Attacca solo dopo il delay e se il cooldown è finito
        if(attackDelayTimer >= attackDelay && attackCooldownTimer <= 0.f)
        {
            attack(scene);
            attackCooldownTimer = attackCooldown;
            attackDelayTimer = 0.f;
        }
    }
    else
    {
        // Reset del timer quando non vede il player
        attackDelayTimer = 0.f;
        
        // Pattugliamento normale
        patrolTimer += dt;
        
        if(patrolTimer >= patrolChangeTime)
        {
            patrolTimer = 0.f;
            patrolDirection *= -1.f;
        }
        
        velocity.x = speed * patrolDirection;
        facingRight = (patrolDirection > 0);
    }
}

void Enemy::updateAnimation(float dt)
{
    static sf::Texture* lastTexture = nullptr;
    static bool lastFacingRight = true;
    
    // Handle attack animation first
    if(isAttacking)
    {
        attackTimer += dt;
        if(attackTimer >= attackFrameDuration)
        {
            attackTimer = 0.f;
            attackFrame++;
            if(attackFrame >= attack_textures.size())
            {
                isAttacking = false;
                attackFrame = 0;
                lastTexture = nullptr;
            }
        }
        
        if(isAttacking)
        {
            sprite.setTexture(attack_textures[attackFrame]);
            sprite.setTextureRect(sf::IntRect(41, 24, 40, 30));
            sprite.setScale(facingRight ? 1.f : -1.f, 1.f);
            return;
        }
    }
    
    // Determina texture
    sf::Texture* texture = &idle_texture;
    
    if(velocity.x != 0.f && isGrounded)
    {
        // Walking animation
        animation_timer += dt;
        if(animation_timer >= animation_speed)
        {
            animation_timer = 0.f;
            current_animation_frame = (current_animation_frame + 1) % walk_textures.size();
        }
        texture = &walk_textures[current_animation_frame];
    }
    
    // Cambia texture SOLO se necessario
    if(texture != lastTexture || facingRight != lastFacingRight)
    {
        sprite.setTexture(*texture);
        sprite.setTextureRect(sf::IntRect(41, 24, 15, 30));
        
        // Flip
        sprite.setScale(facingRight ? 1.f : -1.f, 1.f);
        
        lastTexture = texture;
        lastFacingRight = facingRight;
    }
}

Enemy::EnemyState Enemy::getState()
{
    if(isAttacking)
        return EnemyState::attacking;
    else if(velocity.x != 0.f)
        return EnemyState::walking;
    else
        return EnemyState::idle;
}

void Enemy::updateCollider()
{
    collider.left = sprite.getPosition().x - (collider.width / 2.f);
    collider.top = sprite.getPosition().y - (collider.height / 2.f);
}

void Enemy::attack(const Scene& scene)
{
    // Calculate attack hitbox position
    attackHitbox.width = 20.f;
    attackHitbox.height = 20.f;
    if(facingRight)
    {
        attackHitbox.left = sprite.getPosition().x + collider.width / 2.f;
        attackHitbox.top = sprite.getPosition().y - 10.f;
    }
    else
    {
        attackHitbox.left = sprite.getPosition().x - collider.width / 2.f - 20.f;
        attackHitbox.top = sprite.getPosition().y - 10.f;
    }
    
    // Controlla se l'attacco colpisce un player
    for(const auto& player : scene.getPlayers())
    {
        if(attackHitbox.intersects(player->getBounds()))
        {
            std::cout << "Enemy attacked a Player!" << std::endl;
            player->takeDamage(10.f);
        }
    }
    
    setAttackAnimation();
}

void Enemy::setAttackAnimation()
{
    if(!isAttacking)
    {
        isAttacking = true;
        attackFrame = 0;
        attackTimer = 0.f;
    }
}

void Enemy::update(const Scene& scene)
{
    float dt = scene.getDt();
    
    // Gestione morte
    if (dying)
    {
        // Ruota lo sprite di 90 gradi (progressivamente o subito)
        sprite.setRotation(90.f);
        updateDeath(dt);
        return; // Non fare altro se sta morendo
    }
    
    auto blocks = scene.getBlocks();
    
    updateAI(dt, scene);
    apply_gravity(dt);
    moveX(dt, blocks);
    moveY(dt, blocks);
    updateAnimation(dt);
}

void Enemy::draw(sf::RenderWindow& window)
{
    // Fade out durante la morte
    if (dying)
    {
        float alpha = 255.f * (1.f - getDeathProgress());
        sprite.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));
    }
    
    window.draw(sprite);
    
    // Non disegnare health bar se sta morendo
    if (dying) return;
    
    // Health bar
    float barWidth = 30.f;
    float barHeight = 4.f;
    float barOffsetY = -20.f;
    
    // Background
    sf::RectangleShape healthBarBg;
    healthBarBg.setSize(sf::Vector2f(barWidth, barHeight));
    healthBarBg.setPosition(sprite.getPosition().x - barWidth / 2.f, 
                            sprite.getPosition().y + barOffsetY);
    healthBarBg.setFillColor(sf::Color(60, 60, 60));
    healthBarBg.setOutlineColor(sf::Color::Black);
    healthBarBg.setOutlineThickness(1.f);
    window.draw(healthBarBg);
    
    // Foreground
    sf::RectangleShape healthBar;
    float healthPercent = getHealthPercent();
    healthBar.setSize(sf::Vector2f(barWidth * healthPercent, barHeight));
    healthBar.setPosition(sprite.getPosition().x - barWidth / 2.f, 
                          sprite.getPosition().y + barOffsetY);
    
    if (healthPercent > 0.6f)
        healthBar.setFillColor(sf::Color(50, 205, 50));
    else if (healthPercent > 0.3f)
        healthBar.setFillColor(sf::Color(255, 165, 0));
    else
        healthBar.setFillColor(sf::Color(220, 20, 60));
    
    window.draw(healthBar);
    
    // Draw attack hitbox when attacking (debug)
    if(isAttacking)
    {
        sf::RectangleShape hitboxRect;
        hitboxRect.setPosition(attackHitbox.left, attackHitbox.top);
        hitboxRect.setSize(sf::Vector2f(attackHitbox.width, attackHitbox.height));
        hitboxRect.setFillColor(sf::Color(255, 0, 0, 100));
        hitboxRect.setOutlineColor(sf::Color::Red);
        hitboxRect.setOutlineThickness(2.f);
        window.draw(hitboxRect);
    }
}
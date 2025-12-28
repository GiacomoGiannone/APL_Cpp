#pragma once
#include <cstdint>

// Tipi di messaggi che possiamo scambiare
enum PacketType : uint32_t 
{
    LOGIN = 1,
    MOVE = 2,
    PLAYER_DISCONNECTED = 3,
    ENEMY_SPAWN = 4,      // Server dice ai client di spawnare un nemico
    ENEMY_UPDATE = 5,     // Aggiornamento posizione/stato nemico
    ENEMY_DAMAGE = 6,     // Un player ha colpito un nemico
    ENEMY_DEATH = 7       // Un nemico è morto
};

// Disabilita il padding automatico del compilatore (fondamentale per comunicare con Go!)
#pragma pack(push, 1)

// 1. L'Intestazione (Testa)
// Ogni pacchetto inizia con questi dati, così sappiamo cos'è e quanto è lungo.
struct PacketHeader 
{
    uint32_t type;       // Che tipo di pacchetto è? (es. MOVE)
    uint32_t packetSize; // Quanto è grande tutto il pacchetto (Header + Dati)?
};

// 2. Il Pacchetto di Movimento (Corpo)
struct PacketMove 
{
    PacketHeader header; // Include sempre l'header
    uint32_t playerId;   // Chi si sta muovendo?
    float x;             // Dove sei?
    float y;
    float velocityX;     // A che velocità? (utile per predizione lato client)
    float velocityY;
    bool isFacingRight;  // Dove guardi?
    bool isGrounded;     // Stai saltando?
    //TODO add from which folder the textures must be rendered
};

// 3. Pacchetto Login (Esempio)
struct PacketLogin 
{
    PacketHeader header;
    uint32_t playerId; // Il server ti risponderà assegnandoti un ID
};

// 4. Pacchetto Spawn Nemico
struct PacketEnemySpawn
{
    PacketHeader header;
    uint32_t enemyId;    // ID univoco del nemico
    float x;             // Posizione spawn
    float y;
    float maxHealth;     // Salute massima
};

// 5. Pacchetto Update Nemico (posizione/stato)
struct PacketEnemyUpdate
{
    PacketHeader header;
    uint32_t enemyId;
    float x;
    float y;
    float velocityX;
    float velocityY;
    bool isFacingRight;
    bool isGrounded;
    bool isAttacking;
    float currentHealth;
};

// 6. Pacchetto Danno Nemico
struct PacketEnemyDamage
{
    PacketHeader header;
    uint32_t enemyId;    // Quale nemico è stato colpito
    uint32_t attackerId; // Chi l'ha colpito
    float damage;        // Quanto danno
};

// 7. Pacchetto Morte Nemico
struct PacketEnemyDeath
{
    PacketHeader header;
    uint32_t enemyId;    // Quale nemico è morto
};

#pragma pack(pop) // Riabilita il padding normale
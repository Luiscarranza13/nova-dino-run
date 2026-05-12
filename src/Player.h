#pragma once
#include "raylib.h"
#include <vector>

enum class PlayerState { RUNNING, JUMPING, DUCKING, DEAD };

struct DustParticle {
    Vector2 pos, vel;
    float life, maxLife;
    float size;
};

class Player {
public:
    static constexpr float GROUND_Y   = 460.0f;
    static constexpr float JUMP_FORCE = -620.0f;
    static constexpr float GRAVITY    = 1500.0f;
    static constexpr float RUN_X      = 90.0f;

    Player();
    void LoadAssets();
    void UnloadAssets();

    void Update(float dt);
    void Draw() const;
    void Jump();
    void Duck(bool ducking);
    void Die();
    void Reset();

    Rectangle GetHitbox() const;
    bool IsAlive()         const { return state != PlayerState::DEAD; }
    bool IsTurboActive()   const { return turboActive; }
    bool HasShield()       const { return shieldActive; }

    void ActivateShield()     { shieldActive = true;     shieldTimer = 6.0f; }
    void ActivateDoubleJump() { doubleJumpActive = true; doubleJumpTimer = 8.0f; }
    void ActivateTurbo()      { turboActive = true;      turboTimer = 5.0f; }
    void UseShield()          { shieldActive = false;    shieldTimer = 0; }

private:
    void SpawnDust();
    void UpdateParticles(float dt);
    void DrawParticles() const;

    float x, y, vy;
    PlayerState state;
    int jumpCount;

    bool  shieldActive,     doubleJumpActive,     turboActive;
    float shieldTimer,      doubleJumpTimer,      turboTimer;
    float shieldPulse;

    float animTimer;
    int   animFrame;
    float deathTimer;

    // Sprites
    Texture2D texRun[4];
    Texture2D texDuck;
    Texture2D texDead;

    std::vector<DustParticle> particles;
    float dustTimer;
};

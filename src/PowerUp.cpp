#include "PowerUp.h"
#include "Player.h"
#include <algorithm>
#include <cmath>

static constexpr float SW = 1000.0f;
static constexpr float GY = Player::GROUND_Y;

PowerUpManager::PowerUpManager() {
    texShield = texJump = texTurbo = {};
    Reset();
}

void PowerUpManager::LoadAssets() {
    texShield = LoadTexture("assets/images/powerup_shield.png");
    texJump   = LoadTexture("assets/images/powerup_jump.png");
    texTurbo  = LoadTexture("assets/images/powerup_turbo.png");
}

void PowerUpManager::UnloadAssets() {
    UnloadTexture(texShield);
    UnloadTexture(texJump);
    UnloadTexture(texTurbo);
}

void PowerUpManager::Reset() {
    powerups.clear();
    spawnTimer = 0;
}

void PowerUpManager::Update(float dt, float speed) {
    spawnTimer += dt;
    if (spawnTimer >= 9.0f) {
        spawnTimer = 0;
        PowerUp p;
        p.x = SW;
        p.y = GY - (float)GetRandomValue(40, 100);
        p.type = static_cast<PowerUpType>(GetRandomValue(0, 2));
        p.active = true;
        p.animTimer = 0;
        powerups.push_back(p);
    }

    for (auto& p : powerups) {
        p.x -= speed * dt;
        p.animTimer += dt;
        if (p.x + 16 < 0) p.active = false;
    }

    powerups.erase(
        std::remove_if(powerups.begin(), powerups.end(),
            [](const PowerUp& p){ return !p.active; }),
        powerups.end());
}

void PowerUpManager::Draw() const {
    for (const auto& p : powerups) {
        float bob = sinf(p.animTimer * 3.5f) * 6.0f;
        float rot = sinf(p.animTimer * 2.0f) * 15.0f;
        float py  = p.y + bob;

        Texture2D tex;
        Color glow;
        switch (p.type) {
            case PowerUpType::SHIELD:      tex = texShield; glow = {0,180,255,40};  break;
            case PowerUpType::DOUBLE_JUMP: tex = texJump;   glow = {160,0,240,40};  break;
            case PowerUpType::TURBO:       tex = texTurbo;  glow = {255,140,0,40};  break;
        }

        // Halo de brillo
        float pulse = 1.0f + 0.15f * sinf(p.animTimer * 5.0f);
        DrawCircle((int)p.x, (int)py, 22*pulse, glow);

        // Sprite
        DrawTexturePro(tex,
            { 0, 0, (float)tex.width, (float)tex.height },
            { p.x, py, 36, 36 },
            { 18, 18 }, rot, WHITE);
    }
}

int PowerUpManager::CheckCollect(Rectangle playerBox) {
    for (auto& p : powerups) {
        if (p.active && CheckCollisionRecs(playerBox, p.GetHitbox())) {
            p.active = false;
            return static_cast<int>(p.type);
        }
    }
    return -1;
}

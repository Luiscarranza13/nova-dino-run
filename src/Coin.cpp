#include "Coin.h"
#include "Player.h"
#include <algorithm>
#include <cmath>

static constexpr float SW = 1000.0f;
static constexpr float GY = Player::GROUND_Y;

CoinManager::CoinManager() {
    for (int i = 0; i < 4; i++) texFrames[i] = {};
    Reset();
}

void CoinManager::LoadAssets() {
    for (int i = 0; i < 4; i++)
        texFrames[i] = LoadTexture(TextFormat("assets/images/coin%d.png", i));
}

void CoinManager::UnloadAssets() {
    for (int i = 0; i < 4; i++) UnloadTexture(texFrames[i]);
}

void CoinManager::Reset() {
    coins.clear();
    spawnTimer = 0;
}

void CoinManager::Update(float dt, float speed) {
    spawnTimer += dt;
    if (spawnTimer >= 1.6f) {
        spawnTimer = 0;
        // Spawnear en línea o individual
        int count = GetRandomValue(0,2) == 0 ? 3 : 1;
        float baseY = GY - (float)GetRandomValue(20, 110);
        for (int i = 0; i < count; i++) {
            Coin c;
            c.x = SW + i * 40.0f;
            c.y = baseY;
            c.active = true;
            c.animTimer = 0;
            c.animFrame = 0;
            c.bobTimer = (float)i * 0.3f;
            coins.push_back(c);
        }
    }

    for (auto& c : coins) {
        c.x -= speed * dt;
        c.bobTimer  += dt;
        c.animTimer += dt;
        if (c.animTimer >= 0.08f) {
            c.animTimer = 0;
            c.animFrame = (c.animFrame + 1) % 4;
        }
        if (c.x + 12 < 0) c.active = false;
    }

    coins.erase(
        std::remove_if(coins.begin(), coins.end(),
            [](const Coin& c){ return !c.active; }),
        coins.end());
}

void CoinManager::Draw() const {
    for (const auto& c : coins) {
        float bob = sinf(c.bobTimer * 5.0f) * 5.0f;
        float cy = c.y + bob;
        Texture2D tex = texFrames[c.animFrame];
        float s = 1.1f + 0.05f * sinf(c.bobTimer * 5.0f);
        float sz = 28.0f * s;
        // Brillo
        DrawCircle((int)c.x, (int)cy, sz*0.7f, { 255, 215, 0, 30 });
        // Sprite
        DrawTexturePro(tex,
            { 0, 0, (float)tex.width, (float)tex.height },
            { c.x - sz/2, cy - sz/2, sz, sz },
            { 0, 0 }, 0, WHITE);
    }
}

int CoinManager::CheckCollect(Rectangle playerBox) {
    int n = 0;
    for (auto& c : coins) {
        if (c.active && CheckCollisionRecs(playerBox, c.GetHitbox())) {
            c.active = false;
            n++;
        }
    }
    return n;
}

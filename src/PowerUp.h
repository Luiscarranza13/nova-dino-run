#pragma once
#include "raylib.h"
#include <vector>

enum class PowerUpType { SHIELD, DOUBLE_JUMP, TURBO };

struct PowerUp {
    float x, y;
    PowerUpType type;
    bool  active;
    float animTimer;

    Rectangle GetHitbox() const { return { x - 16, y - 16, 32, 32 }; }
};

class PowerUpManager {
public:
    PowerUpManager();
    void LoadAssets();
    void UnloadAssets();

    void Update(float dt, float speed);
    void Draw() const;
    void Reset();
    int  CheckCollect(Rectangle playerBox);

private:
    std::vector<PowerUp> powerups;
    float spawnTimer;
    Texture2D texShield, texJump, texTurbo;
};

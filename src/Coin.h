#pragma once
#include "raylib.h"
#include <vector>

struct Coin {
    float x, y;
    bool  active;
    float animTimer;
    int   animFrame;
    float bobTimer;

    Rectangle GetHitbox() const { return { x - 12, y - 12, 24, 24 }; }
};

class CoinManager {
public:
    CoinManager();
    void LoadAssets();
    void UnloadAssets();

    void Update(float dt, float speed);
    void Draw() const;
    void Reset();
    int  CheckCollect(Rectangle playerBox);

private:
    std::vector<Coin> coins;
    float spawnTimer;
    Texture2D texFrames[4];
};

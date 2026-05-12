#pragma once
#include "raylib.h"
#include <vector>

enum class ObstacleType { CACTUS, BIRD, ROCK };

struct Obstacle {
    float x, y, w, h;
    ObstacleType type;
    bool  active;
    float animTimer;
    int   animFrame;
    float scale;

    Rectangle GetHitbox() const {
        return { x + w*0.1f, y + h*0.1f, w*0.8f, h*0.8f };
    }
};

class ObstacleManager {
public:
    ObstacleManager();
    void LoadAssets();
    void UnloadAssets();

    void Update(float dt, float speed);
    void Draw() const;
    void Reset();
    bool CheckCollision(Rectangle playerBox) const;

private:
    void Spawn(float speed);

    std::vector<Obstacle> obstacles;
    float spawnTimer, spawnInterval;

    Texture2D texCactus, texRock;
    Texture2D texBird[2];
};

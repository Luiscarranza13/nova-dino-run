#include "Obstacle.h"
#include "Player.h"
#include <algorithm>

static constexpr float SW = 1000.0f;
static constexpr float GY = Player::GROUND_Y;

ObstacleManager::ObstacleManager() {
    texCactus = texRock = texBird[0] = texBird[1] = {};
    Reset();
}

void ObstacleManager::LoadAssets() {
    texCactus  = LoadTexture("assets/images/cactus.png");
    texRock    = LoadTexture("assets/images/rock.png");
    texBird[0] = LoadTexture("assets/images/bird0.png");
    texBird[1] = LoadTexture("assets/images/bird1.png");
}

void ObstacleManager::UnloadAssets() {
    UnloadTexture(texCactus);
    UnloadTexture(texRock);
    UnloadTexture(texBird[0]);
    UnloadTexture(texBird[1]);
}

void ObstacleManager::Reset() {
    obstacles.clear();
    spawnTimer = 0;
    spawnInterval = 2.2f;
}

void ObstacleManager::Spawn(float speed) {
    ObstacleType type = static_cast<ObstacleType>(GetRandomValue(0, 2));
    Obstacle o;
    o.active = true;
    o.type = type;
    o.animTimer = 0;
    o.animFrame = 0;
    o.scale = 1.0f + GetRandomValue(-10, 20) / 100.0f;

    switch (type) {
        case ObstacleType::CACTUS:
            o.w = 48*o.scale; o.h = 80*o.scale;
            o.x = SW; o.y = GY - o.h;
            break;
        case ObstacleType::BIRD:
            o.w = 64; o.h = 40;
            o.x = SW;
            o.y = GY - (float)GetRandomValue(70, 150);
            break;
        case ObstacleType::ROCK:
            o.w = 56*o.scale; o.h = 44*o.scale;
            o.x = SW; o.y = GY - o.h;
            break;
    }
    obstacles.push_back(o);
}

void ObstacleManager::Update(float dt, float speed) {
    spawnTimer += dt;
    spawnInterval = std::max(0.7f, 2.4f - speed * 0.014f);

    if (spawnTimer >= spawnInterval) {
        spawnTimer = 0;
        Spawn(speed);
        // A veces spawnear 2 seguidos
        if (speed > 450 && GetRandomValue(0,2) == 0) {
            spawnTimer = -0.5f;
        }
    }

    for (auto& o : obstacles) {
        o.x -= speed * dt;
        o.animTimer += dt;
        if (o.type == ObstacleType::BIRD && o.animTimer >= 0.15f) {
            o.animTimer = 0;
            o.animFrame = 1 - o.animFrame;
        }
        if (o.x + o.w < -20) o.active = false;
    }

    obstacles.erase(
        std::remove_if(obstacles.begin(), obstacles.end(),
            [](const Obstacle& o){ return !o.active; }),
        obstacles.end());
}

void ObstacleManager::Draw() const {
    for (const auto& o : obstacles) {
        Texture2D tex;
        switch (o.type) {
            case ObstacleType::CACTUS: tex = texCactus; break;
            case ObstacleType::ROCK:   tex = texRock;   break;
            case ObstacleType::BIRD:   tex = texBird[o.animFrame]; break;
        }
        // Sombra
        DrawEllipse((int)(o.x + o.w/2), (int)GY + 4,
                    (int)(o.w*0.4f), 5, { 0,0,0,80 });
        // Sprite
        DrawTexturePro(tex,
            { 0, 0, (float)tex.width, (float)tex.height },
            { o.x, o.y, o.w, o.h },
            { 0, 0 }, 0, WHITE);
    }
}

bool ObstacleManager::CheckCollision(Rectangle playerBox) const {
    for (const auto& o : obstacles)
        if (CheckCollisionRecs(playerBox, o.GetHitbox())) return true;
    return false;
}

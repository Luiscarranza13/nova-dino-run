#pragma once
#include "raylib.h"
#include "Player.h"
#include "Obstacle.h"
#include "Coin.h"
#include "PowerUp.h"
#include "UI.h"
#include <vector>

struct CollectFX {
    Vector2 pos;
    float   life, maxLife;
    int     value;
    Color   color;
};

class Game {
public:
    static constexpr int   SCREEN_W  = 1000;
    static constexpr int   SCREEN_H  = 500;
    static constexpr float BASE_SPEED = 300.0f;

    Game();
    ~Game();
    void Run();

private:
    void HandleInput();
    void Update(float dt);
    void Draw();
    void Reset();
    void LoadHiScore();
    void SaveHiScore();
    void DrawBackground() const;
    void DrawGround() const;
    void DrawCollectFX() const;
    void SpawnCollectFX(float x, float y, int val, Color c);

    GameState state;
    Player    player;
    ObstacleManager obstacles;
    CoinManager     coins;
    PowerUpManager  powerups;
    UI              ui;

    int   score, coinCount, hiScore;
    float speed, gameTime;

    // Parallax
    float bgScroll;
    float groundScroll;
    float cloudScroll[3];
    float cloudY[3];

    // Efectos
    std::vector<CollectFX> collectFX;
    float flashTimer;   // pantalla blanca al morir
    float scoreAnim;    // animación de puntaje

    // Texturas de fondo
    Texture2D texBg, texGround, texCloud;

    // Audio
    Sound sndJump, sndHit, sndCoin, sndPowerup;
    Music music;
    bool  musicLoaded;
};

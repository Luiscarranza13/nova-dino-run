#include "Game.h"
#include <cstdio>
#include <cmath>
#include <algorithm>

static constexpr float GY = Player::GROUND_Y;

static Sound LoadSafe(const char* p) {
    return FileExists(p) ? LoadSound(p) : Sound{};
}
static bool SndOk(Sound s) { return s.frameCount > 0; }

// ── Constructor ──────────────────────────────────────────────────────────────
Game::Game() {
    InitWindow(SCREEN_W, SCREEN_H, "Nova Dino Run | NovaTec");
    SetTargetFPS(60);
    InitAudioDevice();

    // Cargar assets de todos los sistemas
    player.LoadAssets();
    obstacles.LoadAssets();
    coins.LoadAssets();
    powerups.LoadAssets();
    ui.LoadAssets();

    texBg     = LoadTexture("assets/images/background.png");
    texGround = LoadTexture("assets/images/ground.png");
    texCloud  = LoadTexture("assets/images/cloud.png");

    sndJump    = LoadSafe("assets/sounds/jump.wav");
    sndHit     = LoadSafe("assets/sounds/hit.wav");
    sndCoin    = LoadSafe("assets/sounds/coin.wav");
    sndPowerup = LoadSafe("assets/sounds/powerup.wav");

    musicLoaded = false;
    if (FileExists("assets/sounds/music.wav")) {
        music = LoadMusicStream("assets/sounds/music.wav");
        SetMusicVolume(music, 0.5f);
        musicLoaded = true;
    }

    LoadHiScore();
    Reset();
}

Game::~Game() {
    player.UnloadAssets();
    obstacles.UnloadAssets();
    coins.UnloadAssets();
    powerups.UnloadAssets();
    ui.UnloadAssets();

    UnloadTexture(texBg);
    UnloadTexture(texGround);
    UnloadTexture(texCloud);

    if (SndOk(sndJump))    UnloadSound(sndJump);
    if (SndOk(sndHit))     UnloadSound(sndHit);
    if (SndOk(sndCoin))    UnloadSound(sndCoin);
    if (SndOk(sndPowerup)) UnloadSound(sndPowerup);
    if (musicLoaded)       UnloadMusicStream(music);

    CloseAudioDevice();
    CloseWindow();
}

// ── Reset ────────────────────────────────────────────────────────────────────
void Game::Reset() {
    player.Reset();
    obstacles.Reset();
    coins.Reset();
    powerups.Reset();
    collectFX.clear();

    score = coinCount = 0;
    speed = BASE_SPEED;
    gameTime = 0;
    bgScroll = groundScroll = 0;
    flashTimer = 0;
    scoreAnim  = 0;

    for (int i = 0; i < 3; i++) {
        cloudScroll[i] = (float)(i * 340);
        cloudY[i] = 60.0f + i * 50.0f;
    }

    state = GameState::MENU;
}

void Game::LoadHiScore() {
    FILE* f = fopen("hiscore.dat", "rb");
    if (f) { fread(&hiScore, sizeof(int), 1, f); fclose(f); }
    else hiScore = 0;
}

void Game::SaveHiScore() {
    FILE* f = fopen("hiscore.dat", "wb");
    if (f) { fwrite(&hiScore, sizeof(int), 1, f); fclose(f); }
}

// ── Input ────────────────────────────────────────────────────────────────────
void Game::HandleInput() {
    switch (state) {
        case GameState::MENU:
            if (IsKeyPressed(KEY_SPACE)) {
                state = GameState::PLAYING;
                if (musicLoaded) PlayMusicStream(music);
            }
            break;
        case GameState::PLAYING:
            if (IsKeyPressed(KEY_SPACE)) {
                player.Jump();
                if (SndOk(sndJump)) PlaySound(sndJump);
            }
            player.Duck(IsKeyDown(KEY_DOWN));
            if (IsKeyPressed(KEY_P)) state = GameState::PAUSED;
            break;
        case GameState::PAUSED:
            if (IsKeyPressed(KEY_P)) state = GameState::PLAYING;
            break;
        case GameState::GAME_OVER:
            if (IsKeyPressed(KEY_R)) {
                Reset();
                state = GameState::PLAYING;
                if (musicLoaded) PlayMusicStream(music);
            }
            break;
    }
}

// ── Efectos de colección ─────────────────────────────────────────────────────
void Game::SpawnCollectFX(float x, float y, int val, Color c) {
    collectFX.push_back({ {x, y}, 0.8f, 0.8f, val, c });
}

void Game::DrawCollectFX() const {
    for (const auto& fx : collectFX) {
        float t = 1.0f - fx.life / fx.maxLife;
        float alpha = (1.0f - t) * 255.0f;
        float fy = fx.pos.y - t * 40.0f;
        Color c = { fx.color.r, fx.color.g, fx.color.b, (unsigned char)alpha };
        DrawText(TextFormat("+%d", fx.value), (int)fx.pos.x, (int)fy, 20, c);
    }
}

// ── Update ───────────────────────────────────────────────────────────────────
void Game::Update(float dt) {
    // Actualizar FX siempre
    for (auto& fx : collectFX) fx.life -= dt;
    collectFX.erase(
        std::remove_if(collectFX.begin(), collectFX.end(),
            [](const CollectFX& f){ return f.life <= 0; }),
        collectFX.end());

    if (state == GameState::PAUSED) return;
    if (state == GameState::GAME_OVER) {
        flashTimer = std::max(0.0f, flashTimer - dt);
        player.Update(dt);
        return;
    }
    if (state == GameState::MENU) return;

    if (musicLoaded) UpdateMusicStream(music);

    gameTime += dt;
    speed = BASE_SPEED + gameTime * 13.0f;
    if (player.IsTurboActive()) speed *= 1.55f;
    score = (int)(gameTime * 10.0f) + coinCount * 5;

    // Scroll de fondo
    float bgSpeed = speed * 0.08f;
    bgScroll += bgSpeed * dt;
    if (bgScroll >= texBg.width) bgScroll -= texBg.width;

    groundScroll += speed * dt;
    if (groundScroll >= texGround.width) groundScroll -= texGround.width;

    for (int i = 0; i < 3; i++) {
        cloudScroll[i] += speed * (0.08f + i * 0.04f) * dt;
        if (cloudScroll[i] > SCREEN_W + 120) cloudScroll[i] = -120;
    }

    player.Update(dt);
    obstacles.Update(dt, speed);
    coins.Update(dt, speed);
    powerups.Update(dt, speed);

    Rectangle pBox = player.GetHitbox();

    // Colisión obstáculos
    if (obstacles.CheckCollision(pBox)) {
        bool hadShield = player.HasShield();
        player.Die();
        if (!player.IsAlive()) {
            if (SndOk(sndHit)) PlaySound(sndHit);
            if (score > hiScore) { hiScore = score; SaveHiScore(); }
            state = GameState::GAME_OVER;
            flashTimer = 0.3f;
            if (musicLoaded) StopMusicStream(music);
        }
    }

    // Recoger monedas
    int col = coins.CheckCollect(pBox);
    if (col > 0) {
        coinCount += col;
        if (SndOk(sndCoin)) PlaySound(sndCoin);
        SpawnCollectFX(pBox.x + pBox.width/2, pBox.y, col, { 255, 210, 0, 255 });
    }

    // Recoger power-ups
    int pu = powerups.CheckCollect(pBox);
    if (pu >= 0) {
        if (SndOk(sndPowerup)) PlaySound(sndPowerup);
        Color fxc;
        switch (static_cast<PowerUpType>(pu)) {
            case PowerUpType::SHIELD:
                player.ActivateShield();
                fxc = { 0, 180, 255, 255 };
                break;
            case PowerUpType::DOUBLE_JUMP:
                player.ActivateDoubleJump();
                fxc = { 160, 0, 240, 255 };
                break;
            case PowerUpType::TURBO:
                player.ActivateTurbo();
                fxc = { 255, 140, 0, 255 };
                break;
        }
        SpawnCollectFX(pBox.x + pBox.width/2, pBox.y - 20, 0, fxc);
    }
}

// ── Fondo ────────────────────────────────────────────────────────────────────
void Game::DrawBackground() const {
    if (texBg.id > 0) {
        float bw = (float)texBg.width;
        float bh = (float)texBg.height;
        // Escalar para que el fondo llegue exactamente hasta GROUND_Y
        float scale = GY / bh;
        float x0 = -bgScroll;
        while (x0 < SCREEN_W) {
            DrawTextureEx(texBg, { x0, 0 }, 0, scale, WHITE);
            x0 += bw * scale;
        }
    } else {
        DrawRectangleGradientV(0, 0, SCREEN_W, (int)GY,
            { 8, 12, 35, 255 }, { 18, 28, 65, 255 });
    }

    // Nubes
    if (texCloud.id > 0) {
        for (int i = 0; i < 3; i++) {
            float alpha = 180.0f - i * 40.0f;
            DrawTexture(texCloud,
                (int)(cloudScroll[i] - texCloud.width),
                (int)cloudY[i],
                { 255, 255, 255, (unsigned char)alpha });
        }
    }
}

void Game::DrawGround() const {
    if (texGround.id > 0) {
        int tw = texGround.width;
        float x0 = -groundScroll;
        while (x0 < SCREEN_W) {
            DrawTexture(texGround, (int)x0, (int)GY, WHITE);
            x0 += tw;
        }
    } else {
        DrawRectangle(0, (int)GY, SCREEN_W, 8, { 0, 200, 120, 255 });
        DrawRectangle(0, (int)GY + 8, SCREEN_W, SCREEN_H - (int)GY - 8,
                      { 10, 25, 15, 255 });
    }
}

// ── Draw ─────────────────────────────────────────────────────────────────────
void Game::Draw() {
    BeginDrawing();
    ClearBackground({ 8, 12, 35, 255 });

    DrawBackground();
    DrawGround();

    if (state == GameState::MENU) {
        ui.DrawMenu((float)GetTime());
    } else {
        obstacles.Draw();
        coins.Draw();
        powerups.Draw();
        player.Draw();
        DrawCollectFX();

        ui.DrawHUD(score, coinCount, hiScore, speed,
                   player.HasShield(), false, player.IsTurboActive(),
                   0, 0, 0);

        if (state == GameState::PAUSED)
            ui.DrawPause();
        if (state == GameState::GAME_OVER)
            ui.DrawGameOver(score, coinCount, hiScore, (float)GetTime());
    }

    // Flash de muerte
    if (flashTimer > 0) {
        float alpha = (flashTimer / 0.3f) * 180.0f;
        DrawRectangle(0, 0, SCREEN_W, SCREEN_H,
                      { 255, 80, 80, (unsigned char)alpha });
    }

    EndDrawing();
}

// ── Loop ─────────────────────────────────────────────────────────────────────
void Game::Run() {
    while (!WindowShouldClose()) {
        HandleInput();
        Update(GetFrameTime());
        Draw();
    }
}

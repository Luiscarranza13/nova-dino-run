#include "Player.h"
#include <cmath>
#include <algorithm>

Player::Player() {
    for (int i = 0; i < 4; i++) texRun[i] = {};
    texDuck = texDead = {};
    Reset();
}

void Player::LoadAssets() {
    for (int i = 0; i < 4; i++)
        texRun[i] = LoadTexture(TextFormat("assets/images/dino_run%d.png", i));
    texDuck = LoadTexture("assets/images/dino_duck.png");
    texDead = LoadTexture("assets/images/dino_dead.png");
}

void Player::UnloadAssets() {
    for (int i = 0; i < 4; i++) UnloadTexture(texRun[i]);
    UnloadTexture(texDuck);
    UnloadTexture(texDead);
}

void Player::Reset() {
    x = RUN_X; y = GROUND_Y; vy = 0;
    state = PlayerState::RUNNING;
    jumpCount = 0;
    shieldActive = doubleJumpActive = turboActive = false;
    shieldTimer = doubleJumpTimer = turboTimer = 0;
    shieldPulse = 0;
    animTimer = 0; animFrame = 0;
    deathTimer = 0;
    particles.clear();
    dustTimer = 0;
}

void Player::Jump() {
    if (!IsAlive()) return;
    int maxJumps = doubleJumpActive ? 2 : 1;
    if (jumpCount < maxJumps) {
        vy = JUMP_FORCE;
        state = PlayerState::JUMPING;
        jumpCount++;
        SpawnDust();
    }
}

void Player::Duck(bool ducking) {
    if (!IsAlive() || state == PlayerState::JUMPING) return;
    state = ducking ? PlayerState::DUCKING : PlayerState::RUNNING;
}

void Player::Die() {
    if (shieldActive) { UseShield(); return; }
    state = PlayerState::DEAD;
    deathTimer = 0;
}

void Player::SpawnDust() {
    for (int i = 0; i < 6; i++) {
        DustParticle p;
        p.pos  = { x + (float)GetRandomValue(-10, 10), y };
        p.vel  = { (float)GetRandomValue(-60, -20), (float)GetRandomValue(-80, -20) };
        p.life = p.maxLife = (float)GetRandomValue(20, 40) / 100.0f;
        p.size = (float)GetRandomValue(3, 7);
        particles.push_back(p);
    }
}

void Player::UpdateParticles(float dt) {
    for (auto& p : particles) {
        p.pos.x += p.vel.x * dt;
        p.pos.y += p.vel.y * dt;
        p.vel.y += 200.0f * dt;
        p.life  -= dt;
    }
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
            [](const DustParticle& p){ return p.life <= 0; }),
        particles.end());
}

void Player::Update(float dt) {
    if (!IsAlive()) { deathTimer += dt; UpdateParticles(dt); return; }

    auto tick = [&](bool& a, float& t){ if(a){ t-=dt; if(t<=0) a=false; } };
    tick(shieldActive, shieldTimer);
    tick(doubleJumpActive, doubleJumpTimer);
    tick(turboActive, turboTimer);
    shieldPulse += dt * 4.0f;

    if (state == PlayerState::JUMPING || y < GROUND_Y) {
        vy += GRAVITY * dt;
        y  += vy * dt;
        if (y >= GROUND_Y) {
            y = GROUND_Y; vy = 0;
            jumpCount = 0;
            state = PlayerState::RUNNING;
            SpawnDust();
        }
    }

    if (state == PlayerState::RUNNING) {
        animTimer += dt;
        if (animTimer >= 0.10f) { animTimer = 0; animFrame = (animFrame + 1) % 4; }
        dustTimer += dt;
        if (dustTimer >= 0.15f) { dustTimer = 0; SpawnDust(); }
    }

    UpdateParticles(dt);
}

void Player::DrawParticles() const {
    for (const auto& p : particles) {
        float alpha = p.life / p.maxLife;
        Color c = { 0, 200, 120, (unsigned char)(alpha * 160) };
        DrawCircleV(p.pos, p.size * alpha, c);
    }
}

void Player::Draw() const {
    DrawParticles();

    bool ducking = (state == PlayerState::DUCKING);
    bool dead    = (state == PlayerState::DEAD);

    // Seleccionar textura
    Texture2D tex;
    if (dead)         tex = texDead;
    else if (ducking) tex = texDuck;
    else              tex = texRun[animFrame];

    float tw = (float)tex.width;
    float th = (float)tex.height;
    float scale = ducking ? 0.9f : 1.0f;
    float dw = tw * scale, dh = th * scale;

    // Sombra en el suelo
    float shadowAlpha = dead ? 80.0f : 120.0f;
    DrawEllipse((int)x, (int)GROUND_Y + 4, (int)(dw*0.4f), 5,
                { 0, 0, 0, (unsigned char)shadowAlpha });

    // Efecto muerte: rebotar y rotar
    float drawY = y - dh;
    float rotation = 0;
    if (dead) {
        drawY -= fabsf(sinf(deathTimer * 8.0f)) * 20.0f * expf(-deathTimer * 3.0f);
        rotation = deathTimer * 180.0f;
    }

    // Turbo: trail de color naranja
    if (turboActive) {
        for (int i = 1; i <= 4; i++) {
            float alpha = (5.0f - i) / 5.0f * 120.0f;
            DrawTexturePro(tex,
                { 0, 0, tw, th },
                { x - dw/2 - i*8.0f, drawY, dw, dh },
                { 0, 0 }, 0,
                { 255, 140, 0, (unsigned char)alpha });
        }
    }

    // Sprite principal — origin {0,0} para que dest.y sea la esquina superior
    DrawTexturePro(tex,
        { 0, 0, tw, th },
        { x - dw/2, drawY, dw, dh },
        { 0, 0 }, rotation,
        WHITE);

    // Escudo
    if (shieldActive) {
        float pulse = 1.0f + 0.08f * sinf(shieldPulse);
        float sr = (dw > dh ? dw : dh) * 0.65f * pulse;
        DrawCircleLines((int)x, (int)(y - dh/2), sr,     { 0, 200, 255, 200 });
        DrawCircleLines((int)x, (int)(y - dh/2), sr-3,   { 0, 200, 255, 100 });
        // Brillo
        DrawCircle((int)x, (int)(y - dh/2), sr, { 0, 200, 255, 20 });
    }

    // Indicador doble salto
    if (doubleJumpActive && state == PlayerState::JUMPING) {
        DrawText("x2", (int)(x + 20), (int)(y - dh - 20), 14, { 180, 0, 255, 220 });
    }
}

Rectangle Player::GetHitbox() const {
    bool ducking = (state == PlayerState::DUCKING);
    float w = ducking ? 52.0f : 32.0f;
    float h = ducking ? 22.0f : 58.0f;
    return { x - w*0.4f, y - h, w*0.8f, h*0.85f };
}

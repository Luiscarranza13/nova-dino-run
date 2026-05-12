#include "UI.h"
#include <cmath>

static constexpr int SW = 1000;
static constexpr int SH = 500;

static const Color NT_GREEN  = { 0, 200, 120, 255 };
static const Color NT_CYAN   = { 0, 240, 200, 255 };
static const Color NT_DARK   = { 15, 20, 35, 255 };
static const Color NT_PURPLE = { 160, 0, 240, 255 };
static const Color NT_ORANGE = { 255, 140, 0, 255 };
static const Color COIN_GOLD = { 255, 210, 0, 255 };

// DrawRectangleRoundedLines en Raylib 5: (rec, roundness, segments, lineThick, color)
static void RRL(Rectangle r, float rnd, int seg, Color c, float thick = 1.5f) {
    DrawRectangleRoundedLines(r, rnd, seg, thick, c);
}

void UI::LoadAssets() {
    if (FileExists("assets/fonts/font.ttf")) {
        fontBig = LoadFontEx("assets/fonts/font.ttf", 64, nullptr, 0);
        fontMed = LoadFontEx("assets/fonts/font.ttf", 28, nullptr, 0);
        fontsLoaded = true;
    }
}

void UI::UnloadAssets() {
    if (fontsLoaded) { UnloadFont(fontBig); UnloadFont(fontMed); }
}

static void DrawTextC(Font f, bool loaded, const char* txt, float x, float y,
                       float size, float spacing, Color c) {
    if (loaded) DrawTextEx(f, txt, {x, y}, size, spacing, c);
    else        DrawText(txt, (int)x, (int)y, (int)size, c);
}

static int MeasureW(Font f, bool loaded, const char* txt, float size, float sp) {
    if (loaded) return (int)MeasureTextEx(f, txt, size, sp).x;
    return MeasureText(txt, (int)size);
}

void UI::DrawPowerBar(float x, float y, float t, float maxT, Color c, const char* label) const {
    float pct = (maxT > 0) ? t / maxT : 0;
    DrawRectangleRounded({ x, y, 80, 10 }, 0.5f, 4, { 40, 40, 60, 200 });
    if (pct > 0) DrawRectangleRounded({ x, y, 80*pct, 10 }, 0.5f, 4, c);
    DrawText(label, (int)x, (int)(y - 14), 11, c);
}

void UI::DrawMenu(float time) const {
    DrawRectangleRounded({ (float)(SW/2-220), 80, 440, 320 }, 0.1f, 8, { 10, 15, 40, 200 });
    RRL({ (float)(SW/2-220), 80, 440, 320 }, 0.1f, 8, NT_GREEN);

    const char* title = "NOVA DINO RUN";
    float pulse = 1.0f + 0.04f * sinf(time * 2.5f);
    int tw = MeasureW(fontBig, fontsLoaded, title, 52*pulse, 3);
    DrawTextC(fontBig, fontsLoaded, title, (float)(SW/2 - tw/2 + 3), 113, 52*pulse, 3, { 0, 100, 60, 120 });
    DrawTextC(fontBig, fontsLoaded, title, (float)(SW/2 - tw/2), 110, 52*pulse, 3, NT_GREEN);

    const char* sub = "by NovaTec";
    int sw2 = MeasureW(fontMed, fontsLoaded, sub, 20, 2);
    DrawTextC(fontMed, fontsLoaded, sub, (float)(SW/2 - sw2/2), 172, 20, 2, NT_CYAN);

    struct { const char* key; const char* desc; } controls[] = {
        {"ESPACIO", "Saltar / Doble salto"},
        {"ABAJO",   "Agacharse"},
        {"P",       "Pausar"},
    };
    for (int i = 0; i < 3; i++) {
        float cy = 220.0f + i * 32;
        DrawRectangleRounded({ (float)(SW/2-160), cy, 60, 22 }, 0.4f, 4, { 0, 200, 120, 60 });
        DrawText(controls[i].key,  SW/2-155, (int)cy+4, 14, NT_GREEN);
        DrawText(controls[i].desc, SW/2- 88, (int)cy+4, 14, WHITE);
    }

    float bp = 1.0f + 0.06f * sinf(time * 3.0f);
    float bw = 220*bp, bh = 48*bp;
    DrawRectangleRounded({ (float)(SW/2 - bw/2), 340, bw, bh }, 0.4f, 8, NT_GREEN);
    RRL({ (float)(SW/2 - bw/2), 340, bw, bh }, 0.4f, 8, NT_CYAN);
    const char* btn = "PRESIONA ESPACIO";
    DrawText(btn, SW/2 - MeasureText(btn, 16)/2, 364, 16, NT_DARK);
}

void UI::DrawHUD(int score, int coins, int hiScore, float speed,
                 bool shield, bool dj, bool turbo,
                 float shieldT, float djT, float turboT) const {
    DrawRectangleRounded({ 10, 8, 200, 56 }, 0.3f, 4, { 10, 15, 35, 180 });
    DrawText(TextFormat("SCORE %06d", score), 18, 14, 20, WHITE);
    DrawText(TextFormat("BEST  %06d", hiScore), 18, 38, 16, NT_CYAN);

    DrawRectangleRounded({ (float)(SW-130), 8, 120, 36 }, 0.3f, 4, { 10,15,35,180 });
    DrawCircle(SW-110, 26, 9, COIN_GOLD);
    DrawText(TextFormat("x%d", coins), SW-96, 17, 20, COIN_GOLD);
    DrawText(TextFormat("%.0f km/h", speed), SW-130, 50, 14, { 160,160,180,200 });

    float barX = 10;
    if (shield) { DrawPowerBar(barX, SH-30, shieldT, 6.0f, { 0,180,255,255 }, "SHIELD");  barX += 95; }
    if (dj)     { DrawPowerBar(barX, SH-30, djT,     8.0f, NT_PURPLE,          "x2 JUMP"); barX += 95; }
    if (turbo)  { DrawPowerBar(barX, SH-30, turboT,  5.0f, NT_ORANGE,          "TURBO");   }
}

void UI::DrawPause() const {
    DrawRectangle(0, 0, SW, SH, { 0, 0, 0, 140 });
    DrawRectangleRounded({ (float)(SW/2-160), (float)(SH/2-70), 320, 140 }, 0.15f, 8, { 10, 15, 40, 230 });
    RRL({ (float)(SW/2-160), (float)(SH/2-70), 320, 140 }, 0.15f, 8, NT_GREEN);

    const char* t = "PAUSA";
    int tw = MeasureW(fontBig, fontsLoaded, t, 48, 3);
    DrawTextC(fontBig, fontsLoaded, t, (float)(SW/2 - tw/2), (float)(SH/2-55), 48, 3, NT_GREEN);
    DrawText("P  para continuar", SW/2 - MeasureText("P  para continuar",18)/2, SH/2+10, 18, WHITE);
}

void UI::DrawGameOver(int score, int coins, int hiScore, float time) const {
    DrawRectangle(0, 0, SW, SH, { 0, 0, 0, 160 });
    DrawRectangleRounded({ (float)(SW/2-220), 80, 440, 340 }, 0.1f, 8, { 10, 15, 40, 220 });
    RRL({ (float)(SW/2-220), 80, 440, 340 }, 0.1f, 8, { 220, 60, 60, 255 });

    const char* go = "GAME OVER";
    float pulse = 1.0f + 0.05f * sinf(time * 4.0f);
    int gw = MeasureW(fontBig, fontsLoaded, go, 52*pulse, 3);
    DrawTextC(fontBig, fontsLoaded, go, (float)(SW/2 - gw/2), 100, 52*pulse, 3, { 220, 60, 60, 255 });

    DrawLineEx({ (float)(SW/2-160), 165 }, { (float)(SW/2+160), 165 }, 1, { 80,80,100,200 });

    struct { const char* label; int val; Color c; } stats[] = {
        { "PUNTAJE", score,   WHITE     },
        { "MONEDAS", coins,   COIN_GOLD },
        { "RECORD",  hiScore, NT_CYAN   },
    };
    for (int i = 0; i < 3; i++) {
        float sy = 180.0f + i * 44;
        DrawText(stats[i].label, SW/2-160, (int)sy, 18, { 160,160,180,255 });
        DrawText(TextFormat("%d", stats[i].val), SW/2+60, (int)sy, 22, stats[i].c);
    }

    if (score > 0 && score >= hiScore) {
        float gp = 1.0f + 0.08f * sinf(time * 5.0f);
        const char* nr = "NUEVO RECORD!";
        DrawText(nr, SW/2 - MeasureText(nr,(int)(20*gp))/2, 320, (int)(20*gp), NT_GREEN);
    }

    DrawRectangleRounded({ (float)(SW/2-110), 360, 220, 48 }, 0.4f, 8, NT_GREEN);
    RRL({ (float)(SW/2-110), 360, 220, 48 }, 0.4f, 8, NT_CYAN);
    DrawText("R - REINICIAR", SW/2 - MeasureText("R - REINICIAR",16)/2, 376, 16, NT_DARK);
}

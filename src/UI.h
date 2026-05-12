#pragma once
#include "raylib.h"

enum class GameState { MENU, PLAYING, PAUSED, GAME_OVER };

class UI {
public:
    void LoadAssets();
    void UnloadAssets();

    void DrawMenu(float time) const;
    void DrawHUD(int score, int coins, int hiScore, float speed,
                 bool shield, bool dj, bool turbo,
                 float shieldT, float djT, float turboT) const;
    void DrawPause() const;
    void DrawGameOver(int score, int coins, int hiScore, float time) const;

private:
    void DrawPowerBar(float x, float y, float t, float maxT, Color c, const char* label) const;

    Font fontBig;
    Font fontMed;
    bool fontsLoaded = false;
};

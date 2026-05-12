# 🦕 Nova Dino Run

Endless runner 2D inspirado en el dinosaurio de Chrome, con identidad visual **NovaTec**.  
Desarrollado en **C++ 17 + Raylib 5**, compilable con **CMake** en Windows/Linux/macOS.

---

## 🎮 Controles

| Tecla       | Acción          |
|-------------|-----------------|
| `ESPACIO`   | Saltar / Iniciar |
| `↓ ABAJO`   | Agacharse        |
| `P`         | Pausar / Reanudar |
| `R`         | Reiniciar (Game Over) |
| `ESC`       | Salir            |

---

## 🚀 Cómo compilar

### Requisitos

- CMake ≥ 3.16
- Compilador C++17 (MinGW-w64 en Windows, GCC/Clang en Linux/macOS)
- Git (para que CMake descargue Raylib automáticamente)

> Si ya tienes Raylib instalado en el sistema, CMake lo detectará y no lo descargará.

### Pasos

```bash
# 1. Clonar / entrar al directorio
cd nova-dino-run

# 2. Configurar
cmake -B build -DCMAKE_BUILD_TYPE=Release

# 3. Compilar
cmake --build build --config Release

# 4. Ejecutar
./build/NovaDinoRun          # Linux/macOS
build\Release\NovaDinoRun.exe  # Windows
```

### VS Code (recomendado)

1. Instala la extensión **CMake Tools**.
2. Abre la carpeta `nova-dino-run` en VS Code.
3. Selecciona el kit de compilador (MinGW o GCC).
4. Presiona `F7` para compilar y `F5` para ejecutar.

---

## 🔊 Sonidos (opcionales)

Coloca archivos WAV/MP3 en `assets/sounds/`:

| Archivo         | Evento       |
|-----------------|--------------|
| `jump.wav`      | Salto        |
| `hit.wav`       | Colisión     |
| `coin.wav`      | Moneda       |
| `powerup.wav`   | Power-up     |
| `music.mp3`     | Música fondo |

El juego funciona sin ellos (audio silencioso).

---

## 🏆 Mecánicas

- **Obstáculos**: Cactus, Pájaros y Rocas generados aleatoriamente.
- **Monedas**: Aparecen en el aire, dan puntos extra.
- **Power-ups** (hexágonos de colores):
  - 🔵 **S** – Escudo: absorbe un golpe.
  - 🟣 **J** – Doble Salto: permite saltar en el aire.
  - 🟠 **T** – Turbo: aumenta velocidad temporalmente.
- **Dificultad**: La velocidad crece continuamente con el tiempo.
- **Hi-Score**: Se guarda en `hiscore.dat` junto al ejecutable.

---

## 📁 Estructura

```
nova-dino-run/
├── src/
│   ├── main.cpp
│   ├── Game.h / Game.cpp       ← Loop principal, estados, audio
│   ├── Player.h / Player.cpp   ← Física, animación, poderes
│   ├── Obstacle.h / Obstacle.cpp ← Cactus, pájaro, roca
│   ├── Coin.h / Coin.cpp       ← Monedas animadas
│   ├── PowerUp.h / PowerUp.cpp ← Escudo, doble salto, turbo
│   └── UI.h / UI.cpp           ← Menú, HUD, pausa, game over
├── assets/
│   ├── sounds/                 ← WAV/MP3 opcionales
│   └── fonts/                  ← Fuentes opcionales
└── CMakeLists.txt
```

---

## 🎨 Estilo NovaTec

- Fondo: cielo nocturno con estrellas y nubes parallax.
- Paleta: verde `#00C878`, cyan `#00F0C8`, fondo `#0F1423`.
- Personaje: dinosaurio geométrico en verde NovaTec.

---

*NovaTec © 2026 — Base para NovaTec Game CLI*

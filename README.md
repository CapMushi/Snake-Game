# 🐍 Snake Game (C++ with SFML)

A fun, visually enhanced Snake Game written in **C++** using **SFML (Simple and Fast Multimedia Library)**. This version features multiple game modes, background images, music, and interactive menus.

---

## 🎮 Game Modes

1. **Zen Mode (Endless)** – No walls, wrap-around edges.
2. **Easy Mode** – Wall borders added for increased challenge.
3. **Medium Mode** – Horizontal walls across the board.
4. **Hard Mode** – Inverted T-shape and corner boxes as obstacles.

---

## 🕹️ Controls

- **W/A/S/D** → Move Up/Left/Down/Right  
- **P** → Pause / Open Menu  
- **Mouse Click** → Select options in Pause Menu  
- **Y** → Restart game on Game Over screen

---

## 🛠️ Setup & Compile

### 🔧 Prerequisites

- C++ Compiler (e.g., G++, Clang)
- [SFML 2.5+](https://www.sfml-dev.org/download.php)

Install SFML using `vcpkg` (recommended):
```bash
git clone https://github.com/microsoft/vcpkg
./vcpkg/bootstrap-vcpkg.sh
./vcpkg/vcpkg install sfml

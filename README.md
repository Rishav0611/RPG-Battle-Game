# ⚔️ RPG Battle Game

A 2D turn-based RPG battle game built in **C++** using **SFML** (graphics) and **miniaudio** (sound). Choose your hero, pick an enemy, and fight it out in a fully animated battle arena!

---

## 📖 About the Project

This project is made because building new never stop's, and the aim is to build an actual working 2D game — complete with sprite animations, sound effects, turn-based combat, and a polished UI.

**Owners:** Singh Rishav & Rohit Kumar

---

## 🎮 Game Overview

| | |
|---|---|
| **Genre** | 2D Turn-Based RPG Battle Game |
| **Language** | C++ |
| **Graphics** | SFML |
| **Audio** | miniaudio |
| **Platform** | Desktop (Windows) |

---

## ⚔️ Features

### 🦸 Hero Classes
| Hero | HP | Attack | Special Move |
|---|---|---|---|
| **Warrior** | 150 | 30 | Heavy Attack (+20 extra damage) |
| **Archer** | 100 | 20 | Shoot Arrow (+15 extra damage, animated flying arrow) |

### 👹 Enemy Classes
| Enemy | HP | Attack | Special Ability |
|---|---|---|---|
| **Goblin** | 50 | 15 | 30% chance of Quick Strike (double damage) |
| **Dragon** | 190 | 40 | Fire Attack (fixed 60 damage, animated fire effect) |

### 🥊 Battle Mechanics
- Turn-based combat system (Player turn → Enemy turn)
- 4 combat actions: **Attack**, **Special Attack**, **Potion (heal)**, **Defend (halves next damage)**
- Mana/Potion system for healing mid-battle
- Level-up system that boosts Attack & Max HP after winning

### 🎨 Graphics & Animation
- Custom sprite sheet system with idle & attack animations for every character
- Frame-by-frame animation engine
- Dynamic battle backgrounds (different arena for Goblin fight vs Dragon fight)
- Animated main menu with a flying dragon, idle warrior & archer
- Pulsing glow effect on the title text
- Custom rounded UI buttons with hover effects (scale + color change + hover sound)
- Live HP bars for both hero and enemy

### 🔊 Audio
- Looping background menu music
- Sound effects for button hover, attacks, hero selection, and game over

### 🕹️ Game Flow (State Machine)
```
Main Menu → Hero Select → Enemy Select → Battle → Game Over → (back to Main Menu)
```

---

## 🛠️ Built With
- **C++**
- **SFML** – Graphics & Window handling
- **miniaudio** – Sound engine

---

## 🚀 How to Run

```bash
g++ Project.c++ -o sfml_game.exe -I C:\msys64\ucrt64\include -L C:\msys64\ucrt64\lib -lsfml-graphics -lsfml-window -lsfml-system -lwinmm -lole32
.\sfml_game.exe
```

> Make sure all asset files (sprites, fonts, audio) are present in the same folder as the executable.

---

## 📁 Project Structure

```
Asset/
 ├── Font/
 ├── Music/
 └── Sprites/
      ├── Archer Sprite/
      └── Warrior Sprite/
Character-hero-code.cpp
project-code.c
README.md
```

---

## 👥 Team & Contributions

| Member | Contribution |
|---|---|
| **Rishav** | Core character system, graphics & animation engine, UI, main game loop |
| **Saumya** | Hero mechanics (potion, defend, level-up system) |
| **Rohit** | Enemy AI logic (Goblin & Dragon attack behavior) |
---

<img width="451" height="322" alt="image" src="https://github.com/user-attachments/assets/094226c4-8b4b-40a1-beea-253c1de77bfb" />
<img width="448" height="319" alt="image" src="https://github.com/user-attachments/assets/cb24335f-39b3-42b3-bd13-6523b5138656" />



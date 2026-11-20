# SOUL SHIFT

### A 2D Puzzle-Platformer Game

**CSC241 – Object Oriented Programming | Fall 2026**

---

## Project Overview

Soul Shift is a 2D puzzle-platformer developed in C++ using the Windows Win32 API and GDI (Graphics Device Interface).

The game introduces two player forms: **Human** and **Soul**. Each form has different movement characteristics and interacts differently with environmental obstacles and traps.

Players must switch between forms strategically, avoid dangerous traps, navigate barriers, and reach the goal portal to progress through the levels.

## Core Features

- 2D platformer gameplay.
- Human and Soul form switching.
- Form-dependent movement and physics.
- Gravity, jumping, and collision handling.
- Energy consumption and recharge for Soul form.
- Solid walls and spirit barriers.
- Spike traps and astral traps.
- Goal portal and level progression.
- Three level layouts.
- Animated barriers, traps, and goal portal.
- On-screen HUD displaying current form, level, and energy.

## Technologies Used

| Technology | Purpose |
|---|---|
| C++17 | Core programming language |
| Windows Win32 API | Window creation and event handling |
| GDI | 2D drawing and rendering |
| MinGW-w64 GCC | C++ compiler |
| Visual Studio Code | Development environment |

## Gameplay Controls

| Key | Action |
|---|---|
| A / Left Arrow | Move left |
| D / Right Arrow | Move right |
| W / Space / Up Arrow | Jump (Human form) |
| Left Shift / Right Shift / E | Switch between Human and Soul forms |
| R | Reset current level |
| Escape | Exit the game |

## Player Forms

### Human Form

- Uses normal gravity.
- Can jump and move across solid platforms.
- Cannot pass through spirit barriers.
- Is vulnerable to spike traps.
- Soul energy recharges while in Human form.

### Soul Form

- Uses reduced gravity.
- Can float with controlled upward movement.
- Can pass through spirit barriers.
- Is vulnerable to astral traps.
- Consumes energy while active.

## Level Design

The game contains three level layouts defined in the level construction module.

Each level includes a spawn position, obstacles, traps, and a goal portal. Reaching the goal portal advances the player to the next level.

## Object-Oriented Design

The project is structured around the following classes:

| Class | Responsibility |
|---|---|
| GameObject | Abstract base class for game entities |
| Player | Player movement, physics, form switching, and rendering |
| Obstacle | Abstract obstacle base class |
| SolidWall | Standard solid obstacle |
| SpiritBarrier | Barrier that blocks Human form |
| Trap | Abstract trap base class |
| SpikeTrap | Trap dangerous to Human form |
| AstralTrap | Trap dangerous to Soul form |
| GoalPortal | Level completion object |
| Level | Stores level layout and game objects |
| Game | Controls game state, updates, rendering, and progression |

### OOP Concepts Demonstrated

- **Abstraction:** Abstract classes define common interfaces for game objects, obstacles, and traps.
- **Inheritance:** Specialized game objects derive from their respective base classes.
- **Polymorphism:** Virtual update and render functions allow derived objects to implement their own behavior.
- **Encapsulation:** Player state and physics data are maintained within the Player class.
- **Composition:** Game manages Player and Level objects; Level owns its obstacles, traps, and goal portal.

## Project Structure

```text
SoulShift/
├── .vscode/
│   ├── launch.json
│   └── tasks.json
│
├── Game.cpp
├── Game.hpp
├── GameObject.hpp
├── Level.cpp
├── Level.hpp
├── Player.cpp
├── Player.hpp
├── Obstacles.cpp
├── Obstacles.hpp
├── Traps.cpp
├── Traps.hpp
├── Settings.hpp
└── main.cpp
```

## Build and Run

### Prerequisites

- Windows operating system
- MinGW-w64 GCC compiler
- Visual Studio Code (recommended)

### Build Using VS Code

1. Open the project folder in Visual Studio Code.
2. Ensure MinGW-w64 GCC is installed and available at the compiler path configured in `.vscode/tasks.json`.
3. Open the Terminal or use the default build task.
4. Run the configured build task.
5. Execute the generated `SoulShift.exe`.

The build configuration links the Windows GDI and User32 libraries.

## Team Members

| Student Name | Registration Number |
|---|---|
| Rehan Ali | FA25-BEE-278 |
| Muhammad Hashir | FA25-BEE-151 |
| Muhammad Ali Khan | FA25-BEE-131 |

**Class:** BEE-3A  
**Course:** CSC241 – Object Oriented Programming  
**Instructor:** Dr. Riaz Hussain

## Project Purpose

This project is developed as part of the CSC241 Object Oriented Programming semester project. Its purpose is to demonstrate object-oriented design and implementation through an interactive 2D game.

---

*Soul Shift – Switch Forms. Overcome Barriers. Reach the Portal.*

# Create the content for the markdown file
markdown_content = """
# Bus Game Documentation

## 1. Overview
The **Bus Game** is an engaging 2D action game developed using SDL2. Players control a bus, avoid obstacles, and shoot lasers to destroy them. Key features include a scoring system, health mechanics, and experience points.

## 2. Requirements

### Software:
- SDL2 library
- SDL2_ttf (for text rendering)

### System:
- Windows/Linux system capable of running SDL applications

### Dependencies:
- Font file: `C:\\Windows\\Fonts\\arial.ttf` (used for rendering text)

## 3. Features

### Obstacles:
- **Types:** Circular and square obstacles
- **Behavior:** Move downward and respawn at the top after going off-screen

### Bus:
- **Controls:** Movable horizontally
- **Weapons:** Laser-shooting capabilities
- **Health:** Limited, reducing upon collisions

### Scoring System:
- **Points/Experience:** Earned by destroying obstacles
- **Health Penalty:** Collisions reduce health

### Game Over:
- Displays a **Game Over** screen when health reaches zero

### Rendering:
- Dynamic text rendering with SDL2_ttf
- Smooth movement and collision detection for a seamless experience

## 4. Code Breakdown

### Initialization:
- `init()`: Sets up SDL2, SDL2_ttf, the game window, and the renderer.

### Gameplay Logic:
- **Obstacle Management:** `generateObstacles()`, `moveObstacles()`.
- **Laser Mechanics:** `shootLaser()`, `updateLasers()`, `renderLasers()`.
- **Collision Detection:** `detectCollisions()` handles interactions between lasers, obstacles, and the bus.

### Rendering:
- `renderGame()`: Renders the bus, obstacles, lasers, score, and health.
- `renderText()`: Handles text rendering.

### Game Loop:
- Processes input, updates game states, and renders each frame.

### Cleanup:
- `cleanup()`: Frees resources and ensures proper shutdown of SDL and SDL2_ttf.

## 5. Game Controls

| Key           | Action                    |
|---------------|---------------------------|
| **Left Arrow** | Move the bus left         |
| **Right Arrow**| Move the bus right        |
| **Spacebar**   | Shoot laser               |
| **ESC**        | Quit game (on Game Over)  |

## 6. Future Improvements
- Add diverse obstacle types and power-ups
- Introduce levels with difficulty progression
- Integrate sound effects and background music for an immersive experience

## 7. File Organization
The code is self-contained but relies on correctly set up SDL2 library paths for compilation and execution.
"""

# File path to save the markdown file
file_path = "/mnt/data/Bus_Game_Documentation.md"

# Write the markdown content to the file
with open(file_path, "w") as file:
    file.write(markdown_content)

file_path

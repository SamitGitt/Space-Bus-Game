#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define MAX_OBSTACLES 5
#define MAX_LASERS 10
#define LASER_DAMAGE 30
#define OBSTACLE_HEALTH 100
#define LASER_RADIUS 5
#define WINNING_SCORE 1000
#define BUS_HEALTH 1000
#define COLLISION_DAMAGE 100
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif



typedef struct {
    int x, y;
    int width, height;
    int health;
} SquareObstacle;

typedef struct {
    int x, y;
    int radius;
    int health;
} CircleObstacle;

typedef struct {
    int x, y;
    int radius;
    int active;
} Laser;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SquareObstacle squareObstacles[MAX_OBSTACLES];
CircleObstacle circleObstacles[MAX_OBSTACLES];
Laser lasers[MAX_LASERS];
int busX = (WINDOW_WIDTH - 50) / 2;
int busY = WINDOW_HEIGHT - 100;
float busVelocity = 0.0f;
int score = 0;
int gameOver = 0;
int gameWin = 0;
int busHealth = BUS_HEALTH;
TTF_Font* font = NULL;

void init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        exit(1);
    }
    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        exit(1);
    }
    window = SDL_CreateWindow("Bus Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
        exit(1);
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        exit(1);
    }

    font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 24);
    if (!font) {
        printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        exit(1);
    }
}

void cleanup() {
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void generateObstacles() {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        squareObstacles[i].x = rand() % (WINDOW_WIDTH - 30);
        squareObstacles[i].y = -(rand() % 600);
        squareObstacles[i].width = 30;
        squareObstacles[i].height = 30;
        squareObstacles[i].health = OBSTACLE_HEALTH;

        circleObstacles[i].x = rand() % (WINDOW_WIDTH - 30);
        circleObstacles[i].y = -(rand() % 600);
        circleObstacles[i].radius = 15;
        circleObstacles[i].health = OBSTACLE_HEALTH;
    }
}

void shootLaser() {
    for (int i = 0; i < MAX_LASERS; i++) {
        if (!lasers[i].active) {
            lasers[i].x = busX + 25; // Center of the bus
            lasers[i].y = busY;
            lasers[i].radius = LASER_RADIUS;
            lasers[i].active = 1;
            break;
        }
    }
}

void moveObstacles() {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        squareObstacles[i].y += 2;
        circleObstacles[i].y += 2;

        if (squareObstacles[i].y > WINDOW_HEIGHT) {
            squareObstacles[i].y = -30;
            squareObstacles[i].x = rand() % (WINDOW_WIDTH - 30);
            squareObstacles[i].health = OBSTACLE_HEALTH;
        }

        if (circleObstacles[i].y > WINDOW_HEIGHT) {
            circleObstacles[i].y = -30;
            circleObstacles[i].x = rand() % (WINDOW_WIDTH - 30);
            circleObstacles[i].health = OBSTACLE_HEALTH;
        }
    }
}

void updateLasers() {
    for (int i = 0; i < MAX_LASERS; i++) {
        if (lasers[i].active) {
            lasers[i].y -= 5;
            if (lasers[i].y < 0) {
                lasers[i].active = 0;
            }
        }
    }
}

void detectCollisions() {
    // Laser collisions with obstacles
    for (int i = 0; i < MAX_LASERS; i++) {
        if (lasers[i].active) {
            for (int j = 0; j < MAX_OBSTACLES; j++) {
                if (squareObstacles[j].health > 0 &&
                    lasers[i].x >= squareObstacles[j].x &&
                    lasers[i].x <= squareObstacles[j].x + squareObstacles[j].width &&
                    lasers[i].y >= squareObstacles[j].y &&
                    lasers[i].y <= squareObstacles[j].y + squareObstacles[j].height) {
                    lasers[i].active = 0;
                    squareObstacles[j].health -= LASER_DAMAGE;
                    if (squareObstacles[j].health <= 0) {
                        score += 10;
                    }
                }

                if (circleObstacles[j].health > 0) {
                    int dx = lasers[i].x - circleObstacles[j].x;
                    int dy = lasers[i].y - circleObstacles[j].y;
                    double distance = sqrt(dx * dx + dy * dy);
                    if (distance <= circleObstacles[j].radius + LASER_RADIUS) {
                        lasers[i].active = 0;
                        circleObstacles[j].health -= LASER_DAMAGE;
                        if (circleObstacles[j].health <= 0) {
                            score += 10;
                        }
                    }
                }
            }
        }
    }

    // Obstacle collisions with bus
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        SDL_Rect busRect = { busX, busY, 50, 30 };
        SDL_Rect squareRect = { squareObstacles[i].x, squareObstacles[i].y, squareObstacles[i].width, squareObstacles[i].height };

        if (squareObstacles[i].health > 0 &&
            SDL_HasIntersection(&busRect, &squareRect)) {
            squareObstacles[i].health = 0;
            busHealth -= COLLISION_DAMAGE;
        }

        int dx = circleObstacles[i].x - (busX + 25); // Center of the bus
        int dy = circleObstacles[i].y - (busY + 15); // Center of the bus
        double distance = sqrt(dx * dx + dy * dy);

        if (circleObstacles[i].health > 0 && distance < circleObstacles[i].radius + 25) {
            circleObstacles[i].health = 0;
            busHealth -= COLLISION_DAMAGE;
        }
    }

    if (busHealth <= 0) {
        gameOver = 1;
    }

    if (score >= WINNING_SCORE) {
        gameWin = 1;
    }
}

void renderLasers() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    for (int i = 0; i < MAX_LASERS; i++) {
        if (lasers[i].active) {
            SDL_RenderDrawLine(renderer, lasers[i].x, lasers[i].y, lasers[i].x, lasers[i].y - 10);
        }
    }
}

void renderText(const char* text, int x, int y, SDL_Color color) {
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect renderQuad = { x, y, textSurface->w, textSurface->h };
    SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void renderGame() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 50, 205, 50, 255);
    SDL_Rect bus = { busX, busY, 50, 30 };
    SDL_RenderFillRect(renderer, &bus);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (squareObstacles[i].health > 0) {
            SDL_Rect rect = { squareObstacles[i].x, squareObstacles[i].y, squareObstacles[i].width, squareObstacles[i].height };
            SDL_RenderFillRect(renderer, &rect);
        }

        if (circleObstacles[i].health > 0) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            for (int j = 0; j < 360; j++) {
                double rad = j * M_PI / 180.0;
                SDL_RenderDrawPoint(renderer,
                                    circleObstacles[i].x + circleObstacles[i].radius * cos(rad),
                                    circleObstacles[i].y + circleObstacles[i].radius * sin(rad));
            }
        }
    }

    renderLasers();

    SDL_Color white = { 255, 255, 255, 255 };
    char hud[50];
    sprintf(hud, "Score: %d  Health: %d", score, busHealth);
    renderText(hud, 10, 10, white);

    SDL_RenderPresent(renderer);
}

int main(int argc, char* args[]) {
    srand((unsigned int)time(NULL));
    init();
    generateObstacles();

    int quit = 0;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                        if (busX > 0) {
                            busX -= 10;
                        }
                        break;
                    case SDLK_RIGHT:
                        if (busX < WINDOW_WIDTH - 50) {
                            busX += 10;
                        }
                        break;
                    case SDLK_SPACE:
                        shootLaser();
                        break;
                    case SDLK_ESCAPE:
                        if (gameWin || gameOver) {
                            quit = 1;
                        }
                        break;
                }
            }
        }

        if (!gameOver && !gameWin) {
            moveObstacles();
            updateLasers();
            detectCollisions();
        }

        if (gameWin) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            SDL_Color green = { 0, 255, 0, 255 };
            renderText("You Win! Press ESC to quit.", 200, 300, green);
            SDL_RenderPresent(renderer);
        } else if (gameOver) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            SDL_Color red = { 255, 0, 0, 255 };
            renderText("Defeat! Press ESC to quit.", 200, 300, red);
            SDL_RenderPresent(renderer);
        } else {
            renderGame();
        }

        SDL_Delay(16);
    }

    cleanup();
    return 0;
}

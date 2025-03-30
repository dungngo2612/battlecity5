

#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "Map.h"
#include "Tank.h"
#include "EnemyTank.h"  // Thêm EnemyTank
#include "Menu.h"
#include <vector>

class Game {
public:
    Game();
    ~Game();
    void run();

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    bool running;
    Map* map = nullptr;
    Tank* playerTank1 = nullptr;
    Tank* playerTank2 = nullptr;
    std::vector<EnemyTank*> enemyTanks;  // Danh sách xe tăng địch
    Mix_Music* gameMusic = nullptr;
    Mix_Music* menuMusic = nullptr;
    Mix_Chunk* shootSound = nullptr;
    SDL_Texture* winnerTexture = nullptr;
    Mix_Music* winnerSound = nullptr;
    SDL_Texture* startButtonTexture = nullptr;
    SDL_Texture* exitButtonTexture = nullptr;
    SDL_Texture* player1Texture = nullptr;
    SDL_Texture* player2Texture = nullptr;
    SDL_Texture* backgroundTexture = nullptr;
    int selectedOption = 0;
    bool isTwoPlayerMode = true;

    SDL_Texture* hpItemTexture = nullptr;
    SDL_Rect hpItemRect;
    bool hpItemActive = false;
    Uint32 lastHpItemSpawnTime = 0;
    static const int HP_SPAWN_INTERVAL = 30000;

    static const int SCREEN_WIDTH = 800;
    static const int SCREEN_HEIGHT = 600;

    void render();
    void checkBulletCollisions();
    void showGameOverScreen();
    void runGame();
    void createGameOverMenu();
    int handleGameOverInput();
    void resetGame();
    void createModeSelectionMenu();
    int showModeSelectionScreen();
    void spawnHpItem();
    void checkHpItemCollision();
    void spawnEnemyTanks();  // Hàm mới để sinh xe tăng địch
};

#endif

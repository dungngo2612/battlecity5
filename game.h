
#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <vector>
#include "Map.h"
#include "Tank.h"
#include "EnemyTank.h"
#include "FastEnemyTank.h"
#include "Menu.h"

class Game {
private:
    int x,y;
    static const int SCREEN_WIDTH = 800;
    static const int SCREEN_HEIGHT = 600;
    static const int HP_SPAWN_INTERVAL = 10000;
    static const int ENEMY_SPAWN_INTERVAL = 90000; // 90 giây (90000ms)

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    Map* map = nullptr;
    Tank* playerTank1 = nullptr;
    Tank* playerTank2 = nullptr;
    std::vector<EnemyTank*> enemyTanks;
    Mix_Music* gameMusic = nullptr;
    Mix_Music* menuMusic = nullptr;
    Mix_Chunk* shootSound = nullptr;
    Mix_Music* winnerSound = nullptr;
    SDL_Texture* winnerTexture = nullptr;
    SDL_Texture* startButtonTexture = nullptr;
    SDL_Texture* exitButtonTexture = nullptr;
    SDL_Texture* player1Texture = nullptr;
    SDL_Texture* player2Texture = nullptr;
    SDL_Texture* backgroundTexture = nullptr;
    SDL_Texture* hpItemTexture = nullptr;
    SDL_Rect hpItemRect;
    bool hpItemActive = false;
    Uint32 lastHpItemSpawnTime = 0;
    Uint32 lastEnemySpawnTime = 0;
    SDL_Texture* scoreTexture = nullptr;
    int score = 0;
    bool running = true;
    bool isTwoPlayerMode = false;
    int selectedOption = 0;
    bool isSoundOn = true;

    // Biến cho bộ đếm thời gian
    TTF_Font* font = nullptr;
    SDL_Texture* timerTexture = nullptr;
    Uint32 gameStartTime = 0;

    // Biến cho nút sound
    SDL_Texture* soundButtonTexture = nullptr;
    SDL_Rect soundButtonRect;

public:
    Game();
    ~Game();

    void spawnEnemyTanks();
    void spawnSingleEnemyTank();
    void render();
    void checkBulletCollisions();
    void spawnHpItem();
    void checkHpItemCollision();
    void createGameOverMenu();
    void createModeSelectionMenu();
    int handleGameOverInput();
    int showMainMenu(); // Hàm mới cho màn hình ban đầu
    int showModeSelectionScreen();
    void resetGame();
    void showGameOverScreen();
    void runGame();
    void run();
    void updateScoreTexture();
    void updateTimerTexture();
    void setSoundState(bool state);
};

#endif

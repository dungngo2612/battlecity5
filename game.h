#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "Map.h"
#include "Tank.h"
#include "Menu.h"

class Game {
public:
    Game();
    ~Game();
    void run();

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    bool running;
    Map map;
    Tank* playerTank1 = nullptr;
    Tank* playerTank2 = nullptr;
    Mix_Music* gameMusic = nullptr;
    Mix_Music* menuMusic = nullptr;
    Mix_Chunk* shootSound = nullptr;
    SDL_Texture* winnerTexture = nullptr;
    Mix_Music* winnerSound = nullptr;
    SDL_Texture* startButtonTexture = nullptr;  // start_button.png
    SDL_Texture* exitButtonTexture = nullptr;   // exit_button.jpg
    SDL_Texture* player1Texture = nullptr;      // Player1.png
    SDL_Texture* player2Texture = nullptr;      // Player2.png
    SDL_Texture* backgroundTexture = nullptr;   // background.jpg
    int selectedOption = 0;
    bool isTwoPlayerMode = true;

    // Thêm cho item HP
    SDL_Texture* hpItemTexture = nullptr;  // Texture của item HP
    SDL_Rect hpItemRect;                  // Vị trí và kích thước của item HP
    bool hpItemActive = false;            // Trạng thái item HP (xuất hiện hay không)
    Uint32 lastHpItemSpawnTime = 0;       // Thời gian item HP cuối cùng xuất hiện
    static const int HP_SPAWN_INTERVAL = 30000;  // 30 giây (tính bằng milliseconds)

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
    void spawnHpItem();  // Hàm sinh item HP
    void checkHpItemCollision();  // Kiểm tra va chạm với item HP
};

#endif

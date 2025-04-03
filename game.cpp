
#include "Game.h"
#include "FastEnemyTank.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <SDL_ttf.h>
#include <sstream>

using namespace std;

Game::Game() {
    running = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        running = false;
        return;
    }

    if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == 0) {
        cerr << "SDL_image could not initialize! IMG_Error: " << IMG_GetError() << endl;
        running = false;
        SDL_Quit();
        return;
    }

    if (Mix_Init(MIX_INIT_MP3) == 0) {
        cerr << "SDL_mixer could not initialize! Mix_Error: " << Mix_GetError() << endl;
        running = false;
        IMG_Quit();
        SDL_Quit();
        return;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        cerr << "SDL_mixer could not open audio! Mix_Error: " << Mix_GetError() << endl;
        running = false;
        Mix_Quit();
        IMG_Quit();
        SDL_Quit();
        return;
    }

    if (TTF_Init() < 0) {
        cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << endl;
        running = false;
        Mix_Quit();
        IMG_Quit();
        SDL_Quit();
        return;
    }

    window = SDL_CreateWindow(
        "Battle City - 2 Players",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if (!window) {
        cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        running = false;
        TTF_Quit();
        Mix_Quit();
        IMG_Quit();
        SDL_Quit();
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << endl;
        running = false;
        SDL_DestroyWindow(window);
        TTF_Quit();
        Mix_Quit();
        IMG_Quit();
        SDL_Quit();
        return;
    }

    font = TTF_OpenFont("arial.ttf", 24);
    if (!font) {
        cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << endl;
        running = false;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        Mix_Quit();
        IMG_Quit();
        SDL_Quit();
        return;
    }

    map = new Map(renderer);
    if (!map) {
        cerr << "Failed to allocate memory for map!" << endl;
        running = false;
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        Mix_Quit();
        IMG_Quit();
        SDL_Quit();
        return;
    }

    playerTank1 = new Tank(0, 0, SDL_Color{0, 255, 0, 255}, renderer, true);
    if (!playerTank1) {
        cerr << "Failed to allocate memory for playerTank1!" << endl;
        running = false;
        delete map;
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        Mix_Quit();
        IMG_Quit();
        SDL_Quit();
        return;
    }

    playerTank2 = new Tank(770, 570, SDL_Color{255, 0, 0, 255}, renderer, false);
    if (!playerTank2) {
        cerr << "Failed to allocate memory for playerTank2!" << endl;
        delete playerTank1;
        delete map;
        running = false;
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        Mix_Quit();
        IMG_Quit();
        SDL_Quit();
        return;
    }

    spawnEnemyTanks();
    lastEnemySpawnTime = SDL_GetTicks();

    gameMusic = Mix_LoadMUS("background.mp3");
    if (!gameMusic) {
        cerr << "Failed to load game music (background.mp3)! Mix_Error: " << Mix_GetError() << endl;
    }

    menuMusic = Mix_LoadMUS("backgroundMenu.mp3");
    if (!menuMusic) {
        cerr << "Failed to load menu music (backgroundMenu.mp3)! Mix_Error: " << Mix_GetError() << endl;
    } else if (isSoundOn) {
        Mix_PlayMusic(menuMusic, -1);
    }

    shootSound = Mix_LoadWAV("shoot.wav");
    if (!shootSound) {
        cerr << "Failed to load shoot sound (shoot.wav)! Mix_Error: " << Mix_GetError() << endl;
    } else {
        Mix_VolumeChunk(shootSound, isSoundOn ? MIX_MAX_VOLUME : 0);
    }

    winnerSound = Mix_LoadMUS("winner.mp3");
    if (!winnerSound) {
        cerr << "Failed to load winner.mp3! Mix_Error: " << Mix_GetError() << endl;
    }

    SDL_Surface* winnerSurface = IMG_Load("winner.jpg");
    if (!winnerSurface) {
        cerr << "Failed to load winner.jpg! IMG_Error: " << IMG_GetError() << endl;
    } else {
        winnerTexture = SDL_CreateTextureFromSurface(renderer, winnerSurface);
        SDL_FreeSurface(winnerSurface);
        if (!winnerTexture) {
            cerr << "Failed to create texture from winner.jpg! SDL_Error: " << SDL_GetError() << endl;
        }
    }

    SDL_Surface* backgroundSurface = IMG_Load("background.jpg");
    if (!backgroundSurface) {
        cerr << "Failed to load background.jpg! IMG_Error: " << IMG_GetError() << endl;
    } else {
        backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
        SDL_FreeSurface(backgroundSurface);
        if (!backgroundTexture) {
            cerr << "Failed to create texture from background.jpg! SDL_Error: " << SDL_GetError() << endl;
        }
    }

    SDL_Surface* startButtonSurface = IMG_Load("start_button.png");
    if (startButtonSurface) {
        startButtonTexture = SDL_CreateTextureFromSurface(renderer, startButtonSurface);
        SDL_FreeSurface(startButtonSurface);
    } else {
        cerr << "Failed to load start_button.png! IMG_Error: " << IMG_GetError() << endl;
    }

    SDL_Surface* exitButtonSurface = IMG_Load("exit_button.jpg");
    if (exitButtonSurface) {
        exitButtonTexture = SDL_CreateTextureFromSurface(renderer, exitButtonSurface);
        SDL_FreeSurface(exitButtonSurface);
    } else {
        cerr << "Failed to load exit_button.jpg! IMG_Error: " << IMG_GetError() << endl;
    }

    SDL_Surface* soundButtonSurface = IMG_Load("sound_button.jpg");
    if (soundButtonSurface) {
        soundButtonTexture = SDL_CreateTextureFromSurface(renderer, soundButtonSurface);
        SDL_FreeSurface(soundButtonSurface);
    } else {
        cerr << "Failed to load sound_button.jpg! IMG_Error: " << IMG_GetError() << endl;
    }
    soundButtonRect = {(SCREEN_WIDTH - 200) / 2, 400, 200, 50};

    SDL_Surface* hpItemSurface = IMG_Load("hp_item.jpg");
    if (!hpItemSurface) {
        cerr << "Failed to load hp_item.jpg! IMG_Error: " << IMG_GetError() << endl;
    } else {
        hpItemTexture = SDL_CreateTextureFromSurface(renderer, hpItemSurface);
        SDL_FreeSurface(hpItemSurface);
        if (!hpItemTexture) {
            cerr << "Failed to create texture from hp_item.jpg! SDL_Error: " << SDL_GetError() << endl;
        }
    }
    hpItemRect = {0, 0, 20, 20};
    hpItemActive = false;

    createGameOverMenu();
    createModeSelectionMenu();

    srand(static_cast<unsigned int>(time(nullptr)));
    updateScoreTexture();
}

Game::~Game() {
    delete playerTank1;
    delete playerTank2;
    delete map;

    for (auto enemy : enemyTanks) {
        delete enemy;
    }
    enemyTanks.clear();

    if (gameMusic) Mix_FreeMusic(gameMusic);
    if (menuMusic) Mix_FreeMusic(menuMusic);
    if (shootSound) Mix_FreeChunk(shootSound);
    if (winnerSound) Mix_FreeMusic(winnerSound);

    if (winnerTexture) SDL_DestroyTexture(winnerTexture);
    if (startButtonTexture) SDL_DestroyTexture(startButtonTexture);
    if (exitButtonTexture) SDL_DestroyTexture(exitButtonTexture);
    if (player1Texture) SDL_DestroyTexture(player1Texture);
    if (player2Texture) SDL_DestroyTexture(player2Texture);
    if (backgroundTexture) SDL_DestroyTexture(backgroundTexture);
    if (hpItemTexture) SDL_DestroyTexture(hpItemTexture);
    if (scoreTexture) SDL_DestroyTexture(scoreTexture);
    if (timerTexture) SDL_DestroyTexture(timerTexture);
    if (soundButtonTexture) SDL_DestroyTexture(soundButtonTexture);
    if (font) TTF_CloseFont(font);

    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);

    Mix_CloseAudio();
    Mix_Quit();
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

void Game::spawnEnemyTanks() {
    enemyTanks.clear();
    const int numEnemies = 3;
    for (int i = 0; i < numEnemies; i++) {
        int x, y;
        SDL_Rect tempRect;
        bool validPosition = false;

        while (!validPosition) {
            x = rand() % (SCREEN_WIDTH - 30);
            y = rand() % (SCREEN_HEIGHT - 30);
            tempRect = {x, y, 30, 30};
            if (!map->isColliding(tempRect)) {
                validPosition = true;
            }
        }

        EnemyTank* enemy;
        if (rand() % 2 == 0) {
            enemy = new EnemyTank(x, y, renderer);
        } else {
            enemy = new FastEnemyTank(x, y, renderer);
        }
        if (enemy) {
            enemyTanks.push_back(enemy);
            cout << "Spawned EnemyTank " << i + 1 << " at position (" << x << ", " << y << ")" << endl;
        } else {
            cerr << "Failed to allocate memory for EnemyTank!" << endl;
        }
    }
}

void Game::spawnSingleEnemyTank() {
    int x, y;
    SDL_Rect tempRect;
    bool validPosition = false;

    while (!validPosition) {
        x = rand() % (SCREEN_WIDTH - 30);
        y = rand() % (SCREEN_HEIGHT - 30);
        tempRect = {x, y, 30, 30};
        if (!map->isColliding(tempRect)) {
            validPosition = true;
        }
    }

    EnemyTank* enemy;
    if (rand() % 2 == 0) {
        enemy = new EnemyTank(x, y, renderer);
    } else {
        enemy = new FastEnemyTank(x, y, renderer);
    }
    if (enemy) {
        enemyTanks.push_back(enemy);
        cout << "Spawned new EnemyTank at position (" << x << ", " << y << ")" << endl;
    } else {
        cerr << "Failed to allocate memory for new EnemyTank!" << endl;
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
    SDL_RenderClear(renderer);

    map->draw(renderer);

    if (playerTank1->isAlive()) {
        playerTank1->draw(renderer);
    }

    if (isTwoPlayerMode && playerTank2->isAlive()) {
        playerTank2->draw(renderer);
    }

    if (!isTwoPlayerMode) {
        for (auto enemy : enemyTanks) {
            if (enemy->isAlive() || enemy->getIsExploding()) {
                enemy->draw(renderer);
            }
        }
    }

    if (hpItemActive && hpItemTexture) {
        SDL_RenderCopy(renderer, hpItemTexture, nullptr, &hpItemRect);
    }

    if (scoreTexture) {
        int textW, textH;
        SDL_QueryTexture(scoreTexture, nullptr, nullptr, &textW, &textH);
        SDL_Rect scoreRect = {10, 10, textW, textH};
        SDL_RenderCopy(renderer, scoreTexture, nullptr, &scoreRect);
    }

    if (timerTexture) {
        int textW, textH;
        SDL_QueryTexture(timerTexture, nullptr, nullptr, &textW, &textH);
        SDL_Rect timerRect = {SCREEN_WIDTH - textW - 10, 10, textW, textH};
        SDL_RenderCopy(renderer, timerTexture, nullptr, &timerRect);
    }

    SDL_RenderPresent(renderer);
}

void Game::checkBulletCollisions() {
    if (isTwoPlayerMode && playerTank2->isAlive()) {
        for (auto& bullet : playerTank1->getBullets()) {
            if (bullet.isActive()) {
                SDL_Rect bulletRect = bullet.getRect();
                if (playerTank2->isCollidingWithTank(bulletRect, playerTank2->getRect())) {
                    playerTank2->takeDamage();
                    bullet.setActive(false);
                    if (!playerTank2->isAlive()) {
                        cout << "Player 1 wins! Player 2 is destroyed!" << endl;
                    }
                }
            }
        }
    }

    if (isTwoPlayerMode && playerTank1->isAlive()) {
        for (auto& bullet : playerTank2->getBullets()) {
            if (bullet.isActive()) {
                SDL_Rect bulletRect = bullet.getRect();
                if (playerTank1->isCollidingWithTank(bulletRect, playerTank1->getRect())) {
                    playerTank1->takeDamage();
                    bullet.setActive(false);
                    if (!playerTank1->isAlive()) {
                        cout << "Player 2 wins! Player 1 is destroyed!" << endl;
                    }
                }
            }
        }
    }

    if (!isTwoPlayerMode) {
        for (auto enemy : enemyTanks) {
            if (enemy->isAlive()) {
                for (auto& bullet : playerTank1->getBullets()) {
                    if (bullet.isActive()) {
                        SDL_Rect bulletRect = bullet.getRect();
                        if (enemy->isCollidingWithTank(bulletRect, enemy->getRect())) {
                            enemy->takeDamage();
                            bullet.setActive(false);
                            if (!enemy->isAlive()) {
                                score += 100;
                                updateScoreTexture();
                                cout << "Enemy tank destroyed! Score: " << score << endl;
                            }
                        }
                    }
                }
            }
        }

        if (playerTank1->isAlive()) {
            for (auto enemy : enemyTanks) {
                if (enemy->isAlive()) {
                    for (auto& bullet : enemy->getBullets()) {
                        if (bullet.isActive()) {
                            SDL_Rect bulletRect = bullet.getRect();
                            if (playerTank1->isCollidingWithTank(bulletRect, playerTank1->getRect())) {
                                playerTank1->takeDamage();
                                bullet.setActive(false);
                                if (!playerTank1->isAlive()) {
                                    cout << "Player 1 is destroyed by enemy!" << endl;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void Game::spawnHpItem() {
    if (!hpItemActive) {
        hpItemRect.x = rand() % (SCREEN_WIDTH - hpItemRect.w);
        hpItemRect.y = rand() % (SCREEN_HEIGHT - hpItemRect.h);

        while (map->isColliding(hpItemRect)) {
            hpItemRect.x = rand() % (SCREEN_WIDTH - hpItemRect.w);
            hpItemRect.y = rand() % (SCREEN_HEIGHT - hpItemRect.h);
        }

        hpItemActive = true;
        lastHpItemSpawnTime = SDL_GetTicks();
    }
}

void Game::checkHpItemCollision() {
    if (!hpItemActive) return;

    SDL_Rect tank1Rect = playerTank1->getRect();
    if (playerTank1->isAlive() && SDL_HasIntersection(&hpItemRect, &tank1Rect)) {
        if (playerTank1->getHP() < playerTank1->getMaxHP()) {
            playerTank1->increaseHP();
            hpItemActive = false;
            lastHpItemSpawnTime = SDL_GetTicks();
        }
    }

    if (isTwoPlayerMode) {
        SDL_Rect tank2Rect = playerTank2->getRect();
        if (playerTank2->isAlive() && SDL_HasIntersection(&hpItemRect, &tank2Rect)) {
            if (playerTank2->getHP() < playerTank2->getMaxHP()) {
                playerTank2->increaseHP();
                hpItemActive = false;
                lastHpItemSpawnTime = SDL_GetTicks();
            }
        }
    }
}

void Game::createGameOverMenu() {
}

void Game::createModeSelectionMenu() {
    SDL_Surface* player1Surface = IMG_Load("Player1.png");
    if (player1Surface) {
        player1Texture = SDL_CreateTextureFromSurface(renderer, player1Surface);
        SDL_FreeSurface(player1Surface);
    } else {
        cerr << "Failed to load Player1.png! IMG_Error: " << IMG_GetError() << endl;
    }

    SDL_Surface* player2Surface = IMG_Load("Player2.png");
    if (player2Surface) {
        player2Texture = SDL_CreateTextureFromSurface(renderer, player2Surface);
        SDL_FreeSurface(player2Surface);
    } else {
        cerr << "Failed to load Player2.png! IMG_Error: " << IMG_GetError() << endl;
    }
}

int Game::handleGameOverInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return 1;
        }
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                    selectedOption = 0;
                    break;
                case SDLK_DOWN:
                    selectedOption = 1;
                    break;
                case SDLK_RETURN:
                    return selectedOption;
            }
        }
    }
    return -1;
}

int Game::showMainMenu() {
    bool inMainMenu = true;
    selectedOption = 0;

    SDL_Rect startButtonRect = {(SCREEN_WIDTH - 200) / 2, 200, 200, 50};
    SDL_Rect exitButtonRect = {(SCREEN_WIDTH - 200) / 2, 300, 200, 50};
    soundButtonRect = {(SCREEN_WIDTH - 200) / 2, 400, 200, 50}; // Đảm bảo vị trí và kích thước hợp lý

    while (inMainMenu && running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                return -1;
            }
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        selectedOption = (selectedOption == 0) ? 1 : 0;
                        break;
                    case SDLK_DOWN:
                        selectedOption = (selectedOption == 0) ? 1 : 0;
                        break;
                    case SDLK_RETURN:
                        if (selectedOption == 0) {
                            inMainMenu = false;
                            return 0; // Chuyển sang màn hình chọn chế độ chơi
                        } else if (selectedOption == 1) {
                            running = false;
                            return -1; // Thoát trò chơi
                        }
                        break;
                }
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                SDL_Point mousePoint = {x, y};
                cout << "Mouse clicked at: (" << x << ", " << y << ")" << endl; // Debug tọa độ chuột
                if (SDL_PointInRect(&mousePoint, &startButtonRect)) {
                    inMainMenu = false;
                    return 0; // Chuyển sang màn hình chọn chế độ chơi
                }
                if (SDL_PointInRect(&mousePoint, &exitButtonRect)) {
                    running = false;
                    return -1; // Thoát trò chơi
                }
                if (SDL_PointInRect(&mousePoint, &soundButtonRect)) {
                    setSoundState(!isSoundOn); // Đổi trạng thái âm thanh
                    cout << "Sound button clicked! Sound is now: " << (isSoundOn ? "ON" : "OFF") << endl; // Debug
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (backgroundTexture) {
            SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
        }

        if (startButtonTexture) {
            SDL_SetTextureColorMod(startButtonTexture, selectedOption == 0 ? 255 : 128, 255, 255);
            SDL_RenderCopy(renderer, startButtonTexture, nullptr, &startButtonRect);
        }

        if (exitButtonTexture) {
            SDL_SetTextureColorMod(exitButtonTexture, selectedOption == 1 ? 255 : 128, 255, 255);
            SDL_RenderCopy(renderer, exitButtonTexture, nullptr, &exitButtonRect);
        }

        if (soundButtonTexture) {
            SDL_SetTextureColorMod(soundButtonTexture, isSoundOn ? 255 : 128, 255, 255);
            SDL_RenderCopy(renderer, soundButtonTexture, nullptr, &soundButtonRect);
        } else {
            cerr << "Sound button texture is not loaded!" << endl; // Debug
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    return -1;
}

int Game::showModeSelectionScreen() {
    bool inModeSelection = true;
    selectedOption = 0;

    SDL_Rect player1Rect = {(SCREEN_WIDTH - 200) / 2, 200, 200, 50};
    SDL_Rect player2Rect = {(SCREEN_WIDTH - 200) / 2, 300, 200, 50};

    while (inModeSelection && running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                return -1;
            }
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        selectedOption = 0;
                        break;
                    case SDLK_DOWN:
                        selectedOption = 1;
                        break;
                    case SDLK_RETURN:
                        inModeSelection = false;
                        return selectedOption;
                }
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                SDL_Point mousePoint = {x, y};
                if (SDL_PointInRect(&mousePoint, &player1Rect)) {
                    inModeSelection = false;
                    return 0; // Chọn chế độ 1 người chơi
                }
                if (SDL_PointInRect(&mousePoint, &player2Rect)) {
                    inModeSelection = false;
                    return 1; // Chọn chế độ 2 người chơi
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (backgroundTexture) {
            SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
        }

        if (player1Texture) {
            SDL_SetTextureColorMod(player1Texture, selectedOption == 0 ? 255 : 128, 255, 255);
            SDL_RenderCopy(renderer, player1Texture, nullptr, &player1Rect);
        }

        if (player2Texture) {
            SDL_SetTextureColorMod(player2Texture, selectedOption == 1 ? 255 : 128, 255, 255);
            SDL_RenderCopy(renderer, player2Texture, nullptr, &player2Rect);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    return -1;
}

void Game::resetGame() {
    delete playerTank1;
    delete playerTank2;
    for (auto enemy : enemyTanks) {
        delete enemy;
    }
    enemyTanks.clear();

    playerTank1 = new Tank(770, 0, SDL_Color{0, 255, 0, 255}, renderer, true);
    playerTank2 = new Tank(0, 570, SDL_Color{255, 0, 0, 255}, renderer, false);
    playerTank2->setAI(!isTwoPlayerMode);

    spawnEnemyTanks();

    map->reset();
    hpItemActive = false;
    lastHpItemSpawnTime = 0;
    lastEnemySpawnTime = SDL_GetTicks();
    score = 0;
    updateScoreTexture();
}

void Game::showGameOverScreen() {
    Mix_HaltMusic();
    if (winnerSound && isSoundOn) {
        Mix_PlayMusic(winnerSound, 0);
    }

    bool inGameOver = true;
    selectedOption = 0;

    while (inGameOver) {
        int result = handleGameOverInput();
        if (result == 0) {
            resetGame();
            inGameOver = false;
            runGame();
            return;
        } else if (result == 1) {
            running = false;
            inGameOver = false;
            return;
        }

        SDL_RenderClear(renderer);
        if (winnerTexture) {
            SDL_RenderCopy(renderer, winnerTexture, nullptr, nullptr);
        }

        if (startButtonTexture) {
            int textW, textH;
            SDL_QueryTexture(startButtonTexture, nullptr, nullptr, &textW, &textH);
            SDL_Rect startButtonRect = {(SCREEN_WIDTH - 200) / 2, 300, 200, 50};
            SDL_SetTextureColorMod(startButtonTexture, selectedOption == 0 ? 255 : 128, 255, 255);
            SDL_RenderCopy(renderer, startButtonTexture, nullptr, &startButtonRect);
        }

        if (exitButtonTexture) {
            int textW, textH;
            SDL_QueryTexture(exitButtonTexture, nullptr, nullptr, &textW, &textH);
            SDL_Rect exitButtonRect = {(SCREEN_WIDTH - 200) / 2, 400, 200, 50};
            SDL_SetTextureColorMod(exitButtonTexture, selectedOption == 1 ? 255 : 128, 255, 255);
            SDL_RenderCopy(renderer, exitButtonTexture, nullptr, &exitButtonRect);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}

void Game::updateTimerTexture() {
    if (!font) return;

    Uint32 currentTime = SDL_GetTicks();
    int elapsedSeconds = (currentTime - gameStartTime) / 1000;

    stringstream ss;
    ss << "Time: " << elapsedSeconds;
    string timerText = ss.str();

    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, timerText.c_str(), textColor);
    if (!textSurface) {
        cerr << "Failed to create text surface! TTF_Error: " << TTF_GetError() << endl;
        return;
    }

    if (timerTexture) {
        SDL_DestroyTexture(timerTexture);
    }
    timerTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);

    if (!timerTexture) {
        cerr << "Failed to create timer texture! SDL_Error: " << SDL_GetError() << endl;
    }
}

void Game::runGame() {
    Mix_HaltMusic();
    if (gameMusic && isSoundOn) {
        Mix_PlayMusic(gameMusic, -1);
    }

    SDL_Event event;
    gameStartTime = SDL_GetTicks();

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (playerTank1->isAlive()) {
                playerTank1->handleInput(event, *map, true, shootSound);
            }
            if (isTwoPlayerMode && playerTank2->isAlive()) {
                playerTank2->handleInput(event, *map, false, shootSound);
            }
        }

        Uint32 currentTime = SDL_GetTicks();
        if (!hpItemActive) {
            if (currentTime - gameStartTime >= HP_SPAWN_INTERVAL && lastHpItemSpawnTime == 0) {
                spawnHpItem();
            } else if (lastHpItemSpawnTime > 0 && currentTime - lastHpItemSpawnTime >= HP_SPAWN_INTERVAL) {
                spawnHpItem();
            }
        }

        if (!isTwoPlayerMode && currentTime - lastEnemySpawnTime >= ENEMY_SPAWN_INTERVAL) {
            spawnSingleEnemyTank();
            lastEnemySpawnTime = currentTime;
        }

        if (playerTank1->isAlive()) {
            playerTank1->update(*map, *playerTank2);

        }
        if (isTwoPlayerMode && playerTank2->isAlive()) {
            if (!isTwoPlayerMode) {
                playerTank2->handleAI(*map, *playerTank1, shootSound);
            }
            playerTank2->update(*map, *playerTank1);
        }
        if (!isTwoPlayerMode) {
            for (auto enemy : enemyTanks) {
                if (enemy->isAlive()) {
                    enemy->handleAI(*map, *playerTank1, shootSound);
                    enemy->update(*map, *playerTank1);
                } else if (enemy->getIsExploding()) {
                    enemy->update(*map, *playerTank1);
                }
            }

            enemyTanks.erase(
                remove_if(enemyTanks.begin(), enemyTanks.end(),
                    [](EnemyTank* enemy) {
                        if (enemy->canBeRemoved()) {
                            cout << "Removing enemy tank after explosion!" << endl;
                            delete enemy;
                            return true;
                        }
                        return false;
                    }),
                enemyTanks.end()
            );
        }

        checkBulletCollisions();
        checkHpItemCollision();

        updateTimerTexture();

        bool allEnemiesDead = true;
        if (!isTwoPlayerMode) {
            for (auto enemy : enemyTanks) {
                if (enemy->isAlive()) {
                    allEnemiesDead = false;
                    break;
                }
            }
        }
        if (!playerTank1->isAlive() || (allEnemiesDead && !isTwoPlayerMode)) {
            showGameOverScreen();
            return;
        }
        if (isTwoPlayerMode && !playerTank2->isAlive()) {
            showGameOverScreen();
            return;
        }

        render();
        SDL_Delay(16);
    }
}

void Game::run() {
    int mainMenuResult = showMainMenu();
    if (mainMenuResult == -1 || !running) {
        running = false;
        return;
    }

    int mode = showModeSelectionScreen();
    if (mode == -1 || !running) {
        running = false;
        return;
    }

    isTwoPlayerMode = (mode == 1);
    playerTank2->setAI(!isTwoPlayerMode);

    if (!isTwoPlayerMode) {
        spawnEnemyTanks();
    }

    runGame();
}

void Game::updateScoreTexture() {
    if (!font) return;

    stringstream ss;
    ss << "Score: " << score;
    string scoreText = ss.str();

    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
    if (!textSurface) {
        cerr << "Failed to create text surface! TTF_Error: " << TTF_GetError() << endl;
        return;
    }

    if (scoreTexture) {
        SDL_DestroyTexture(scoreTexture);
    }
    scoreTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);

    if (!scoreTexture) {
        cerr << "Failed to create score texture! SDL_Error: " << SDL_GetError() << endl;
    }
}

void Game::setSoundState(bool state) {
    isSoundOn = state;
    cout << "Setting sound state to: " << (isSoundOn ? "ON" : "OFF") << endl; // Debug
    if (isSoundOn) {
        Mix_Volume(-1, MIX_MAX_VOLUME); // Đặt âm lượng tối đa
        Mix_VolumeChunk(shootSound, MIX_MAX_VOLUME);
        if (Mix_PausedMusic()) {
            Mix_ResumeMusic();
            cout << "Resuming music" << endl; // Debug
        } else if (!Mix_PlayingMusic()) {
            Mix_PlayMusic(menuMusic, -1);
            cout << "Playing menu music" << endl; // Debug
        }
    } else {
        Mix_Volume(-1, 0); // Tắt âm lượng
        Mix_VolumeChunk(shootSound, 0);
        if (Mix_PlayingMusic()) {
            Mix_PauseMusic();
            cout << "Pausing music" << endl; // Debug
        }
    }
}

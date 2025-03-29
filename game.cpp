#include "Game.h"
#include <iostream>

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

    if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) == 0) {
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

    window = SDL_CreateWindow("Battle City - 2 Players", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        running = false;
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
        Mix_Quit();
        IMG_Quit();
        SDL_Quit();
        return;
    }

    playerTank1 = new Tank(0, 0, SDL_Color{0, 255, 0, 255}, renderer, true);
    if (!playerTank1) {
        cerr << "Failed to allocate memory for playerTank1!" << endl;
        running = false;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        Mix_Quit();
        IMG_Quit();
        SDL_Quit();
        return;
    }
    playerTank2 = new Tank(800, 800, SDL_Color{255, 0, 0, 255}, renderer, false);
    if (!playerTank2) {
        cerr << "Failed to allocate memory for playerTank2!" << endl;
        delete playerTank1;
        running = false;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        Mix_Quit();
        IMG_Quit();
        SDL_Quit();
        return;
    }

    gameMusic = Mix_LoadMUS("background.mp3");
    if (!gameMusic) {
        cerr << "Failed to load game music (background.mp3)! Mix_Error: " << Mix_GetError() << endl;
    }

    menuMusic = Mix_LoadMUS("backgroundMenu.mp3");
    if (!menuMusic) {
        cerr << "Failed to load menu music (backgroundMenu.mp3)! Mix_Error: " << Mix_GetError() << endl;
    }

    shootSound = Mix_LoadWAV("shoot.wav");
    if (!shootSound) {
        cerr << "Failed to load shoot sound (shoot.wav)! Mix_Error: " << Mix_GetError() << endl;
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

    winnerSound = Mix_LoadMUS("winner.mp3");
    if (!winnerSound) {
        cerr << "Failed to load winner.mp3! Mix_Error: " << Mix_GetError() << endl;
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

    SDL_Surface* hpItemSurface = IMG_Load("hp_item.jpg");
    if (!hpItemSurface) {
        cerr << "Failed to load hp_item.png! IMG_Error: " << IMG_GetError() << endl;
    } else {
        hpItemTexture = SDL_CreateTextureFromSurface(renderer, hpItemSurface);
        SDL_FreeSurface(hpItemSurface);
        if (!hpItemTexture) {
            cerr << "Failed to create texture from hp_item.png! SDL_Error: " << SDL_GetError() << endl;
        }
    }
    hpItemRect = { 0, 0, 50, 50 };
    hpItemActive = false;

    createGameOverMenu();
    createModeSelectionMenu();
}

Game::~Game() {
    delete playerTank1;
    delete playerTank2;
    if (gameMusic) {
        Mix_FreeMusic(gameMusic);
        gameMusic = nullptr;
    }
    if (menuMusic) {
        Mix_FreeMusic(menuMusic);
        menuMusic = nullptr;
    }
    if (shootSound) {
        Mix_FreeChunk(shootSound);
        shootSound = nullptr;
    }
    if (winnerTexture) {
        SDL_DestroyTexture(winnerTexture);
        winnerTexture = nullptr;
    }
    if (winnerSound) {
        Mix_FreeMusic(winnerSound);
        winnerSound = nullptr;
    }
    if (startButtonTexture) {
        SDL_DestroyTexture(startButtonTexture);
        startButtonTexture = nullptr;
    }
    if (exitButtonTexture) {
        SDL_DestroyTexture(exitButtonTexture);
        exitButtonTexture = nullptr;
    }
    if (player1Texture) {
        SDL_DestroyTexture(player1Texture);
        player1Texture = nullptr;
    }
    if (player2Texture) {
        SDL_DestroyTexture(player2Texture);
        player2Texture = nullptr;
    }
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = nullptr;
    }
    if (hpItemTexture) {
        SDL_DestroyTexture(hpItemTexture);
        hpItemTexture = nullptr;
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    Mix_CloseAudio();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
    SDL_RenderClear(renderer);

    map.draw(renderer);
    if (playerTank1->isAlive()) playerTank1->draw(renderer);
    if (playerTank2->isAlive()) playerTank2->draw(renderer);

    if (hpItemActive && hpItemTexture) {
        SDL_RenderCopy(renderer, hpItemTexture, nullptr, &hpItemRect);
    }

    SDL_RenderPresent(renderer);
}

void Game::checkBulletCollisions() {
    if (playerTank2->isAlive()) {
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

    if (playerTank1->isAlive()) {
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
}

void Game::createGameOverMenu() {
    // Không có play_again.png, chỉ tải exit_button.jpg
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

int Game::showModeSelectionScreen() {
    bool inModeSelection = true;
    selectedOption = 0;

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
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (backgroundTexture) {
            SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
        }

        if (player1Texture) {
            int textW, textH;
            SDL_QueryTexture(player1Texture, nullptr, nullptr, &textW, &textH);
            SDL_Rect player1Rect = { (SCREEN_WIDTH - 200) / 2, 200, 200, 50 };
            SDL_SetTextureColorMod(player1Texture, selectedOption == 0 ? 255 : 128, 255, 255);
            SDL_RenderCopy(renderer, player1Texture, nullptr, &player1Rect);
        }

        if (player2Texture) {
            int textW, textH;
            SDL_QueryTexture(player2Texture, nullptr, nullptr, &textW, &textH);
            SDL_Rect player2Rect = { (SCREEN_WIDTH - 200) / 2, 300, 200, 50 };
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
    playerTank1 = new Tank(800, 0, SDL_Color{0, 255, 0, 255}, renderer, true);
    playerTank2 = new Tank(0, 800, SDL_Color{255, 0, 0, 255}, renderer, false);
    playerTank2->setAI(!isTwoPlayerMode);
    map = Map();
    hpItemActive = false;
    lastHpItemSpawnTime = 0;
}

void Game::showGameOverScreen() {
    Mix_HaltMusic();
    if (winnerSound) {
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
            SDL_Rect startButtonRect = { (SCREEN_WIDTH - 200) / 2, 300, 200, 50 };
            SDL_SetTextureColorMod(startButtonTexture, selectedOption == 0 ? 255 : 128, 255, 255);
            SDL_RenderCopy(renderer, startButtonTexture, nullptr, &startButtonRect);
        }

        if (exitButtonTexture) {
            int textW, textH;
            SDL_QueryTexture(exitButtonTexture, nullptr, nullptr, &textW, &textH);
            SDL_Rect exitButtonRect = { (SCREEN_WIDTH - 200) / 2, 400, 200, 50 };
            SDL_SetTextureColorMod(exitButtonTexture, selectedOption == 1 ? 255 : 128, 255, 255);
            SDL_RenderCopy(renderer, exitButtonTexture, nullptr, &exitButtonRect);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}

void Game::spawnHpItem() {
    if (!hpItemActive) {
        hpItemRect.x = rand() % (SCREEN_WIDTH - hpItemRect.w);
        hpItemRect.y = rand() % (SCREEN_HEIGHT - hpItemRect.h);

        while (map.isColliding(hpItemRect)) {
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

    SDL_Rect tank2Rect = playerTank2->getRect();
    if (playerTank2->isAlive() && SDL_HasIntersection(&hpItemRect, &tank2Rect)) {
        if (playerTank2->getHP() < playerTank2->getMaxHP()) {
            playerTank2->increaseHP();
            hpItemActive = false;
            lastHpItemSpawnTime = SDL_GetTicks();
        }
    }
}

void Game::runGame() {
    if (gameMusic) {
        Mix_PlayMusic(gameMusic, -1);
    }

    SDL_Event event;
    Uint32 gameStartTime = SDL_GetTicks();
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (playerTank1->isAlive()) {
                playerTank1->handleInput(event, map, true, shootSound);
            }
            if (playerTank2->isAlive() && isTwoPlayerMode) {
                playerTank2->handleInput(event, map, false, shootSound);
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

        if (playerTank1->isAlive()) {
            playerTank1->update(map, *playerTank2);
        }
        if (playerTank2->isAlive()) {
            if (!isTwoPlayerMode) {
                playerTank2->handleAI(map, *playerTank1, shootSound);
            }
            playerTank2->update(map, *playerTank1);
        }

        checkBulletCollisions();
        checkHpItemCollision();

        if (!playerTank1->isAlive() || !playerTank2->isAlive()) {
            showGameOverScreen();
            return;
        }

        render();
        SDL_Delay(16);
    }
}

void Game::run() {
    Menu menu(renderer);
    bool inMenu = true;

    if (menuMusic) {
        Mix_PlayMusic(menuMusic, -1);
    }

    while (inMenu && running) {
        int result = menu.handleInput();
        if (result == 0) {
            Mix_HaltMusic();
            int mode = showModeSelectionScreen();
            if (mode == 0) {
                isTwoPlayerMode = false;
                inMenu = false;
                runGame();
            } else if (mode == 1) {
                isTwoPlayerMode = true;
                inMenu = false;
                runGame();
            } else if (mode == -1) {
                running = false;
                inMenu = false;
            }
        } else if (result == 1) {
            Mix_HaltMusic();
            running = false;
            inMenu = false;
        }
        menu.draw();
        SDL_Delay(16);
    }
}

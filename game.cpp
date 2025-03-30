
#include "Game.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

// Constructor: Khởi tạo trò chơi, bao gồm SDL, các thư viện liên quan, và các đối tượng trong game
Game::Game() {
    running = true;

    // Khởi tạo SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        running = false;
        return;
    }

    // Khởi tạo SDL_image
    if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == 0) {
        cerr << "SDL_image could not initialize! IMG_Error: " << IMG_GetError() << endl;
        running = false;
        SDL_Quit();
        return;
    }

    // Khởi tạo SDL_mixer
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

    // Tạo cửa sổ game
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
        Mix_Quit();
        IMG_Quit();
        SDL_Quit();
        return;
    }

    // Tạo renderer
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

    // Khởi tạo bản đồ
    map = new Map(renderer);
    if (!map) {
        cerr << "Failed to allocate memory for map!" << endl;
        running = false;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        Mix_Quit();
        IMG_Quit();
        SDL_Quit();
        return;
    }

    // Khởi tạo xe tăng người chơi
    playerTank1 = new Tank(0, 0, SDL_Color{0, 255, 0, 255}, renderer, true);
    if (!playerTank1) {
        cerr << "Failed to allocate memory for playerTank1!" << endl;
        running = false;
        delete map;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        Mix_Quit();
        IMG_Quit();
        SDL_Quit();
        return;
    }

    playerTank2 = new Tank(800, 600, SDL_Color{255, 0, 0, 255}, renderer, false);
    if (!playerTank2) {
        cerr << "Failed to allocate memory for playerTank2!" << endl;
        delete playerTank1;
        delete map;
        running = false;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        Mix_Quit();
        IMG_Quit();
        SDL_Quit();
        return;
    }

    // Sinh xe tăng địch
    spawnEnemyTanks();

    // Tải âm thanh game
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

    winnerSound = Mix_LoadMUS("winner.mp3");
    if (!winnerSound) {
        cerr << "Failed to load winner.mp3! Mix_Error: " << Mix_GetError() << endl;
    }

    // Tải texture cho màn hình chiến thắng
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

    // Tải texture nền
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

    // Tải texture nút Start
    SDL_Surface* startButtonSurface = IMG_Load("start_button.png");
    if (startButtonSurface) {
        startButtonTexture = SDL_CreateTextureFromSurface(renderer, startButtonSurface);
        SDL_FreeSurface(startButtonSurface);
    } else {
        cerr << "Failed to load start_button.png! IMG_Error: " << IMG_GetError() << endl;
    }

    // Tải texture nút Exit
    SDL_Surface* exitButtonSurface = IMG_Load("exit_button.jpg");
    if (exitButtonSurface) {
        exitButtonTexture = SDL_CreateTextureFromSurface(renderer, exitButtonSurface);
        SDL_FreeSurface(exitButtonSurface);
    } else {
        cerr << "Failed to load exit_button.jpg! IMG_Error: " << IMG_GetError() << endl;
    }

    // Tải texture item HP
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

    // Khởi tạo các menu
    createGameOverMenu();
    createModeSelectionMenu();

    // Khởi tạo seed cho rand()
    srand(static_cast<unsigned int>(time(nullptr)));
}

// Destructor: Giải phóng tất cả tài nguyên đã cấp phát
Game::~Game() {
    delete playerTank1;
    delete playerTank2;
    delete map;

    // Giải phóng xe tăng địch
    for (auto enemy : enemyTanks) {
        delete enemy;
    }
    enemyTanks.clear();

    // Giải phóng âm thanh
    if (gameMusic) Mix_FreeMusic(gameMusic);
    if (menuMusic) Mix_FreeMusic(menuMusic);
    if (shootSound) Mix_FreeChunk(shootSound);
    if (winnerSound) Mix_FreeMusic(winnerSound);

    // Giải phóng texture
    if (winnerTexture) SDL_DestroyTexture(winnerTexture);
    if (startButtonTexture) SDL_DestroyTexture(startButtonTexture);
    if (exitButtonTexture) SDL_DestroyTexture(exitButtonTexture);
    if (player1Texture) SDL_DestroyTexture(player1Texture);
    if (player2Texture) SDL_DestroyTexture(player2Texture);
    if (backgroundTexture) SDL_DestroyTexture(backgroundTexture);
    if (hpItemTexture) SDL_DestroyTexture(hpItemTexture);

    // Giải phóng renderer và window
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);

    // Đóng các thư viện SDL
    Mix_CloseAudio();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}

// Hàm sinh xe tăng địch ở các vị trí hợp lệ trên bản đồ
void Game::spawnEnemyTanks() {
    enemyTanks.clear();  // Xóa các xe tăng địch cũ nếu có
    const int numEnemies = 3;  // Số lượng xe tăng địch
    for (int i = 0; i < numEnemies; i++) {
        int x, y;
        SDL_Rect tempRect;
        bool validPosition = false;

        // Tìm vị trí hợp lệ không va chạm với tường
        while (!validPosition) {
            x = rand() % (SCREEN_WIDTH - 30);
            y = rand() % (SCREEN_HEIGHT - 30);
            tempRect = {x, y, 30, 30};
            if (!map->isColliding(tempRect)) {
                validPosition = true;
            }
        }

        EnemyTank* enemy = new EnemyTank(x, y, renderer);
        if (enemy) {
            enemyTanks.push_back(enemy);
            cout << "Spawned EnemyTank " << i + 1 << " at position (" << x << ", " << y << ")" << endl;
        } else {
            cerr << "Failed to allocate memory for EnemyTank!" << endl;
        }
    }
}

// Hàm vẽ toàn bộ màn hình game
void Game::render() {
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
    SDL_RenderClear(renderer);

    // Vẽ bản đồ
    map->draw(renderer);

    // Vẽ xe tăng người chơi 1
    if (playerTank1->isAlive()) {
        playerTank1->draw(renderer);
    }

    // Vẽ xe tăng người chơi 2 (nếu ở chế độ 2 người chơi)
    if (isTwoPlayerMode && playerTank2->isAlive()) {
        playerTank2->draw(renderer);
    }

    // Vẽ xe tăng địch (nếu ở chế độ 1 người chơi)
    if (!isTwoPlayerMode) {
        for (auto enemy : enemyTanks) {
            if (enemy->isAlive()) {
                enemy->draw(renderer);
            }
        }
    }

    // Vẽ item HP nếu đang hoạt động
    if (hpItemActive && hpItemTexture) {
        SDL_RenderCopy(renderer, hpItemTexture, nullptr, &hpItemRect);
    }

    SDL_RenderPresent(renderer);
}

// Hàm kiểm tra va chạm giữa đạn và xe tăng
void Game::checkBulletCollisions() {
    // Kiểm tra va chạm giữa đạn của Player 1 và Player 2 (nếu ở chế độ 2 người chơi)
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

    // Kiểm tra va chạm giữa đạn của Player 2 và Player 1 (nếu ở chế độ 2 người chơi)
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

    // Kiểm tra va chạm giữa đạn của Player 1 và xe tăng địch (nếu ở chế độ 1 người chơi)
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
                                cout << "Enemy tank destroyed!" << endl;
                            }
                        }
                    }
                }
            }
        }

        // Kiểm tra va chạm giữa đạn của xe tăng địch và Player 1
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

// Hàm sinh item HP ở vị trí ngẫu nhiên
void Game::spawnHpItem() {
    if (!hpItemActive) {
        hpItemRect.x = rand() % (SCREEN_WIDTH - hpItemRect.w);
        hpItemRect.y = rand() % (SCREEN_HEIGHT - hpItemRect.h);

        // Đảm bảo item HP không sinh ra trên tường
        while (map->isColliding(hpItemRect)) {
            hpItemRect.x = rand() % (SCREEN_WIDTH - hpItemRect.w);
            hpItemRect.y = rand() % (SCREEN_HEIGHT - hpItemRect.h);
        }

        hpItemActive = true;
        lastHpItemSpawnTime = SDL_GetTicks();
    }
}

// Hàm kiểm tra va chạm giữa xe tăng và item HP
void Game::checkHpItemCollision() {
    if (!hpItemActive) return;

    // Kiểm tra với Player 1
    SDL_Rect tank1Rect = playerTank1->getRect();
    if (playerTank1->isAlive() && SDL_HasIntersection(&hpItemRect, &tank1Rect)) {
        if (playerTank1->getHP() < playerTank1->getMaxHP()) {
            playerTank1->increaseHP();
            hpItemActive = false;
            lastHpItemSpawnTime = SDL_GetTicks();
        }
    }

    // Kiểm tra với Player 2 (nếu ở chế độ 2 người chơi)
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

// Hàm tạo menu game over (hiện tại chưa sử dụng play_again.png)
void Game::createGameOverMenu() {
    // Tạm thời không làm gì, vì không có play_again.png
}

// Hàm tạo menu chọn chế độ chơi
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

// Hàm xử lý input trong màn hình game over
int Game::handleGameOverInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return 1;  // Thoát game
        }
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                    selectedOption = 0;  // Chọn "Play Again"
                    break;
                case SDLK_DOWN:
                    selectedOption = 1;  // Chọn "Exit"
                    break;
                case SDLK_RETURN:
                    return selectedOption;  // Xác nhận lựa chọn
            }
        }
    }
    return -1;  // Không có lựa chọn
}

// Hàm hiển thị màn hình chọn chế độ chơi
int Game::showModeSelectionScreen() {
    bool inModeSelection = true;
    selectedOption = 0;

    while (inModeSelection && running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                return -1;  // Thoát game
            }
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        selectedOption = 0;  // Chọn chế độ 1 người chơi
                        break;
                    case SDLK_DOWN:
                        selectedOption = 1;  // Chọn chế độ 2 người chơi
                        break;
                    case SDLK_RETURN:
                        inModeSelection = false;
                        return selectedOption;  // Xác nhận lựa chọn
                }
            }
        }

        // Vẽ màn hình chọn chế độ
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (backgroundTexture) {
            SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
        }

        if (player1Texture) {
            int textW, textH;
            SDL_QueryTexture(player1Texture, nullptr, nullptr, &textW, &textH);
            SDL_Rect player1Rect = {(SCREEN_WIDTH - 200) / 2, 200, 200, 50};
            SDL_SetTextureColorMod(player1Texture, selectedOption == 0 ? 255 : 128, 255, 255);
            SDL_RenderCopy(renderer, player1Texture, nullptr, &player1Rect);
        }

        if (player2Texture) {
            int textW, textH;
            SDL_QueryTexture(player2Texture, nullptr, nullptr, &textW, &textH);
            SDL_Rect player2Rect = {(SCREEN_WIDTH - 200) / 2, 300, 200, 50};
            SDL_SetTextureColorMod(player2Texture, selectedOption == 1 ? 255 : 128, 255, 255);
            SDL_RenderCopy(renderer, player2Texture, nullptr, &player2Rect);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);  // Giới hạn khung hình (khoảng 60 FPS)
    }
    return -1;  // Thoát game
}

// Hàm reset trạng thái game để chơi lại
void Game::resetGame() {
    // Xóa xe tăng cũ
    delete playerTank1;
    delete playerTank2;
    for (auto enemy : enemyTanks) {
        delete enemy;
    }
    enemyTanks.clear();

    // Tạo lại xe tăng người chơi
    playerTank1 = new Tank(800, 0, SDL_Color{0, 255, 0, 255}, renderer, true);
    playerTank2 = new Tank(0, 800, SDL_Color{255, 0, 0, 255}, renderer, false);
    playerTank2->setAI(!isTwoPlayerMode);

    // Sinh lại xe tăng địch
    if (!isTwoPlayerMode) {
        spawnEnemyTanks();
    }

    // Reset bản đồ và item HP
    map->reset();
    hpItemActive = false;
    lastHpItemSpawnTime = 0;
}

// Hàm hiển thị màn hình game over
void Game::showGameOverScreen() {
    Mix_HaltMusic();
    if (winnerSound) {
        Mix_PlayMusic(winnerSound, 0);
    }

    bool inGameOver = true;
    selectedOption = 0;

    while (inGameOver) {
        int result = handleGameOverInput();
        if (result == 0) {  // Play Again
            resetGame();
            inGameOver = false;
            runGame();
            return;
        } else if (result == 1) {  // Exit
            running = false;
            inGameOver = false;
            return;
        }

        // Vẽ màn hình game over
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

// Hàm chạy vòng lặp chính của game
void Game::runGame() {
    if (gameMusic) {
        Mix_PlayMusic(gameMusic, -1);
    }

    SDL_Event event;
    Uint32 gameStartTime = SDL_GetTicks();

    while (running) {
        // Xử lý input
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

        // Sinh item HP theo thời gian
        Uint32 currentTime = SDL_GetTicks();
        if (!hpItemActive) {
            if (currentTime - gameStartTime >= HP_SPAWN_INTERVAL && lastHpItemSpawnTime == 0) {
                spawnHpItem();
            } else if (lastHpItemSpawnTime > 0 && currentTime - lastHpItemSpawnTime >= HP_SPAWN_INTERVAL) {
                spawnHpItem();
            }
        }

        // Cập nhật trạng thái xe tăng
        if (playerTank1->isAlive()) {
            playerTank1->update(*map, *playerTank2);
        }
        if (isTwoPlayerMode && playerTank2->isAlive()) {
            if (!isTwoPlayerMode) {
                playerTank2->handleAI(*map, *playerTank1, shootSound);
            }
            playerTank2->update(*map, *playerTank1);
        }

        // Cập nhật xe tăng địch (nếu ở chế độ 1 người chơi)
        if (!isTwoPlayerMode) {
            for (auto enemy : enemyTanks) {
                if (enemy->isAlive()) {
                    enemy->handleAI(*map, *playerTank1, shootSound);
                    enemy->update(*map, *playerTank1);
                }
            }
        }

        // Kiểm tra va chạm
        checkBulletCollisions();
        checkHpItemCollision();

        // Kiểm tra điều kiện kết thúc game
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

        // Vẽ màn hình
        render();
        SDL_Delay(16);  // Giới hạn khung hình
    }
}

// Hàm chạy vòng lặp chính của chương trình
void Game::run() {
    Menu menu(renderer);
    bool inMenu = true;

    if (menuMusic) {
        Mix_PlayMusic(menuMusic, -1);
    }

    while (inMenu && running) {
        int result = menu.handleInput();
        if (result == 0) {  // Start game
            Mix_HaltMusic();
            int mode = showModeSelectionScreen();
            if (mode == 0) {  // Chế độ 1 người chơi
                isTwoPlayerMode = false;
                inMenu = false;
                runGame();
            } else if (mode == 1) {  // Chế độ 2 người chơi
                isTwoPlayerMode = true;
                inMenu = false;
                runGame();
            } else if (mode == -1) {  // Thoát
                running = false;
                inMenu = false;
            }
        } else if (result == 1) {  // Exit game
            Mix_HaltMusic();
            running = false;
            inMenu = false;
        }
        menu.draw();
        SDL_Delay(16);
    }
}

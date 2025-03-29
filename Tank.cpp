#include "Tank.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

Tank::Tank(int x, int y, SDL_Color tankColor, SDL_Renderer* renderer, bool isPlayer1) {
    rect = { x, y, 30, 30 };
    direction = UP;
    speed = 2;
    isMoving = false;
    hp = MAX_HP;
    color = tankColor;

    const char* imagePath = isPlayer1 ? "tank1.jpg" : "tank2.png";
    SDL_Surface* surface = IMG_Load(imagePath);
    if (!surface) {
        std::cerr << "Failed to load image: " << imagePath << "! IMG_Error: " << IMG_GetError() << std::endl;
        texture = nullptr;
    } else {
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if (!texture) {
            std::cerr << "Failed to create texture from surface! SDL_Error: " << SDL_GetError() << std::endl;
        }
    }

    srand(static_cast<unsigned int>(time(nullptr)));
}

Tank::~Tank() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}

void Tank::handleInput(SDL_Event& event, Map& map, bool isPlayer1, Mix_Chunk* shootSound) {
    if (event.type == SDL_KEYDOWN) {
        if (isPlayer1) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                    direction = UP;
                    isMoving = true;
                    break;
                case SDLK_DOWN:
                    direction = DOWN;
                    isMoving = true;
                    break;
                case SDLK_LEFT:
                    direction = LEFT;
                    isMoving = true;
                    break;
                case SDLK_RIGHT:
                    direction = RIGHT;
                    isMoving = true;
                    break;
                case SDLK_k:  // Tank1 bắn bằng Enter
                    shoot(map, shootSound);
                    break;
            }
        } else {
            switch (event.key.keysym.sym) {
                case SDLK_w:
                    direction = UP;
                    isMoving = true;
                    break;
                case SDLK_s:
                    direction = DOWN;
                    isMoving = true;
                    break;
                case SDLK_a:
                    direction = LEFT;
                    isMoving = true;
                    break;
                case SDLK_d:
                    direction = RIGHT;
                    isMoving = true;
                    break;
                case SDLK_SPACE:  // Tank2 bắn bằng Space
                    shoot(map, shootSound);
                    break;
            }
        }
    } else if (event.type == SDL_KEYUP) {
        if (isPlayer1) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                case SDLK_DOWN:
                case SDLK_LEFT:
                case SDLK_RIGHT:
                    isMoving = false;
                    break;
            }
        } else {
            switch (event.key.keysym.sym) {
                case SDLK_w:
                case SDLK_s:
                case SDLK_a:
                case SDLK_d:
                    isMoving = false;
                    break;
            }
        }
    }
}

void Tank::handleAI(Map& map, const Tank& otherTank, Mix_Chunk* shootSound) {
    if (!isAI) return;

    Uint32 currentTime = SDL_GetTicks();

    if (currentTime - lastDirectionChange > 2000) {
        int randomDirection = rand() % 4;
        direction = static_cast<Direction>(randomDirection);
        isMoving = true;
        lastDirectionChange = currentTime;
    }

    if (rand() % 100 < 1 && currentTime - lastShootTime > 1000) {
        shoot(map, shootSound);
        lastShootTime = currentTime;
    }
}

void Tank::shoot(Map& map, Mix_Chunk* shootSound) {
    int bulletX, bulletY;
    switch (direction) {
        case UP:
            bulletX = rect.x + rect.w / 2 - 2;
            bulletY = rect.y - 5;
            break;
        case DOWN:
            bulletX = rect.x + rect.w / 2 - 2;
            bulletY = rect.y + rect.h;
            break;
        case LEFT:
            bulletX = rect.x - 5;
            bulletY = rect.y + rect.h / 2 - 2;
            break;
        case RIGHT:
            bulletX = rect.x + rect.w;
            bulletY = rect.y + rect.h / 2 - 2;
            break;
    }
    SDL_Rect bulletRect = { bulletX, bulletY,10 , 5 };
    bool destroyWall = false;
    if (!map.handleBulletCollision(bulletRect, destroyWall)) {
        bullets.emplace_back(bulletX, bulletY, direction);
        if (shootSound) {
            Mix_PlayChannel(-1, shootSound, 0);
        }
    }
}

void Tank::update(Map& map, const Tank& otherTank) {
    if (isMoving) {
        int newX = rect.x;
        int newY = rect.y;

        switch (direction) {
            case UP:
                newY -= speed;
                break;
            case DOWN:
                newY += speed;
                break;
            case LEFT:
                newX -= speed;
                break;
            case RIGHT:
                newX += speed;
                break;
        }

        SDL_Rect newRect = { newX, newY, rect.w, rect.h };
        if (!map.isColliding(newRect) && !isCollidingWithTank(newRect, otherTank.getRect())) {
            rect.x = newX;
            rect.y = newY;
        }

        if (rect.x < 0) rect.x = 0;
        if (rect.x > 800 - rect.w) rect.x = 800 - rect.w;
        if (rect.y < 0) rect.y = 0;
        if (rect.y > 600 - rect.h) rect.y = 600 - rect.h;
    }

    for (auto& bullet : bullets) {
        bullet.update(map);
    }

    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
            [](const Bullet& b) { return !b.isActive(); }),
        bullets.end()
    );
}

bool Tank::isCollidingWithTank(const SDL_Rect& newRect, const SDL_Rect& otherTankRect) const {
    return (newRect.x < otherTankRect.x + otherTankRect.w &&
            newRect.x + newRect.w > otherTankRect.x &&
            newRect.y < otherTankRect.y + otherTankRect.h &&
            newRect.y + newRect.h > otherTankRect.y);
}

void Tank::draw(SDL_Renderer* renderer) {
    if (texture) {
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
    } else {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &rect);
    }

    for (auto& bullet : bullets) {
        bullet.draw(renderer);
    }

    drawHealthBar(renderer);
}

void Tank::drawHealthBar(SDL_Renderer* renderer) {
    int healthBarWidth = rect.w;
    int healthBarHeight = 5;
    int healthBarX = rect.x;
    int healthBarY = rect.y - healthBarHeight - 2;

    float healthRatio = static_cast<float>(hp) / MAX_HP;
    int filledWidth = static_cast<int>(healthBarWidth * healthRatio);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect backgroundRect = { healthBarX, healthBarY, healthBarWidth, healthBarHeight };
    SDL_RenderFillRect(renderer, &backgroundRect);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_Rect healthRect = { healthBarX, healthBarY, filledWidth, healthBarHeight };
    SDL_RenderFillRect(renderer, &healthRect);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &backgroundRect);
}

SDL_Rect Tank::getRect() const {
    return rect;
}

std::vector<Bullet>& Tank::getBullets() {
    return bullets;
}

int Tank::getHP() const {
    return hp;
}

void Tank::takeDamage() {
    if (hp > 0) hp--;
}

bool Tank::isAlive() const {
    return hp > 0;
}

void Tank::setAI(bool value) {
    isAI = value;
}

void Tank::increaseHP() {
    if (hp < MAX_HP) {
        hp++;
    }
}

int Tank::getMaxHP() const {
    return MAX_HP;
}

#ifndef ENEMYTANK_H
#define ENEMYTANK_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>
#include <algorithm>
#include <ctime>
#include "Map.h"
#include "Bullet.h"

using namespace std;

class EnemyTank {
private:
    SDL_Rect rect;
    Direction direction;
    int speed;
    vector<Bullet> bullets;
    bool isMoving;
    int hp;
    SDL_Texture* texture;
    static const int MAX_HP = 3;
    Uint32 lastDirectionChange = 0;
    Uint32 lastShootTime = 0;
    int shootDelay;  // Khoảng thời gian chờ bắn ngẫu nhiên cho mỗi xe tăng

public:
    EnemyTank(int x, int y, SDL_Renderer* renderer) {
        rect = { x, y, 30, 30 };
        direction = static_cast<Direction>(rand() % 4);
        speed = 2;
        isMoving = true;
        hp = MAX_HP;
        shootDelay = 1000 + (rand() % 2000);  // Khoảng thời gian chờ bắn từ 1-3 giây

        // Tải ảnh enemytank.jpg
        SDL_Surface* surface = IMG_Load("enemytank.jpg");
        if (!surface) {
            cerr << "Failed to load enemytank.jpg! IMG_Error: " << IMG_GetError() << endl;
            texture = nullptr;
        } else {
            texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);
            if (!texture) {
                cerr << "Failed to create texture from enemytank.jpg! SDL_Error: " << SDL_GetError() << endl;
            }
        }

        srand(static_cast<unsigned int>(time(nullptr)));
    }

    ~EnemyTank() {
        if (texture) {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
    }

    void handleAI(Map& map, const Tank& playerTank, Mix_Chunk* shootSound) {
        Uint32 currentTime = SDL_GetTicks();

        // Thay đổi hướng ngẫu nhiên sau mỗi 2 giây
        if (currentTime - lastDirectionChange > 2000) {
            direction = static_cast<Direction>(rand() % 4);
            isMoving = true;
            lastDirectionChange = currentTime;
        }

        // Bắn ngẫu nhiên với xác suất cao hơn để dễ thấy
        if (currentTime - lastShootTime > shootDelay && rand() % 100 < 20) {  // Tăng xác suất bắn lên 20%
            shoot(map, shootSound);
            lastShootTime = currentTime;
            shootDelay = 1000 + (rand() % 2000);  // Đặt lại thời gian chờ bắn ngẫu nhiên
        }
    }

    void shoot(Map& map, Mix_Chunk* shootSound) {
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
        SDL_Rect bulletRect = { bulletX, bulletY, 10, 5 };
        bool destroyWall = false;
        if (!map.handleBulletCollision(bulletRect, destroyWall)) {
            bullets.emplace_back(bulletX, bulletY, direction);
            if (shootSound) {
                Mix_PlayChannel(-1, shootSound, 0);
            }
        }
    }

    void update(Map& map, const Tank& playerTank) {
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
            if (!map.isColliding(newRect) && !isCollidingWithTank(newRect, playerTank.getRect())) {
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
            remove_if(bullets.begin(), bullets.end(),
                [](const Bullet& b) { return !b.isActive(); }),
            bullets.end()
        );
    }

    bool isCollidingWithTank(const SDL_Rect& newRect, const SDL_Rect& otherTankRect) const {
        return (newRect.x < otherTankRect.x + otherTankRect.w &&
                newRect.x + newRect.w > otherTankRect.x &&
                newRect.y < otherTankRect.y + otherTankRect.h &&
                newRect.y + newRect.h > otherTankRect.y);
    }

    void draw(SDL_Renderer* renderer) {
        if (texture) {
            SDL_RenderCopy(renderer, texture, nullptr, &rect);
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);  // Màu cam nếu không tải được ảnh
            SDL_RenderFillRect(renderer, &rect);
        }

        for (auto& bullet : bullets) {
            bullet.draw(renderer);
        }

        drawHealthBar(renderer);
    }

    void drawHealthBar(SDL_Renderer* renderer) {
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

    SDL_Rect getRect() const { return rect; }
    vector<Bullet>& getBullets() { return bullets; }
    int getHP() const { return hp; }
    void takeDamage() { if (hp > 0) hp--; }
    bool isAlive() const { return hp > 0; }
};

#endif

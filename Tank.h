// Tank.h
#ifndef TANK_H
#define TANK_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>
#include <algorithm>
#include "Map.h"
#include "Bullet.h"

class Tank {
private:
    SDL_Rect rect;
    Direction direction;
    int speed;
    std::vector<Bullet> bullets;
    bool isMoving;
    int hp;
    SDL_Color color;
    SDL_Texture* texture;
    static const int MAX_HP = 3;
    bool isAI = false;
    Uint32 lastDirectionChange = 0;
    Uint32 lastShootTime = 0;

public:
    Tank(int x, int y, SDL_Color tankColor, SDL_Renderer* renderer, bool isPlayer1);
    ~Tank();
    void handleInput(SDL_Event& event, Map& map, bool isPlayer1, Mix_Chunk* shootSound);
    void handleAI(Map& map, const Tank& otherTank, Mix_Chunk* shootSound);
    void shoot(Map& map, Mix_Chunk* shootSound);
    void update(Map& map, const Tank& otherTank);
    bool isCollidingWithTank(const SDL_Rect& newRect, const SDL_Rect& otherTankRect) const;
    void draw(SDL_Renderer* renderer);
    void drawHealthBar(SDL_Renderer* renderer);
    SDL_Rect getRect() const;
    std::vector<Bullet>& getBullets();
    int getHP() const;              // Đã có sẵn
    void takeDamage();              // Đã có sẵn
    bool isAlive() const;           // Đã có sẵn
    void setAI(bool value);         // Đã có sẵn
    void increaseHP();              // Thêm hàm để tăng HP
    int getMaxHP() const;           // Thêm hàm để lấy MAX_HP
};

#endif

#ifndef FASTENEMYTANK_H
#define FASTENEMYTANK_H

#include "EnemyTank.h"

class FastEnemyTank : public EnemyTank {
public:
    FastEnemyTank(int x, int y, SDL_Renderer* renderer) : EnemyTank(x, y, renderer) {
        setSpeed(4);  // Tốc độ nhanh hơn
        setHP(2);     // HP thấp hơn để cân bằng
        setShootDelay(500 + (rand() % 1000)); // Bắn nhanh hơn

        // Tải ảnh khác để phân biệt
        SDL_Surface* surface = IMG_Load("fastenemytank.jpg");
        if (!surface) {
            cerr << "Failed to load fastenemytank.jpg! IMG_Error: " << IMG_GetError() << endl;
        } else {
            SDL_DestroyTexture(getTexture()); // Giải phóng texture cũ từ EnemyTank
            SDL_Texture* newTexture = SDL_CreateTextureFromSurface(renderer, surface);
            setTexture(newTexture);
            SDL_FreeSurface(surface);
            if (!newTexture) {
                cerr << "Failed to create texture from fastenemytank.jpg! SDL_Error: " << SDL_GetError() << endl;
            }
        }
    }
};

#endif

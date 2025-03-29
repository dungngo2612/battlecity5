
#ifndef BULLET_H
#define BULLET_H

#include <SDL.h>
#include "Map.h"  // Để sử dụng lớp Map trong hàm update

// Định nghĩa hướng di chuyển
enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Bullet {
private:
    SDL_Rect rect;  // Vị trí và kích thước của đạn
    Direction direction;
    int speed;  // Tốc độ của đạn (pixel mỗi khung hình)
    bool active;  // Trạng thái của đạn (đang bay hay không)

public:
    Bullet(int x, int y, Direction dir) {
        rect = { x, y, 10, 5 };  // Đạn
        direction = dir; // hướng của đạn
        speed = 5;
        active = true;
    }

    void update(Map& map) {  // Thêm tham số Map để kiểm tra va chạm
        if (!active) return;

        // Cập nhật vị trí đạn dựa trên hướng
        switch (direction) {
            case UP:
                rect.y -= speed;
                break;
            case DOWN:
                rect.y += speed;
                break;
            case LEFT:
                rect.x -= speed;
                break;
            case RIGHT:
                rect.x += speed;
                break;
        }

        // Kiểm tra va chạm với bản đồ
        bool destroyWall = false;
        if (map.handleBulletCollision(rect, destroyWall)) {
            active = false;  // Hủy đạn nếu va chạm với tường
        }

        // Nếu đạn hoàn toàn ra ngoài màn hình, hủy đạn
        if (rect.x + rect.w < 0 || rect.x > 800 || rect.y + rect.h < 0 || rect.y > 600) {
            active = false;
        }
    }

    void draw(SDL_Renderer* renderer) {
        if (!active) return;

        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Đạn màu vàng
        SDL_RenderFillRect(renderer, &rect);
    }

    bool isActive() const {
        return active;
    }
    SDL_Rect getRect() const {
        return rect;
        }
    void setActive(bool value) {
        active = value;
        }  // Thêm setter để hủy đạn khi va chạm với xe tăng
};

#endif


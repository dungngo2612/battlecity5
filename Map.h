// file map.h
#ifndef MAP_H
#define MAP_H

#include <SDL.h>
#include <SDL_image.h>
#include <algorithm>
#include <iostream>

using namespace std;


//enum TileType {
//    EMPTY = 0,
//    BRICK_WALL = 1,
//    STEEL_WALL = 2,
//    WATER = 3,
//    TREE = 4
//};

    const int EMPTY=0;
    const int BRICK_WALL = 1;
    const int STEEL_WALL = 2;
    const int WATER = 3;
    const int TREE = 4;
class Map {
private:
    static const int MAP_WIDTH = 21;      // Chiều rộng bản đồ: 20 ô
    static const int MAP_HEIGHT = 22;     // Chiều cao bản đồ: 15 ô
    static const int TILE_SIZE = 40;      // Kích thước mỗi ô: 40x40 pixel

    int mapData[MAP_HEIGHT][MAP_WIDTH];   // Mảng 2 chiều lưu dữ liệu bản đồ
    SDL_Texture* backgroundTexture;       // Texture cho hình nền background3.jpg

public:
    // Constructor: Khởi tạo bản đồ và tải hình nền
    Map(SDL_Renderer* renderer) {
        // Khởi tạo dữ liệu bản đồ
        for (int i = 0; i < MAP_HEIGHT; ++i) {
            for (int j = 0; j < MAP_WIDTH; ++j) {
                if( (i % 4 == 1 && j % 4 == 1) || (i% 4 == 3 && j %4 == 3)){
                    mapData[i][j] = BRICK_WALL;
                } else if((i%4 == 1 && j%4 == 3) || (i%4 == 3 && j%4 == 1)){
                    mapData[i][j] = STEEL_WALL;
                } else {
                    mapData[i][j] = EMPTY;
                }
            }
        }

        // Tải hình nền background3.jpg
        backgroundTexture = nullptr;
        SDL_Surface* backgroundSurface = IMG_Load("background3.jpg");
        if (!backgroundSurface) {
            cerr << "Failed to load background3.jpg! IMG_Error: " << IMG_GetError() << endl;
        } else {
            backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
            SDL_FreeSurface(backgroundSurface);
            if (!backgroundTexture) {
                cerr << "Failed to create texture from background3.jpg! SDL_Error: " << SDL_GetError() << endl;
            }
        }
    }

    // Destructor: Giải phóng texture
    ~Map() {
        if (backgroundTexture) {
            SDL_DestroyTexture(backgroundTexture);
            backgroundTexture = nullptr;
        }
    }

    // Hàm reset: Đặt lại dữ liệu bản đồ về trạng thái ban đầu
    void reset() {
        for (int i = 0; i < MAP_HEIGHT; ++i) {        // Duyệt qua từng hàng
            for (int j = 0; j < MAP_WIDTH; ++j) {     // Duyệt qua từng cột
                if (i % 4 == 1 && j % 4 == 1) {      // Nếu hàng và cột đều là số lẻ
                    mapData[i][j] = BRICK_WALL;      // Đặt ô là tường gạch
                } else if( i % 4 == 3 && j% 4 == 3){
                    mapData[i][j] = STEEL_WALL;
                } else {                              // Nếu không
                    mapData[i][j] = EMPTY;           // Đặt ô là trống
                }
            }
        }
    }

    // Hàm vẽ bản đồ: Vẽ hình nền và các ô
    void draw(SDL_Renderer* renderer) {
        if (backgroundTexture) {
            SDL_Rect dstRect = { 0, 0, MAP_WIDTH * TILE_SIZE, MAP_HEIGHT * TILE_SIZE };
            SDL_RenderCopy(renderer, backgroundTexture, nullptr, &dstRect);
        } else {
            // Nếu không có hình nền, vẽ màu mặc định để tránh màn hình trống
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_Rect fullRect = { 0, 0, MAP_WIDTH * TILE_SIZE, MAP_HEIGHT * TILE_SIZE };
            SDL_RenderFillRect(renderer, &fullRect);
        }

        for (int i = 0; i < MAP_HEIGHT; ++i) {
            for (int j = 0; j < MAP_WIDTH; ++j) {
                if (mapData[i][j] == EMPTY) {
                    continue;  // Bỏ qua ô EMPTY để hình nền hiển thị
                }
                SDL_Rect tile = { j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                switch (mapData[i][j]) {
                    case BRICK_WALL:
                        SDL_SetRenderDrawColor(renderer, 178, 34, 34, 255);
                        SDL_RenderFillRect(renderer, &tile);
                        break;
                    case STEEL_WALL:
                        SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255);
                        SDL_RenderFillRect(renderer, &tile);
                        break;
                    case WATER:
                        SDL_SetRenderDrawColor(renderer, 0, 191, 255, 255);
                        SDL_RenderFillRect(renderer, &tile);
                        break;
                    case TREE:
                        SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255);
                        SDL_RenderFillRect(renderer, &tile);
                        break;
                }
            }
        }
    }

    // Hàm xử lý va chạm đạn
    bool handleBulletCollision(SDL_Rect& bulletRect, bool& destroyWall) {
        destroyWall = false;

        int tileX = bulletRect.x / TILE_SIZE;
        int tileY = bulletRect.y / TILE_SIZE;

        if (tileX < 0 || tileX >= MAP_WIDTH || tileY < 0 || tileY >= MAP_HEIGHT) {
            return true;
        }

        if (mapData[tileY][tileX] == BRICK_WALL) {
            mapData[tileY][tileX] = BRICK_WALL;
            destroyWall = true;
            return true;
        }
        else if (mapData[tileY][tileX] == STEEL_WALL) {
                mapData[tileY][tileX] =EMPTY;
            destroyWall = true;
            return true;
        }

        return false;
    }

    // Hàm kiểm tra va chạm
    bool isColliding(const SDL_Rect& rect) const {
        int leftTile = rect.x / TILE_SIZE;
        int rightTile = (rect.x + rect.w - 1) / TILE_SIZE;
        int topTile = rect.y / TILE_SIZE;
        int bottomTile = (rect.y + rect.h - 1) / TILE_SIZE;

        leftTile = max(0, leftTile);
        rightTile = min(MAP_WIDTH - 1, rightTile);
        topTile = max(0, topTile);
        bottomTile = min(MAP_HEIGHT - 1, bottomTile);

        for (int i = topTile; i <= bottomTile; ++i) {
            for (int j = leftTile; j <= rightTile; ++j) {
                if (mapData[i][j] == BRICK_WALL || mapData[i][j] == STEEL_WALL) {
                    return true;
                }
            }
        }
        return false;
    }
};

#endif

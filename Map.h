// Map.h
#ifndef MAP_H
#define MAP_H

#include <SDL.h>
#include <algorithm>

enum TileType {
    EMPTY = 0,
    BRICK_WALL = 1,
    STEEL_WALL = 2,
    WATER = 3,
    TREE = 4
};

class Map {
private:
    static const int MAP_WIDTH = 20;
    static const int MAP_HEIGHT = 15;
    static const int TILE_SIZE = 40;

    int mapData[MAP_HEIGHT][MAP_WIDTH];

public:
    Map() {
        for (int i = 0; i < MAP_HEIGHT; ++i) {
            for (int j = 0; j < MAP_WIDTH; ++j) {
                if (i % 2 == 1 && j % 2 == 1) {
                    mapData[i][j] = BRICK_WALL;
                } else {
                    mapData[i][j] = EMPTY;
                }
            }
        }
    }

    void draw(SDL_Renderer* renderer) {
        for (int i = 0; i < MAP_HEIGHT; ++i) {
            for (int j = 0; j < MAP_WIDTH; ++j) {
                SDL_Rect tile = { j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                switch (mapData[i][j]) {
                    case EMPTY:
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                        SDL_RenderFillRect(renderer, &tile);
                        break;
                    case BRICK_WALL:
                        SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
                        SDL_RenderFillRect(renderer, &tile);
                        break;
                    case STEEL_WALL:
                        SDL_SetRenderDrawColor(renderer, 169, 169, 169, 255);
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

    bool handleBulletCollision(SDL_Rect& bulletRect, bool& destroyWall) {
        destroyWall = false;

        int tileX = bulletRect.x / TILE_SIZE;
        int tileY = bulletRect.y / TILE_SIZE;

        if (tileX < 0 || tileX >= MAP_WIDTH || tileY < 0 || tileY >= MAP_HEIGHT) {
            return true;
        }

        if (mapData[tileY][tileX] == BRICK_WALL) {
            mapData[tileY][tileX] = EMPTY;
            destroyWall = true;
            return true;
        }
        else if (mapData[tileY][tileX] == STEEL_WALL) {
            return true;
        }

        return false;
    }

    bool isColliding(const SDL_Rect& rect) const {
        int leftTile = rect.x / TILE_SIZE;
        int rightTile = (rect.x + rect.w - 1) / TILE_SIZE;
        int topTile = rect.y / TILE_SIZE;
        int bottomTile = (rect.y + rect.h - 1) / TILE_SIZE;

        leftTile = std::max(0, leftTile);
        rightTile = std::min(MAP_WIDTH - 1, rightTile);
        topTile = std::max(0, topTile);
        bottomTile = std::min(MAP_HEIGHT - 1, bottomTile);

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

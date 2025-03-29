// Menu.h
#ifndef MENU_H
#define MENU_H

#include <SDL.h>
#include <SDL_image.h>
#include<iostream>

using namespace std;
class Menu {
private:
    SDL_Renderer* renderer;
    int selectedOption;
    SDL_Texture* startButtonTexture = nullptr;
    SDL_Texture* exitButtonTexture = nullptr;
    SDL_Texture* backgroundTexture = nullptr;

public:
    Menu(SDL_Renderer* renderer) : renderer(renderer), selectedOption(0) {
        SDL_Surface* startButtonSurface = IMG_Load("start_button.png");
        if (startButtonSurface) {
            startButtonTexture = SDL_CreateTextureFromSurface(renderer, startButtonSurface);
            SDL_FreeSurface(startButtonSurface);
        } else {
            std::cerr << "Failed to load start_button.png! IMG_Error: " << IMG_GetError() << std::endl;
        }

        SDL_Surface* exitButtonSurface = IMG_Load("exit_button.jpg");
        if (exitButtonSurface) {
            exitButtonTexture = SDL_CreateTextureFromSurface(renderer, exitButtonSurface);
            SDL_FreeSurface(exitButtonSurface);
        } else {
            std::cerr << "Failed to load exit_button.jpg! IMG_Error: " << IMG_GetError() << std::endl;
        }

        SDL_Surface* backgroundSurface = IMG_Load("background.jpg");
        if (backgroundSurface) {
            backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
            SDL_FreeSurface(backgroundSurface);
        } else {
            std::cerr << "Failed to load background.jpg! IMG_Error: " << IMG_GetError() << std::endl;
        }
    }

    ~Menu() {
        if (startButtonTexture) {
            SDL_DestroyTexture(startButtonTexture);
            startButtonTexture = nullptr;
        }
        if (exitButtonTexture) {
            SDL_DestroyTexture(exitButtonTexture);
            exitButtonTexture = nullptr;
        }
        if (backgroundTexture) {
            SDL_DestroyTexture(backgroundTexture);
            backgroundTexture = nullptr;
        }
    }

    int handleInput() {
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

    void draw() {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (backgroundTexture) {
            SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
        }

        if (startButtonTexture) {
            int textW, textH;
            SDL_QueryTexture(startButtonTexture, nullptr, nullptr, &textW, &textH);
            SDL_Rect startButtonRect = { (800 - 200) / 2, 200, 200, 50 };
            SDL_SetTextureColorMod(startButtonTexture, selectedOption == 0 ? 255 : 128, 255, 255);
            SDL_RenderCopy(renderer, startButtonTexture, nullptr, &startButtonRect);
        }

        if (exitButtonTexture) {
            int textW, textH;
            SDL_QueryTexture(exitButtonTexture, nullptr, nullptr, &textW, &textH);
            SDL_Rect exitButtonRect = { (800 - 200) / 2, 300, 200, 50 };
            SDL_SetTextureColorMod(exitButtonTexture, selectedOption == 1 ? 255 : 128, 255, 255);
            SDL_RenderCopy(renderer, exitButtonTexture, nullptr, &exitButtonRect);
        }

        SDL_RenderPresent(renderer);
    }
};

#endif

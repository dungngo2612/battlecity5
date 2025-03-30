// file Menu.h
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
    SDL_Texture* startTexture = nullptr;
    SDL_Texture* exitTexture = nullptr;
    SDL_Texture* backgroundTexture = nullptr;

public:
    Menu(SDL_Renderer* renderer) : renderer(renderer), selectedOption() {
        SDL_Surface* startSurface = IMG_Load("start_button.png");
        if (startSurface) {
            startTexture = SDL_CreateTextureFromSurface(renderer, startSurface);
            SDL_FreeSurface(startSurface);
        } else {
            cerr << "Failed to load start_button.png! IMG_Error: " << IMG_GetError() << endl;
        }

        SDL_Surface* exitSurface = IMG_Load("exit_button.jpg");
        if (exitSurface) {
            exitTexture = SDL_CreateTextureFromSurface(renderer, exitSurface);
            SDL_FreeSurface(exitSurface);
        } else {
            cerr << "Failed to load exit_button.jpg! IMG_Error: " << IMG_GetError() << endl;
        }

        SDL_Surface* backgroundSurface = IMG_Load("background.jpg");
        if (backgroundSurface) {
            backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
            SDL_FreeSurface(backgroundSurface);
        } else {
            cerr << "Failed to load background.jpg! IMG_Error: " << IMG_GetError() << endl;
        }
    }

    ~Menu() {
        if (startTexture) {
            SDL_DestroyTexture(startTexture);
            startTexture = nullptr;
        }
        if (exitTexture) {
            SDL_DestroyTexture(exitTexture);
            exitTexture = nullptr;
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

        if (startTexture) {
            int textW, textH;
            SDL_QueryTexture(startTexture, nullptr, nullptr, &textW, &textH);
            SDL_Rect startButtonRect = { (800 - 200) / 2, 200, 200, 50 };
            SDL_SetTextureColorMod(startTexture, selectedOption == 0 ? 255 : 128, 255, 255);
            SDL_RenderCopy(renderer, startTexture, nullptr, &startButtonRect);
        }

        if (exitTexture) {
            int textW, textH;
            SDL_QueryTexture(exitTexture, nullptr, nullptr, &textW, &textH);
            SDL_Rect exitRect = { (800 - 200) / 2, 300, 200, 50 };
            SDL_SetTextureColorMod(exitTexture, selectedOption == 1 ? 255 : 128, 255, 255);
            SDL_RenderCopy(renderer, exitTexture, nullptr, &exitRect);
        }

        SDL_RenderPresent(renderer);
    }
};

#endif


#ifndef MENU_H
#define MENU_H

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

using namespace std;

class Menu {
private:
    SDL_Renderer* renderer;
    int selectedOption;
    SDL_Texture* startTexture = nullptr;
    SDL_Texture* exitTexture = nullptr;
    SDL_Texture* soundTexture = nullptr; // Thêm nút âm thanh
    SDL_Texture* backgroundTexture = nullptr;
    bool isSoundOn = true; // Trạng thái âm thanh, mặc định là bật

public:
    Menu(SDL_Renderer* renderer) : renderer(renderer), selectedOption(0) {
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

        SDL_Surface* soundSurface = IMG_Load("sound_button.jpg");
        if (soundSurface) {
            soundTexture = SDL_CreateTextureFromSurface(renderer, soundSurface);
            SDL_FreeSurface(soundSurface);
        } else {
            cerr << "Failed to load sound_button.jpg! IMG_Error: " << IMG_GetError() << endl;
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
        if (soundTexture) {
            SDL_DestroyTexture(soundTexture);
            soundTexture = nullptr;
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
                return 1; // Thoát game
            }
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        selectedOption = (selectedOption - 1 + 3) % 3; // 3 tùy chọn: Start, Exit, Sound
                        break;
                    case SDLK_DOWN:
                        selectedOption = (selectedOption + 1) % 3;
                        break;
                    case SDLK_RETURN:
                        if (selectedOption == 2) { // Nút Sound
                            isSoundOn = !isSoundOn; // Chuyển đổi trạng thái âm thanh
                            if (isSoundOn) {
                                Mix_VolumeMusic(MIX_MAX_VOLUME); // Bật nhạc
                            } else {
                                Mix_VolumeMusic(0); // Tắt nhạc
                            }
                            return -1; // Không thoát menu, chỉ cập nhật trạng thái
                        }
                        return selectedOption; // Trả về 0 (Start) hoặc 1 (Exit)
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

        if (soundTexture) {
            int textW, textH;
            SDL_QueryTexture(soundTexture, nullptr, nullptr, &textW, &textH);
            SDL_Rect soundRect = { (800 - 200) / 2, 400, 200, 50 }; // Đặt nút Sound bên dưới Exit
            SDL_SetTextureColorMod(soundTexture, selectedOption == 2 ? 255 : 128, 255, 255);
            if (!isSoundOn) {
                SDL_SetTextureAlphaMod(soundTexture, 128); // Làm mờ nút khi âm thanh tắt
            } else {
                SDL_SetTextureAlphaMod(soundTexture, 255);
            }
            SDL_RenderCopy(renderer, soundTexture, nullptr, &soundRect);
        }

        SDL_RenderPresent(renderer);
    }

    bool getSoundState() const { return isSoundOn; } // Getter để Game lấy trạng thái âm thanh
};

#endif

#include "pictureshow.h"

#include <SDL2/SDL.h>
#include <iostream>
#include <string>




/**
 * @brief 显示图片
 *
 * 使用SDL库在窗口中显示指定的图片。
 *
 * @param imagePath 图片路径
 *
 * @return 如果成功显示图片，则返回true；否则返回false
 */
bool showImage(const std::string& imagePath) 
{
    // 初始化SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // 创建窗口
    SDL_Window* window = SDL_CreateWindow("Image Display", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    // 创建渲染器
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    // 加载图片
    SDL_Surface* loadedSurface = SDL_LoadBMP(imagePath.c_str());
    if (!loadedSurface) {
        std::cerr << "Unable to load image " << imagePath << "! SDL Error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    // 转换图片格式
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    SDL_FreeSurface(loadedSurface);

    if (!texture) {
        std::cerr << "Unable to create texture from " << imagePath << "! SDL Error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    // 清空渲染目标
    SDL_RenderClear(renderer);

    // 绘制图片
    SDL_RenderCopy(renderer, texture, NULL, NULL);

    // 显示渲染结果
    SDL_RenderPresent(renderer);

    // 等待用户关闭窗口
    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
    }

    // 清理资源
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return true;
}
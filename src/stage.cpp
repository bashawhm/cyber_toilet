#include "stage.h"

#include <iostream>
#include <assert.h>
#include <cmath>
#include <stdlib.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL2_framerate.h"
#include "SDL2/SDL_image.h"

using namespace std;

Stage::Stage() {
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow("Cyber Toilet", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINWIDTH, WINHEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    SDL_initFramerate(&fpsManager);
    SDL_setFramerate(&fpsManager, 60);
    running = true;
    initTextures();
    Tile **texMap = loadGame();
    initTiles(texMap);
    SDL_Rect rect;
    rect.w = (int)round((float)WINWIDTH / (float)TILES_PER_SCREEN_X);
    rect.h = (int)round((float)WINHEIGHT / (float)TILES_PER_SCREEN_Y);
    player.x = 128;
    player.y = 128;
    rect.x = (TILES_PER_SCREEN_X / 2) * rect.w;
    rect.y = (TILES_PER_SCREEN_Y / 2) * rect.h;
    player.tile.rect = rect;
    player.tile.texIdx = Toilet;
}

void Stage::initTextures() {
    SDL_Surface *sur = IMG_Load("assets/space.png");
    assert(sur != NULL);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, sur);
    textures.push_back(tex);
    SDL_FreeSurface(sur);
    sur = IMG_Load("assets/tile.png");
    assert(sur != NULL);
    tex = SDL_CreateTextureFromSurface(renderer, sur);
    textures.push_back(tex);
    SDL_FreeSurface(sur);
    sur = IMG_Load("assets/toilet.png");
    assert(sur != NULL);
    tex = SDL_CreateTextureFromSurface(renderer, sur);
    textures.push_back(tex);
    SDL_FreeSurface(sur);
    sur = IMG_Load("assets/wall.png");
    assert(sur != NULL);
    tex = SDL_CreateTextureFromSurface(renderer, sur);
    textures.push_back(tex);
    SDL_FreeSurface(sur);
}

void Stage::initTiles(Tile **texMap) {
    assert(texMap != NULL);
    for (int i = 0; i < TILE_NUM_X; ++i) {
        for (int j = 0; j < TILE_NUM_Y; ++j) {
            tiles[i][j] = texMap[i][j];
        }
    }
}

Tile **Stage::loadGame() {
    Tile **texMap = NULL;
    texMap = (Tile**)malloc(TILE_NUM_X*sizeof(Tile*));
    for (int i = 0; i < TILE_NUM_X; ++i) {
        texMap[i] = (Tile*)malloc(TILE_NUM_Y*sizeof(Tile));
    }
    if (GENMAP) {
        for (int i = 0; i < TILE_NUM_X; ++i) {
            for (int j = 0; j < TILE_NUM_Y; ++j) {
                SDL_Rect rect;
                rect.x = i * (int)round((float)WINWIDTH / (float)TILES_PER_SCREEN_X);
                rect.y = j * (int)round((float)WINHEIGHT / (float)TILES_PER_SCREEN_Y);
                rect.w = (int)round((float)WINWIDTH / (float)TILES_PER_SCREEN_X);
                rect.h = (int)round((float)WINHEIGHT / (float)TILES_PER_SCREEN_Y);
                texMap[i][j].rect = rect;
                if (rand()%3 == 0) {
                    texMap[i][j].texIdx = Space;
                } else {
                    texMap[i][j].texIdx = Station;
                }
            }
        }
    }
    return texMap;
}

void Stage::render() {
    SDL_RenderClear(renderer);
    int setX = 0;
    for (int i = (player.x - TILES_PER_SCREEN_X / 2); i <= (player.x + TILES_PER_SCREEN_X / 2); ++i, ++setX) {
        int setY = 0;
        for (int j = (player.y - TILES_PER_SCREEN_Y / 2); j <= (player.y + TILES_PER_SCREEN_Y / 2); ++j, ++setY) {
            //start the view screen at 0, 0
            if (i >= 0 && j >= 0) {
                SDL_Rect tmpRect;
                tmpRect.w = tiles[i][j].rect.w;
                tmpRect.h = tiles[i][j].rect.h;
                tmpRect.x = setX * tmpRect.w;
                tmpRect.y = setY * tmpRect.h;

                SDL_RenderCopy(renderer, textures[tiles[i][j].texIdx], NULL, &tmpRect); 
            }
        }
    }
    //Render player
    SDL_RenderCopy(renderer, textures[player.tile.texIdx], NULL, &player.tile.rect);
    SDL_RenderPresent(renderer);
}

EventCode Stage::getEvent() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYDOWN: {
            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE: {
                return Exit;
            }
            case SDLK_w: {
                player.y--;
                return Move;
            }
            case SDLK_a: {
                player.x--;
                return Move;
            }
            case SDLK_s: {
                player.y++;
                return Move;
            }
            case SDLK_d: {
                player.x++;
                return Move;
            }
            }
            break;
        }
        case SDL_QUIT: {
            return Exit;
        }
        }
    }
    return None;
}
#include "stage.h"

#include <iostream>
#include <assert.h>
#include <cmath>
#include <stdlib.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL2_framerate.h"
#include "SDL2/SDL_image.h"

using namespace std;

static inline Entity initPlayer() {
    Entity player;
    SDL_Rect rect;
    rect.w = (int)round((float)WINWIDTH / (float)TILES_PER_SCREEN_X);
    rect.h = (int)round((float)WINHEIGHT / (float)TILES_PER_SCREEN_Y);
    player.x = PLAYER_START_X;
    player.y = PLAYER_START_Y;
    rect.x = (TILES_PER_SCREEN_X / 2) * rect.w;
    rect.y = (TILES_PER_SCREEN_Y / 2) * rect.h;
    player.tile.rect = rect;
    player.tile.texIdx = Toilet;
    player.health = 100;
    player.strength = 25;
    return player;
}

static inline Entity *initTank(SDL_Rect r, int x, int y) {
    Entity *tank = new Entity;
    SDL_Rect rect = r;
    tank -> x = x;
    tank -> y = y;
    rect.x = tank -> x * rect.w;
    rect.y = tank -> y * rect.h;
    tank -> tile.rect = rect;
    tank -> tile.texIdx = Tank;
    tank -> health = 50;
    tank -> strength = 10;
    return tank;
}

void Stage::placeEscapePod() {
    int i = rand() % TILE_NUM_X;
    int j = rand() % TILE_NUM_Y;
    do { 
        i = rand() % TILE_NUM_X;
        j = rand() % TILE_NUM_Y;
    } while (tiles[i][j].texIdx != Station);
    tiles[i][j].texIdx = Escape;
    // cerr << i << " " << j << endl;
}

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
    player = initPlayer();
    placeEscapePod();
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
    sur = IMG_Load("assets/tank.png");
    assert(sur != NULL);
    tex = SDL_CreateTextureFromSurface(renderer, sur);
    textures.push_back(tex);
    SDL_FreeSurface(sur);
    sur = IMG_Load("assets/escape.png");
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
                    if (rand()%NUM_TANKS == 0) {
                        entities.push_back(*initTank(texMap[i][j].rect, i, j));
                    }
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
            if (i >= 0 && j >= 0 && i < TILE_NUM_X && j < TILE_NUM_Y) {
                SDL_Rect tmpRect;
                tmpRect.w = tiles[i][j].rect.w;
                tmpRect.h = tiles[i][j].rect.h;
                tmpRect.x = setX * tmpRect.w;
                tmpRect.y = setY * tmpRect.h;

                SDL_RenderCopy(renderer, textures[tiles[i][j].texIdx], NULL, &tmpRect); 
                for (auto itr = entities.begin(); itr != entities.end(); ++itr) {
                    if (itr -> x == i && itr -> y == j) {
                        if (itr -> health > 0) {
                            SDL_RenderCopy(renderer, textures[itr -> tile.texIdx], NULL, &tmpRect);
                        }
                    }
                }
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
                if (player.y-1 >= 0 && (tiles[player.x][player.y-1].texIdx == Station || tiles[player.x][player.y-1].texIdx == Escape)) {
                    player.y--;
                }
                return Move;
            }
            case SDLK_a: {
                if (player.x-1 >= 0 && (tiles[player.x-1][player.y].texIdx == Station || tiles[player.x-1][player.y].texIdx == Escape)) {
                    player.x--;
                }
                return Move;
            }
            case SDLK_s: {
                if (player.y+1 < TILE_NUM_Y && (tiles[player.x][player.y+1].texIdx == Station || tiles[player.x][player.y+1].texIdx == Escape)) {
                    player.y++;
                }
                return Move;
            }
            case SDLK_d: {
                if (player.x+1 < TILE_NUM_X && (tiles[player.x+1][player.y].texIdx == Station || tiles[player.x+1][player.y].texIdx == Escape)) {
                    player.x++;
                }
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

bool Stage::atEscape() {
    if (tiles[player.x][player.y].texIdx == Escape) {
        return true;
    }
    return false;
}

bool Stage::lineOfSight(const Entity &ent) {
    if (ent.x == player.x && ent.y == player.x) {
        return true;
    }
    if (abs(ent.x - player.x) <= FIRE_SIGHT_RANGE && abs(ent.y - player.y) <= FIRE_SIGHT_RANGE) {
        //In sight range
        //If wall not in way in x
        bool goodX = false;
        //Good in +x direction
        for (int i = 0; i <= FIRE_SIGHT_RANGE; ++i) {
            if (ent.x+i == player.x && ent.y == player.y) {
                goodX = true;
                break;
            }
            if (tiles[ent.x+i][ent.y].texIdx != Station) {
                goodX = false;
                break;
            }
        }
        if (goodX) {
            return true;
        }

        goodX = false;
        //Good in -x direction
        for (int i = 0; i <= FIRE_SIGHT_RANGE; ++i) {
            if (ent.x-i == player.x && ent.y == player.y) {
                goodX = true;
                break;
            }
            if (tiles[ent.x-i][ent.y].texIdx != Station) {
                goodX = false;
                break;
            }
        }
        if (goodX) {
            return true;
        }

        bool goodY = false;
        //good in +y dir
        for (int i = 0; i <= FIRE_SIGHT_RANGE; ++i) {
            if (ent.x == player.x && ent.y+i == player.y) {
                goodY = true;
                break;
            }
            if (tiles[ent.x][ent.y+i].texIdx != Station) {
                goodY = false;
                break;
            }
        }
        if (goodY) {
            return true;
        }

        goodY = false;
        //Good in the -y direction
        for (int i = 0; i <= FIRE_SIGHT_RANGE; ++i) {
            if (ent.x == player.x && ent.y-i == player.y) {
                goodY = true;
                break;
            }
            if (tiles[ent.x][ent.y-i].texIdx != Station) {
                goodY = false;
                break;
            }
        }
        if (goodY) {
            return true;
        }
    }
    return false;
}

void Stage::simEntities() {
    for (auto itr = entities.begin(); itr != entities.end(); ++itr) {
        if (itr -> health > 0) {
            //Move towards player if possible
            if (abs(itr -> x - player.x) <= PATH_SIGHT_RANGE && abs(itr -> y - player.y) <= PATH_SIGHT_RANGE) {
                float dx = ((float)itr->x - (float)player.x);
                float dy = (float)itr->y - (float)player.y;
                if (abs(dy) - abs(dx) >= 0) {
                    //If there is a bigger vertical diff
                    if (dy >= 0) {
                        if (tiles[itr -> x][itr -> y-1].texIdx == Station) {
                            //Go up
                            itr -> y--;
                        }
                    } else {
                        if (tiles[itr -> x][itr -> y+1].texIdx == Station) {
                            //Go down
                            itr -> y++;
                        }
                    }
                } else {
                    if (dx >= 0) {
                        if (tiles[itr -> x-1][itr -> y].texIdx == Station) {
                            //Go right
                            itr -> x--;
                        }
                    } else {
                        if (tiles[itr -> x+1][itr -> y].texIdx == Station) {
                            //Go left
                            itr -> x++;
                        }
                    }
                }
                //Attack player if possible
                if (lineOfSight(*itr)) {
                    // cerr << "player.x: " << player.x << " player.y: " << player.y << " ent.x: " << itr->x << " ent.y: " << itr->y << endl;
                    itr -> health -= player.strength;
                    player.health -= itr -> strength;
                    cerr << player.health << endl;
                }
            }
        }
    }
}


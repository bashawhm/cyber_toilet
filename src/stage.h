#include <vector>

#include "SDL2/SDL.h"
#include "SDL2/SDL2_framerate.h"
#include "common.h"

using namespace std;

enum TileTex {
    Space = 0,
    Station,
    Toilet,
    Wall,
};

enum EventCode {
    None = 0,
    Exit,
    Move,
};

struct Tile {
    SDL_Rect rect;
    TileTex texIdx;
};

struct Entity {
    int x;
    int y;
    Tile tile;
};

class Stage {
public:
    SDL_Window *window;
    SDL_Renderer *renderer;

    FPSmanager fpsManager;
    bool running;

    Tile tiles[TILE_NUM_X][TILE_NUM_Y];
    vector<SDL_Texture*> textures;

    Entity player;

    Stage();
    void initTextures();
    void initTiles(Tile **texMap);
    Tile **loadGame();
    void render();
    EventCode getEvent();
};
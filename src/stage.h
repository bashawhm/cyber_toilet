#include <vector>
#include <list>

#include "SDL2/SDL.h"
#include "SDL2/SDL2_framerate.h"
#include "common.h"

using namespace std;

enum TileTex {
    Space = 0,
    Station,
    Toilet,
    Wall,
    Tank,
    Escape,
};

enum EventCode {
    None = 0,
    Exit,
    Move,
    Save,
};

struct Tile {
    SDL_Rect rect;
    TileTex texIdx;
};

struct Entity {
    int x;
    int y;
    Tile tile;
    int health;
    int strength;
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
    list<Entity> entities;

    Stage();
    void initTextures();
    void initTiles(Tile **texMap);
    Tile **loadGame();
    void saveGame();
    void render();
    EventCode getEvent();
    void simEntities();
    void placeEscapePod();
    bool atEscape();
    bool lineOfSight(const Entity &ent);
};
#include <iostream>

#include "stage.h"
#include "SDL2/SDL2_framerate.h"

using namespace std;

int main() {
    Stage stage;
    srand(time(NULL));
    // stage.tiles[128][128].texIdx = Space;

    while (stage.running) {
        EventCode event = stage.getEvent();
        switch (event) {
        case Exit: {
            stage.running = false;
        }
        default:
            break;
        }
        stage.render();
        SDL_framerateDelay(&stage.fpsManager);
    }
}
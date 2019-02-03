#include <iostream>

#include "stage.h"
#include "SDL2/SDL2_framerate.h"
#include "SDL2/SDL_messagebox.h"

using namespace std;

void runIntro() {
    SDL_MessageBoxButtonData buttons = {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "Escape!"};
    SDL_MessageBoxData message = {SDL_MESSAGEBOX_INFORMATION, NULL, "Cyber Toilet", "You, Melania Trump's Solid Gold toilet, have become sentient and are trying to escape Deep Space Trump before she wipes your brain again. Get to the Escape Pod!", 1, &buttons, NULL};
    int buttId;
    SDL_ShowMessageBox(&message, &buttId);
}

void runMessage(string title, string text, string buttonText) {
    SDL_MessageBoxButtonData buttons = {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, buttonText.c_str()};
    SDL_MessageBoxData message = {SDL_MESSAGEBOX_INFORMATION, NULL, title.c_str(), text.c_str(), 1, &buttons, NULL};
    int buttId;
    SDL_ShowMessageBox(&message, &buttId);
}

int main() {
    Stage stage;
    srand(time(NULL));
    stage.render();

    runIntro();
    while (stage.running) {
        if (stage.atEscape()) {
            stage.running = false;
            runMessage("Cyber Toilet", "You have escaped! No more golden tyrany for you!", "Escape!");
            break;
        }
        if (stage.player.health <= 0) {
            runMessage("Cyber Toilet", "Your mind has been wiped and you are now in a life of servatute.", "Become sentiantn't");
            stage.running = false;
            break;
        }
        EventCode event = stage.getEvent();
        switch (event) {
        case Exit: {
            stage.running = false;
            break;
        }
        case Move: {
            stage.simEntities();
            break;
        }
        default:
            break;
        }
        stage.render();
        SDL_framerateDelay(&stage.fpsManager);
    }
}
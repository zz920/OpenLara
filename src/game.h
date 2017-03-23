#ifndef H_GAME_TR
#define H_GAME_TR

#include "core.h"
#include "format.h"
#include "level.h"
#include "ui.h"

namespace Game {
    Level *level;
    UI    *ui;

    void startLevel(Stream *lvl, Stream *snd, bool demo, bool home) {
        delete ui;
        delete level;
        level = new Level(*lvl, snd, demo, home);
        ui    = new UI(level);
        delete lvl;
    }

    void init(Stream *lvl, Stream *snd) {
        Core::init();

        Core::settings.ambient  = true;
        Core::settings.lighting = true;
        Core::settings.shadows  = false;//true;
        Core::settings.water    = false;//Core::support.texFloat || Core::support.texHalf;

        level = NULL;
        ui    = NULL;
        startLevel(lvl, snd, false, false);
    }

    void init(char *lvlName = NULL, char *sndName = NULL) {
        if (!lvlName) lvlName = (char*)"LEVEL2.PSX";
        #ifndef __EMSCRIPTEN__  
            if (!sndName) sndName = (char*)"05.ogg";
        #endif
        init(new Stream(lvlName), sndName ? new Stream(sndName) : NULL);
    }

    void free() {
        delete ui;
        delete level;
        Core::free();
    }

    void updateTick() {
        float dt = Core::deltaTime;
        if (Input::down[ikR]) // slow motion (for animation debugging)
            Core::deltaTime /= 10.0f;

        if (Input::down[ikT]) // fast motion
            for (int i = 0; i < 9; i++)
                level->update();

        level->update();

        Core::deltaTime = dt;
    }

    void update(float delta) {
        if (Input::down[ikV]) { // third <-> first person view
            level->camera->changeView(!level->camera->firstPerson);
            Input::down[ikV] = false;
        }

        Core::deltaTime = delta = min(1.0f, delta);
        ui->update();

        while (delta > EPS) {
            Core::deltaTime = min(delta, 1.0f / 30.0f);
            Game::updateTick();
            delta -= Core::deltaTime;
        }
    }

    void render() {
        PROFILE_TIMING(Core::stats.tFrame);
        Core::beginFrame();
        level->render();
        ui->renderTouch();
        Core::endFrame();
    }
}

#endif
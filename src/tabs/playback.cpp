#include "playback.hpp"

#include <string.h>
#include <ncurses.h>
#include <inttypes.h>

#include <vector>
#include <algorithm>

#include "../pa.hpp"

Playback::Playback()
{
    selected_index = pa.exists(pa.PA_INPUTS, -1);
}

Playback::~Playback()
{

}

void Playback::handleInput(int input)
{
    selected_index = pa.exists(pa.PA_INPUTS, selected_index);

    if (selected_index == -1) {
        return;
    }

    auto pai = pa.PA_INPUTS.find(selected_index);

    PaObject *selected_pobj = nullptr;
    if (pai != pa.PA_INPUTS.end()) {
        selected_pobj = pai->second;
    }

    switch (input) {
        case 'm':
            if(selected_pobj != nullptr){
                selected_pobj->toggle_mute();    
            }

            break;

        case 'g': {
            auto i = pa.PA_INPUTS.begin();

            if (i != pa.PA_INPUTS.end()) {
                selected_index = i->first;
            }

            break;
        }

        case 'G': {
            auto i = pa.PA_INPUTS.rbegin();

            if (i != pa.PA_INPUTS.rend()) {
                selected_index = i->first;
            }

            break;
        }

        case 'c': {
            auto i = pa.PA_INPUTS.find(selected_index);

            if (i != pa.PA_INPUTS.end()) {
                dropDown(pa.PA_SINKS, 0);
            }

            break;
        }

        case 'k': {
            auto i = std::prev(pa.PA_INPUTS.find(selected_index), 1);

            if (i != pa.PA_INPUTS.end()) {
                selected_index = i->first;
            }

            break;
        }

        case 'j': {
            auto i = std::next(pa.PA_INPUTS.find(selected_index), 1);

            if (i != pa.PA_INPUTS.end()) {
                selected_index = i->first;
            }

            break;
        }

        case 'h':{
            if(selected_pobj != nullptr){
                selected_pobj->step_volume(-1);    
            }
         }
            break;

        case 'l':
            if(selected_pobj != nullptr){
                selected_pobj->step_volume(1);
            }
            break;

        case '\t': {
            if (selected_pobj != nullptr) {
                auto current_sink = pa.PA_SINKS.find(selected_pobj->getSink());
                current_sink = std::next(current_sink, 1);

                if (current_sink == pa.PA_SINKS.end()) {
                    current_sink = pa.PA_SINKS.begin();
                }

                selected_pobj->move(current_sink->first);
            }

            break;
        }
    }
}

void Playback::draw(int w, int h)
{
    int baseY = 3;

    for (auto &i : pa.PA_INPUTS) {
        float perc = static_cast<float>(i.second->volume) /
                     (PA_VOLUME_NORM * 1.5f);

        volumeBar(w, h, 0, baseY, perc, i.second->peak);

        if (i.first == selected_index) {
            attron(COLOR_PAIR(1));
        }

        char label[255] = {0};
        char app[255] = {0};

        if (strlen(i.second->getAppName()) > 0) {
            snprintf(
                app,
                sizeof(app),
                "%s : %s",
                i.second->getAppName(),
                i.second->name
            );
        } else {
            snprintf(app, sizeof(app), "%s", i.second->name);
        }

        if (i.second->mute) {
            snprintf(
                label,
                sizeof(label),
                "%s (muted)",
                app
            );
        } else {
            snprintf(
                label,
                sizeof(label),
                "%s (%d%%)",
                app,
                static_cast<int>(perc * 1.5f * 100.f)
            );
        }

        mvaddstr(baseY - 2, 1, label);
        char *name = pa.PA_SINKS.find(i.second->getSink())->second->name;

        unsigned int len = strlen(name);
        unsigned int sink_pos = w - 1 - len;

        mvaddstr(
            baseY - 2,
            sink_pos,
            name
        );

        if (i.first == selected_index) {
            attroff(COLOR_PAIR(1));
        }

        baseY += 5;
    }
}

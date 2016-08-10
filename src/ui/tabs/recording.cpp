#include "recording.hpp"

#include <string.h>
#include <ncurses.h>
#include <inttypes.h>

#include "pa.hpp"

Recording::Recording()
{
    selected_index = pa.exists(pa.PA_SOURCE_OUTPUTS, -1);
}

Recording::~Recording()
{

}

void Recording::handleInput(int input)
{
    selected_index = pa.exists(pa.PA_SOURCE_OUTPUTS, selected_index);

    if (selected_index == -1) {
        return;
    }

    auto pai = pa.PA_SOURCE_OUTPUTS.find(selected_index);

    PaObject *selected_pobj = nullptr;

    if (pai != pa.PA_INPUTS.end()) {
        selected_pobj = pai->second;
    }

    switch (input) {
        case 'm':
            if (selected_pobj != nullptr) {
                selected_pobj->toggle_mute();
            }

            break;

        case 'g': {
            auto i = pa.PA_SOURCE_OUTPUTS.begin();

            if (i != pa.PA_SOURCE_OUTPUTS.end()) {
                selected_index = i->first;
            }

            break;
        }

        case 'G': {
            auto i = pa.PA_SOURCE_OUTPUTS.rbegin();

            if (i != pa.PA_SOURCE_OUTPUTS.rend()) {
                selected_index = i->first;
            }

            break;
        }

        case 'k': {
            auto i = std::prev(pa.PA_SOURCE_OUTPUTS.find(selected_index), 1);

            if (i != pa.PA_SOURCE_OUTPUTS.end()) {
                selected_index = i->first;
            }

            break;
        }

        case 'j': {
            auto i = std::next(pa.PA_SOURCE_OUTPUTS.find(selected_index), 1);

            if (i != pa.PA_SOURCE_OUTPUTS.end()) {
                selected_index = i->first;
            }

            break;
        }

        case 'h':
            if (selected_pobj != nullptr) {
                selected_pobj->step_volume(-1);
            }

            break;

        case 'l':
            if (selected_pobj != nullptr) {
                selected_pobj->step_volume(1);
            }

            break;

        case '\t': {
            if (selected_pobj != nullptr) {
                auto current_source = pa.PA_SOURCES.find(selected_pobj->getSource());
                current_source = std::next(current_source, 1);

                if (current_source == pa.PA_SOURCES.end()) {
                    current_source = pa.PA_SOURCES.begin();
                }

                selected_pobj->move(current_source->first);
            }
        }
    }

}

void Recording::draw(int w, int h)
{
    int baseY = 3;

    for (auto &i : pa.PA_SOURCE_OUTPUTS) {
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
        char *name = pa.PA_SOURCES.find(i.second->getSource())->second->name;

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

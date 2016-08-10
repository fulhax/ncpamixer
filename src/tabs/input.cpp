#include "input.hpp"

#include <string.h>
#include <ncurses.h>
#include <inttypes.h>

#include "../pa.hpp"

Input::Input()
{
}

Input::~Input()
{

}

void Input::handleInput(int input)
{
    selected_source_index = pa.sink_exists(selected_source_index);
    if (selected_source_index == -1) {
        return;
    }

    switch (input) {
        case 'm':
            pa.toggle_sink_mute(selected_source_index);
            break;

        case 'g': {
            auto i = pa.PA_SOURCES.begin();

            if (i != pa.PA_SOURCES.end()) {
                selected_source_index = i->first;
            }

            break;
        }

        case 'G': {
            auto i = pa.PA_SOURCES.rbegin();

            if (i != pa.PA_SOURCES.rend()) {
                selected_source_index = i->first;
            }

            break;
        }

        case 'k': {
            auto i = std::prev(pa.PA_SOURCES.find(selected_source_index), 1);

            if (i != pa.PA_SOURCES.end()) {
                selected_source_index = i->first;
            }

            break;
        }

        case 'j': {
            auto i = std::next(pa.PA_SOURCES.find(selected_source_index), 1);

            if (i != pa.PA_SOURCES.end()) {
                selected_source_index = i->first;
            }

            break;
        }

        case 'h':
            pa.set_sink_volume(selected_source_index, -1);

            break;

        case 'l':
            pa.set_sink_volume(selected_source_index, 1);

            break;
    }


}

void Input::draw(int w, int h)
{
    int baseY = 3;

    for (auto &i : pa.PA_SOURCES) {
        float perc = static_cast<float>(i.second.volume) /
                     (PA_VOLUME_NORM * 1.5f);

        volumeBar(w, h, 0, baseY, perc, i.second.peak);

        if (i.first == selected_source_index) {
            attron(COLOR_PAIR(1));
        }

        char label[255];

        if (i.second.mute) {
            snprintf(
                label,
                sizeof(label),
                "%s (muted)",
                i.second.name
            );
        } else {
            snprintf(
                label,
                sizeof(label),
                "%s (%d%%)",
                i.second.name,
                static_cast<int>(perc * 1.5f * 100.f)
            );
        }

        mvaddstr(baseY - 2, 1, label);

        if (i.first == selected_source_index) {
            attroff(COLOR_PAIR(1));
        }

        baseY += 5;
    }

}

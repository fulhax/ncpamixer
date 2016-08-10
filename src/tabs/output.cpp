#include "output.hpp"

#include <string.h>
#include <ncurses.h>
#include <inttypes.h>

#include "../pa.hpp"

Output::Output()
{
    selected_index = pa.exists(pa.PA_SINKS, -1);
}

Output::~Output()
{

}

void Output::handleInput(int input)
{
    selected_index = pa.exists(pa.PA_SINKS, selected_index);

    if (selected_index == -1) {
        return;
    }

    switch (input) {
        case 'm':
            pa.toggle_sink_mute(selected_index);
            break;

        case 'g': {
            auto i = pa.PA_SINKS.begin();

            if (i != pa.PA_SINKS.end()) {
                selected_index = i->first;
            }

            break;
        }

        case 'G': {
            auto i = pa.PA_SINKS.rbegin();

            if (i != pa.PA_SINKS.rend()) {
                selected_index = i->first;
            }

            break;
        }

        case 'k': {
            auto i = std::prev(pa.PA_SINKS.find(selected_index), 1);

            if (i != pa.PA_SINKS.end()) {
                selected_index = i->first;
            }

            break;
        }

        case 'j': {
            auto i = std::next(pa.PA_SINKS.find(selected_index), 1);

            if (i != pa.PA_SINKS.end()) {
                selected_index = i->first;
            }

            break;
        }

        case 'h':
            pa.set_sink_volume(selected_index, -1);

            break;

        case 'l':
            pa.set_sink_volume(selected_index, 1);

            break;
    }

}

void Output::draw(int w, int h)
{
    int baseY = 3;

    for (auto &i : pa.PA_SINKS) {
        float perc = static_cast<float>(i.second->volume) /
                     (PA_VOLUME_NORM * 1.5f);

        volumeBar(w, h, 0, baseY, perc, i.second->peak);

        if (i.first == selected_index) {
            attron(COLOR_PAIR(1));
        }

        char label[255];

        if (i.second->mute) {
            snprintf(
                label,
                sizeof(label),
                "%s (muted)",
                i.second->name
            );
        } else {
            snprintf(
                label,
                sizeof(label),
                "%s (%d%%)",
                i.second->name,
                static_cast<int>(perc * 1.5f * 100.f)
            );
        }

        mvaddstr(baseY - 2, 1, label);

        if (i.first == selected_index) {
            attroff(COLOR_PAIR(1));
        }

        baseY += 5;
    }
}

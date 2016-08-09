#include "playback.hpp"

#include <string.h>
#include <ncursesw/ncurses.h>

#include "../pa.hpp"

Playback::Playback()
{
    selected_input_index = 0;
}

Playback::~Playback()
{

}

void Playback::handleInput(unsigned int input)
{
    if (!pa.sink_input_exists(selected_input_index)) {
        return;
    }

    switch (input) {
        case 'm':
            pa.toggle_input_mute(selected_input_index);
            break;

        case 'g': {
            auto i = pa.PA_INPUTS.begin();

            if (i != pa.PA_INPUTS.end()) {
                selected_input_index = i->first;
            }

            break;
        }

        case 'G': {
            auto i = pa.PA_INPUTS.rbegin();

            if (i != pa.PA_INPUTS.rend()) {
                selected_input_index = i->first;
            }

            break;
        }

        case 'k': {
            auto i = std::prev(pa.PA_INPUTS.find(selected_input_index), 1);

            if (i != pa.PA_INPUTS.end()) {
                selected_input_index = i->first;
            }

            break;
        }

        case 'j': {
            auto i = std::next(pa.PA_INPUTS.find(selected_input_index), 1);

            if (i != pa.PA_INPUTS.end()) {
                selected_input_index = i->first;
            }

            break;
        }

        case 'h':
            pa.set_input_volume(selected_input_index, -1);

            break;

        case 'l':
            pa.set_input_volume(selected_input_index, 1);

            break;

        case '\t': {
            auto i = pa.PA_INPUTS.find(selected_input_index);

            if (i != pa.PA_INPUTS.end()) {
                auto current_sink = pa.PA_SINKS.find(i->second.sink);

                current_sink = std::next(current_sink, 1);

                if (current_sink == pa.PA_SINKS.end()) {
                    current_sink = pa.PA_SINKS.begin();
                }

                pa.move_input_sink(selected_input_index, current_sink->first);
            }
            break;
        }
    }
}

void Playback::draw(int w, int h)
{
    int baseY = 3;

    for (auto &i : pa.PA_INPUTS) {
        double perc = static_cast<double>(i.second.volume) /
                      (PA_VOLUME_NORM * 2);

        volumeBar(w, h, 0, baseY, perc, perc);

        if (i.first == selected_input_index) {
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
                static_cast<int>(perc * 100)
            );
        }

        mvaddstr(baseY - 2, 1, label);
        char *name = pa.PA_SINKS.find(i.second.sink)->second.name;

        unsigned int len = strlen(name);
        unsigned int sink_pos = w - 1 - len;

        mvaddstr(
            baseY - 2,
            sink_pos,
            name
        );

        if (i.first == selected_input_index) {
            attroff(COLOR_PAIR(1));
        }

        baseY += 5;
    }
}

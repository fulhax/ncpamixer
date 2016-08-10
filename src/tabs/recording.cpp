#include "recording.hpp"

#include <string.h>
#include <ncurses.h>
#include <inttypes.h>

#include "../pa.hpp"

Recording::Recording()
{
    selected_output_index = pa.sink_source_output_exists(-1);
}

Recording::~Recording()
{

}

void Recording::handleInput(int input)
{
    selected_output_index = pa.sink_source_output_exists(selected_output_index);

    if(selected_output_index == -1) {
        return;
    }

    switch (input) {
        case 'm':
            pa.toggle_input_mute(selected_output_index);
            break;

        case 'g': {
            auto i = pa.PA_SOURCE_OUTPUTS.begin();

            if (i != pa.PA_SOURCE_OUTPUTS.end()) {
                selected_output_index = i->first;
            }

            break;
        }

        case 'G': {
            auto i = pa.PA_SOURCE_OUTPUTS.rbegin();

            if (i != pa.PA_SOURCE_OUTPUTS.rend()) {
                selected_output_index = i->first;
            }

            break;
        }

        case 'k': {
            auto i = std::prev(pa.PA_SOURCE_OUTPUTS.find(selected_output_index), 1);

            if (i != pa.PA_SOURCE_OUTPUTS.end()) {
                selected_output_index = i->first;
            }

            break;
        }

        case 'j': {
            auto i = std::next(pa.PA_SOURCE_OUTPUTS.find(selected_output_index), 1);

            if (i != pa.PA_SOURCE_OUTPUTS.end()) {
                selected_output_index = i->first;
            }

            break;
        }

        // case 'h':
        //     pa.set_input_volume(selected_output_index, -1);

        //     break;

        // case 'l':
        //     pa.set_input_volume(selected_output_index, 1);

        //     break;

        // case '\t': {
        //     auto i = pa.PA_SOURCE_OUTPUTS.find(selected_output_index);

        //     if (i != pa.PA_SOURCE_OUTPUTS.end()) {
        //         auto current_sink = pa.PA_SOURCE_OUTPUTS.find(i->second.sink);

        //         current_sink = std::next(current_sink, 1);

        //         if (current_sink == pa.PA_SOURCE_OUTPUTS.end()) {
        //             current_sink = pa.PA_SOURCE_OUTPUTS.begin();
        //         }

        //         pa.move_input_sink(selected_output_index, current_sink->first);
        //     }
        //     break;
        // }
    }

}

void Recording::draw(int w, int h)
{
    int baseY = 3;

    for (auto &i : pa.PA_SOURCE_OUTPUTS) {
        float perc = static_cast<float>(i.second.volume) /
                      (PA_VOLUME_NORM * 1.5f);

        volumeBar(w, h, 0, baseY, perc, i.second.peak);

        if (i.first == selected_output_index) {
            attron(COLOR_PAIR(1));
        }

        char label[255] = {0};
        char app[255] = {0};

        if(strlen(i.second.app_name) > 0) {
            snprintf(
                app,
                sizeof(app),
                "%s : %s",
                i.second.app_name,
                i.second.name
            );
        } else {
            snprintf(app, sizeof(app), "%s", i.second.name);
        }

        if (i.second.mute) {
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
        char *name = pa.PA_SOURCES.find(i.second.source)->second.name;

        unsigned int len = strlen(name);
        unsigned int sink_pos = w - 1 - len;

        mvaddstr(
            baseY - 2,
            sink_pos,
            name
        );

        if (i.first == selected_output_index) {
            attroff(COLOR_PAIR(1));
        }

        baseY += 5;
    }

}

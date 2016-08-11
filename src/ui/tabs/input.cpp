#include "input.hpp"

#include <string.h>
#include <ncurses.h>
#include <inttypes.h>

#include "pa.hpp"

Input::Input()
{
    object = &pa.PA_SOURCES;
    toggle = nullptr;

    selected_index = pa.exists(*object, -1);
}

Input::~Input()
{

}

void Input::draw(int w, int h)
{
    int baseY = 3;

    for (auto &i : pa.PA_SOURCES) {
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

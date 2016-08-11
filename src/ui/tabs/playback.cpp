#include "playback.hpp"

#include <string.h>
#include <ncurses.h>
#include <inttypes.h>

#include <vector>
#include <algorithm>

#include "pa.hpp"

Playback::Playback()
{
    object = &pa.PA_INPUTS;
    toggle = &pa.PA_SINKS;

    selected_index = pa.exists(*object, -1);
}

Playback::~Playback()
{

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

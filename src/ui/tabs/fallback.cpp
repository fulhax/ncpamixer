#include "fallback.hpp"

#include <ncurses.h>

#include <cstring>

#include "ui/ui.hpp"

void Fallback::draw()
{
    static const char fallback[] = {
        "Establishing connection to PulseAudio. Please wait..."
    };

    wattron(ui.window, COLOR_PAIR(COLOR_DEFAULT));
    mvwaddstr(
        ui.window,
        (ui.height / 2) - 1,
        ui.width / 2 - strlen(fallback) / 2,
        fallback
    );
    wattroff(ui.window, COLOR_PAIR(COLOR_DEFAULT));
}

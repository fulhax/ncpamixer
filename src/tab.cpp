#include "tab.hpp"

#include <ncurses.h>

void Tab::volumeBar(int w, int h, int px, int py, float vol, float peak)
{
    // light      ░ \u2593
    // medium     ▒ \u2592
    // dark shade ▓ \u2593
    // block      █ \u2588
    // lower      ▁ \u2581
    // higher     ▔ \u2594
    // triangle   ▲ \u25b2
    // https://en.wikipedia.org/wiki/Block_Elements

    float dw = static_cast<float>(w);

    int pw = dw * peak;
    int vw = dw * vol;
    int fw = w - pw;

    unsigned int color;

    fillW(w, h, 0, py - 1, "\u2581");

    for (int i = 0; i < vw; i++) {
        color = getVolumeColor(
                    (static_cast<float>(i) / w) * 100
                );
        attron(COLOR_PAIR(color));
        mvaddstr(py, i, "\u2588");
        attroff(COLOR_PAIR(color));
    }

    for (int i = 0; i < fw; i++) {
        color = getVolumeColor(
                    (static_cast<float>(pw + i) / w) * 100
                );
        attron(COLOR_PAIR(color + 3));
        mvaddstr(py, pw + i, "\u2593");
        attroff(COLOR_PAIR(color + 3));
    }

    fillW(w, h, 0, py + 1, "\u2594");
    mvaddstr(py, vw - 1, "\u2588"); // Mark volume

}

unsigned int Tab::getVolumeColor(int p)
{
    if (p < 33) {
        return 1;
    } else if (p >= 33 && p < 66) {
        return 2;
    } else {
        return 3;
    }
}

void Tab::fillW(int w, int h, int offset_x, int offset_y, const char *str)
{
    int wo = (w - offset_x);

    for (int i = 0; i < wo; i++) {
        mvaddstr(offset_y, offset_x + i, str);
    }
}

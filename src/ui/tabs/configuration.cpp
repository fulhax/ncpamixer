#include "configuration.hpp"

#include <string.h>
#include <ncurses.h>
#include <inttypes.h>

#include "pa.hpp"

Configuration::Configuration()
{
    object = &pa.PA_CARDS;
    toggle = nullptr;

    selected_index = pa.exists(*object, -1);
}

Configuration::~Configuration()
{

}

void Configuration::draw(int w, int h)
{
    int sx = 1;
    int sy = 1;
    for (auto &i : pa.PA_CARDS) {
        PaCard *card = reinterpret_cast<PaCard*>(i.second);

        mvaddstr(sy, sx, card->name);

        if (i.first == selected_index) {
            attron(COLOR_PAIR(1));
        }
        if(card->active_profile != nullptr){
            mvaddstr(sy + 2, sx + 2, card->active_profile->description);
            borderBox(w-2, 2, sx, sy+1);
        }

        if (i.first == selected_index) {
            attroff(COLOR_PAIR(1));
        }

        sy += 5;
    }
}

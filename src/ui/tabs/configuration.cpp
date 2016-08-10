#include "configuration.hpp"

#include <string.h>
#include <ncurses.h>
#include <inttypes.h>

#include "pa.hpp"

Configuration::Configuration()
{
}

Configuration::~Configuration()
{

}

void Configuration::handleInput(int input)
{
}

void Configuration::draw(int w, int h)
{
    mvaddstr(1, 1, "GK104 HDMI Audio Controller");
    selectBox(w-2, 1, 2, true);

    mvaddstr(6, 1, "GK104 HDMI Audio Controller");
    selectBox(w-2, 1, 7, false);

    mvaddstr(11, 1, "GK104 HDMI Audio Controller");
    selectBox(w-2, 1, 12, false);
}

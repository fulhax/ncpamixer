#include "configuration.hpp"

#include <string.h>
#include <ncurses.h>
#include <inttypes.h>

#include "pa.hpp"
#include "ui/ui.hpp"

Configuration::Configuration()
{
    object = &pulse.PA_CARDS;
    toggle = nullptr;

    selected_index = pulse.exists(*object, -1);

    has_volume = false;
}

Configuration::~Configuration()
{

}

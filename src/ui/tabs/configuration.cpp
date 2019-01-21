#include "configuration.hpp"

#include <ncurses.h>

#include <cinttypes>
#include <cstring>

#include "pa.hpp"
#include "ui/ui.hpp"

Configuration::Configuration()
{
    object = &pulse.PA_CARDS;
    toggle = nullptr;

    selected_index = pulse.exists(*object, -1);

    has_volume = false;
}

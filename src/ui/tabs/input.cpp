#include "input.hpp"

#include <ncurses.h>

#include <cstring>
#include <cinttypes>

#include "pa.hpp"

Input::Input()
{
    object = &pulse.PA_SOURCES;
    toggle = nullptr;

    selected_index = pulse.exists(*object, -1);
}

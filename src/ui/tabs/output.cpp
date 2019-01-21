#include "output.hpp"

#include <ncurses.h>

#include <cinttypes>
#include <cstring>

#include "pa.hpp"

Output::Output()
{
    object = &pulse.PA_SINKS;
    toggle = nullptr;

    selected_index = pulse.exists(*object, -1);
}

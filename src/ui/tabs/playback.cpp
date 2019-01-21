#include "playback.hpp"

#include <ncurses.h>

#include <algorithm>
#include <cinttypes>
#include <cstring>
#include <vector>

#include "pa.hpp"

Playback::Playback()
{
    object = &pulse.PA_INPUTS;
    toggle = &pulse.PA_SINKS;

    selected_index = pulse.exists(*object, -1);
}

#include "recording.hpp"

#include <ncurses.h>

#include <cinttypes>
#include <cstring>

#include "pa.hpp"

Recording::Recording()
{
    object = &pulse.PA_SOURCE_OUTPUTS;
    toggle = &pulse.PA_SOURCES;

    selected_index = pulse.exists(*object, -1);
}

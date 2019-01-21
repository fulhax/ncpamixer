#include "playback.hpp"

#include <string.h>
#include <ncurses.h>
#include <inttypes.h>

#include <vector>
#include <algorithm>

#include "pa.hpp"

Playback::Playback()
{
    object = &pulse.PA_INPUTS;
    toggle = &pulse.PA_SINKS;

    selected_index = pulse.exists(*object, -1);
}

Playback::~Playback()
{

}

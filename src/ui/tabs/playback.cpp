#include "playback.hpp"

#include <string.h>
#include <ncurses.h>
#include <inttypes.h>

#include <vector>
#include <algorithm>

#include "pa.hpp"

Playback::Playback()
{
    object = &pa.PA_INPUTS;
    toggle = &pa.PA_SINKS;

    selected_index = pa.exists(*object, -1);
}

Playback::~Playback()
{

}

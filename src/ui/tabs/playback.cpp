#include "playback.hpp"

#include <ncurses.h>

#include <algorithm>
#include <cinttypes>
#include <cstring>
#include <vector>

#include <audio.hpp>

Playback::Playback()
{
    object = &pulse.PA_INPUTS;
    toggle = &pulse.PA_SINKS;

    selected_index = audio->exists(*object, -1);
}

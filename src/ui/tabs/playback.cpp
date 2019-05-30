#include "playback.hpp"

#include <ncurses.h>

#include <algorithm>
#include <cinttypes>
#include <cstring>
#include <vector>

#include <audio.hpp>

Playback::Playback()
{
    object = audio->getInputs();
    toggle = audio->getSinks();

    selected_index = audio->exists(*object, -1);
}

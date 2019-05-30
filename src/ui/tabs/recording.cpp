#include "recording.hpp"

#include <ncurses.h>

#include <cinttypes>
#include <cstring>

#include <audio.hpp>

Recording::Recording()
{
    object = audio->getSourceOutputs();
    toggle = audio->getSources();

    selected_index = audio->exists(*object, -1);
}

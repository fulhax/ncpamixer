#include "output.hpp"

#include <ncurses.h>

#include <cinttypes>
#include <cstring>

#include <audio.hpp>

Output::Output()
{
    object = audio->getSinks();
    toggle = nullptr;

    selected_index = audio->exists(*object, -1);
}

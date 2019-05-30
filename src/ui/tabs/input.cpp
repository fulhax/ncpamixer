#include "input.hpp"

#include <ncurses.h>

#include <cstring>
#include <cinttypes>

#include <audio.hpp>

Input::Input()
{
    object = audio->getSources();
    toggle = nullptr;

    selected_index = audio->exists(*object, -1);
}

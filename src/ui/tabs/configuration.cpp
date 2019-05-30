#include "configuration.hpp"

#include <ncurses.h>

#include <cinttypes>
#include <cstring>

#include <audio.hpp>

#include "ui/ui.hpp"

Configuration::Configuration()
{
    object = audio->getCards();
    toggle = nullptr;

    selected_index = audio->exists(*object, -1);

    has_volume = false;
}

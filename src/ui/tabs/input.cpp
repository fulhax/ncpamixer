#include "input.hpp"

#include <string.h>
#include <ncurses.h>
#include <inttypes.h>

#include "pa.hpp"

Input::Input()
{
    object = &pulse.PA_SOURCES;
    toggle = nullptr;

    selected_index = pulse.exists(*object, -1);
}

Input::~Input()
{

}

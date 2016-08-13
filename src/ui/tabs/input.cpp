#include "input.hpp"

#include <string.h>
#include <ncurses.h>
#include <inttypes.h>

#include "pa.hpp"

Input::Input()
{
    object = &pa.PA_SOURCES;
    toggle = nullptr;

    selected_index = pa.exists(*object, -1);
}

Input::~Input()
{

}

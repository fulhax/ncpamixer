#include "output.hpp"

#include <string.h>
#include <ncurses.h>
#include <inttypes.h>

#include "pa.hpp"

Output::Output()
{
    object = &pa.PA_SINKS;
    toggle = nullptr;

    selected_index = pa.exists(*object, -1);
}

Output::~Output()
{

}

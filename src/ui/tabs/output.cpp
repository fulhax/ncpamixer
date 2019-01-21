#include "output.hpp"

#include <string.h>
#include <ncurses.h>
#include <inttypes.h>

#include "pa.hpp"

Output::Output()
{
    object = &pulse.PA_SINKS;
    toggle = nullptr;

    selected_index = pulse.exists(*object, -1);
}

Output::~Output()
{

}

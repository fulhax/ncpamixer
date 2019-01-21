#include "recording.hpp"

#include <string.h>
#include <ncurses.h>
#include <inttypes.h>

#include "pa.hpp"

Recording::Recording()
{
    object = &pulse.PA_SOURCE_OUTPUTS;
    toggle = &pulse.PA_SOURCES;

    selected_index = pulse.exists(*object, -1);
}

Recording::~Recording()
{

}

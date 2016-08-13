#include "recording.hpp"

#include <string.h>
#include <ncurses.h>
#include <inttypes.h>

#include "pa.hpp"

Recording::Recording()
{
    object = &pa.PA_SOURCE_OUTPUTS;
    toggle = &pa.PA_SOURCES;

    selected_index = pa.exists(*object, -1);
}

Recording::~Recording()
{

}

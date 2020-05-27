#include "pa_input.hpp"

#include <string.h>

PaInput::PaInput()
{
    type = pa_object_t::INPUT;
    monitor_stream = nullptr;
    pa_set_volume = &pa_context_set_sink_input_volume;
    pa_set_mute = &pa_context_set_sink_input_mute;
    pa_move = &pa_context_move_sink_input_by_index;
    pa_set_active_attribute = nullptr;
    pa_set_default = nullptr;

    sink = 0;
    app_name = nullptr;
}

PaInput::~PaInput()
{
    if(app_name)
        free(app_name);
}

void PaInput::setAppName(const char *name)
{
    if(app_name) {
        free(app_name);
        app_name = nullptr;
    }

    if(name)
        app_name = strdup(name);
}

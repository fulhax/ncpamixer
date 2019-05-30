#include "pa_input.hpp"

#include <cstring>

PaInput::PaInput():app_name("")
{
    pa_set_volume = &pa_context_set_sink_input_volume;
    pa_set_mute = &pa_context_set_sink_input_mute;
    pa_move = &pa_context_move_sink_input_by_index;
    pa_set_active_attribute = nullptr;
    pa_set_default = nullptr;

    sink = 0;
}

pa_object_t PaInput::getType()
{
    return pa_object_t::INPUT;
}

uint32_t PaInput::getRelation()
{
    return sink;
}

void PaInput::setRelation(uint32_t relation)
{
    this->sink = relation;
}

std::string PaInput::getAppName()
{
    return app_name;
}

void PaInput::setAppName(std::string app_name)
{
    this->app_name = std::move(app_name);
}

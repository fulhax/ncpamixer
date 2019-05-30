#include "pa_source_output.hpp"

PaSourceOutput::PaSourceOutput()
{
    pa_set_volume = &pa_context_set_source_output_volume;
    pa_set_mute = &pa_context_set_source_output_mute;
    pa_move = &pa_context_move_source_output_by_index;
    pa_set_active_attribute = nullptr;
    pa_set_default = nullptr;

    source = 0;
}

pa_object_t PaSourceOutput::getType()
{
    return pa_object_t::SOURCE_OUTPUT;
}

uint32_t PaSourceOutput::getRelation()
{
    return source;
}

void PaSourceOutput::setRelation(uint32_t relation)
{
    source = relation;
}

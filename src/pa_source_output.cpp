#include "pa_source_output.hpp"

PaSourceOutput::PaSourceOutput()
{
    type = pa_object_t::SOURCE_OUTPUT;
    monitor_stream = nullptr;

    pa_set_volume = &pa_context_set_source_output_volume;
    pa_set_mute = &pa_context_set_source_output_mute;
    pa_move = &pa_context_move_source_output_by_index;
    pa_set_profile = nullptr;
}

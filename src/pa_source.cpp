#include "pa_source.hpp"

PaSource::PaSource()
{
    type = pa_object_t::SOURCE;
    monitor_stream = nullptr;

    pa_set_volume = &pa_context_set_source_volume_by_index;
    pa_set_mute = &pa_context_set_source_mute_by_index ;
    pa_move = nullptr;
    pa_set_profile = nullptr;
}

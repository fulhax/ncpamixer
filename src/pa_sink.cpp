#include "pa_sink.hpp"

PaSink::PaSink()
{
    type = pa_object_t::SINK;
    monitor_stream = nullptr;

    pa_set_volume = &pa_context_set_sink_volume_by_index;
    pa_set_mute = &pa_context_set_sink_mute_by_index;
    pa_move = nullptr;
}

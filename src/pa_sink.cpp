#include "pa_sink.hpp"

PaSink::PaSink()
{
    type = pa_object_t::SINK;
    monitor_stream = nullptr;
}

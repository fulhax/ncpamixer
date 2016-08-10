#include "pa_source.hpp"

PaSource::PaSource()
{
    type = pa_object_t::SOURCE;
    monitor_stream = nullptr;
}

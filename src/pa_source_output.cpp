#include "pa_source.hpp"

PaSourceOutput::PaSourceOutput()
{
    type = pa_object_t::SOURCE_OUTPUT;
    monitor_stream = nullptr;
}

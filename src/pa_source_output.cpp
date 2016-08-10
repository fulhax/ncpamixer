#include "pa_source_output.hpp"

PaSourceOutput::PaSourceOutput()
{
    type = pa_object_t::SOURCE_OUTPUT;
    monitor_stream = nullptr;
}

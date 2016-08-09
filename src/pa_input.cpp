#include "pa_input.hpp"

PaInput::PaInput()
{
    type = pa_object_t::INPUT;
    monitor_stream = nullptr;
}

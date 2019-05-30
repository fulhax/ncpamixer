#include "pa_sink.hpp"

PaSink::PaSink()
{
    pa_set_volume = &pa_context_set_sink_volume_by_index;
    pa_set_mute = &pa_context_set_sink_mute_by_index;
    pa_move = nullptr;
    pa_set_active_attribute = &pa_context_set_sink_port_by_index;
    pa_set_default = &pa_context_set_default_sink;
}

pa_object_t PaSink::getType()
{
    return pa_object_t::CARD;
}

void PaSink::updatePorts(pa_sink_port_info **info, uint32_t n_ports)
{
    clearAttributes();

    for (uint32_t i = 0; i < n_ports; i++) {
        auto p = new AudioObjectAttribute;
        p->name = &info[i]->name[0];
        p->description = &info[i]->description[0];

        addAttribute(p);
    }
}

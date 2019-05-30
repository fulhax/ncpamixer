#include "pa_source.hpp"

#include <cstdio>

PaSource::PaSource()
{
    pa_set_volume = &pa_context_set_source_volume_by_index;
    pa_set_mute = &pa_context_set_source_mute_by_index ;
    pa_move = nullptr;
    pa_set_active_attribute = &pa_context_set_source_port_by_index;
    pa_set_default = &pa_context_set_default_source;
}

pa_object_t PaSource::getType()
{
    return pa_object_t::SOURCE;
}

void PaSource::updatePorts(pa_source_port_info **info, uint32_t n_ports)
{
    clearAttributes();

    for (uint32_t i = 0; i < n_ports; i++) {
        auto *p = new AudioObjectAttribute;
        p->name = &info[i]->name[0];
        p->description = &info[i]->description[0];

        addAttribute(p);
    }
}

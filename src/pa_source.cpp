#include "pa_source.hpp"
#include <stdio.h>

PaSource::PaSource()
{
    type = pa_object_t::SOURCE;
    monitor_stream = nullptr;

    pa_set_volume = &pa_context_set_source_volume_by_index;
    pa_set_mute = &pa_context_set_source_mute_by_index ;
    pa_move = nullptr;
    pa_set_profile = nullptr;
}

void PaSource::updatePorts(pa_source_port_info **info, uint32_t n_ports)
{
    clearAttributes();

    for (uint32_t i = 0; i < n_ports; i++) {
        PaPort *p = new PaPort;
        snprintf(p->name, sizeof(p->name), "%s", info[i]->name);
        snprintf(
            p->description,
            sizeof(p->description),
            "%s",
            info[i]->description
        );
        attributes.push_back(p);
    }
}

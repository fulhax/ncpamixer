#include "pa_sink.hpp"
#include <stdio.h>

PaSink::PaSink()
{
    type = pa_object_t::SINK;
    monitor_stream = nullptr;

    pa_set_volume = &pa_context_set_sink_volume_by_index;
    pa_set_mute = &pa_context_set_sink_mute_by_index;
    pa_move = nullptr;
    pa_set_active_attribute = pa_context_set_sink_port_by_index;
}



void PaSink::updatePorts(pa_sink_port_info **info, uint32_t n_ports)
{
    clearPorts();
    for (uint32_t i = 0; i < n_ports; i++) {
        PaPort *p = new PaPort;
        snprintf(p->name, sizeof(p->name), "%s", info[i]->name);
        snprintf(p->description, sizeof(p->description), "%s",
             info[i]->description);
       ports.push_back(p); 
    }
}

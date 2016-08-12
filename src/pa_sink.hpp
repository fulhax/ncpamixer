#ifndef PA_SINK_
#define PA_SINK_
#include <vector>
#include "pa_object.hpp"
#include "pa_port.hpp"

class PaSink : public PaObject
{
public:
    PaSink();
    void updatePorts(pa_sink_port_info **info, uint32_t n_ports);
};

#endif // PA_SINK_

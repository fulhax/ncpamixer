#ifndef PA_SINK_
#define PA_SINK_

#include <vector>

#include <pulseaudio/pa_object.hpp>
#include <pulseaudio/pa_port.hpp>

class PaSink : public PaObject
{
public:
    PaSink();
    ~PaSink() final = default;

    pa_object_t getType() override;

    void updatePorts(pa_sink_port_info **info, uint32_t n_ports);
};

#endif // PA_SINK_
